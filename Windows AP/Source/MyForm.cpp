#include <stdio.h>
#include <time.h>
#include "MyForm.h"
#include "Utility.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Threading;
using namespace VirtualInstrument;

extern int roiType;

MyForm::MyForm()
{
	InitializeComponent();
	roiX = roiY = roiW = roiH = 0;
    //pCharInfo = calloc(16, sizeof(SEG_CHAR_INFO));
}

MyForm::~MyForm()
{
	if (components)
	{
		delete components;
	}
}

Void MyForm::MyForm_Load(System::Object^  sender, System::EventArgs^  e)
{
	capture->Enabled = false;
	video->Enabled = false;
	roiDigit->Enabled = false;
	roiWave->Enabled = false;
    roiHand->Enabled = false;
	ok->Enabled = false;
    textBox_R->Enabled = false;
    textBox_G->Enabled = false;
    textBox_B->Enabled = false;
	isRoi = false;

    //20180729 Simon: Set RGB filter to 255 by default 
    textBox_R->Text = "255";
    textBox_G->Text = "255";
    textBox_B->Text = "255";
}

Void MyForm::connect_Click(System::Object^  sender, System::EventArgs^  e)
{
	if (Utility::connectCamera()) {
		capture->Enabled = true;
		connect->Enabled = false;
		video->Enabled = true;
		out_message->Text = "get device\n";
	}
	else {
		//capture->Enabled = false;
		//video->Enabled = false;
		out_message->Text = "fail to get device\n";
	}

	//System::Drawing::Rectangle theRectangle = System::Drawing::Rectangle::Rectangle(50, 50, 200, 200);
	//ControlPaint::DrawReversibleFrame(theRectangle, Color::FromName("Red"), FrameStyle::Thick);
}

void MyForm::UpdateImage()
{
	if (this->pictureBox1->InvokeRequired)
	{
		UpdateImageDelegate^ d =
			gcnew UpdateImageDelegate(this, &MyForm::UpdateImage);
		this->Invoke(d);
	}
	else
	{
		pictureBox1->Image = resultImage;
	}
}

Void MyForm::capture_Click(System::Object^  sender, System::EventArgs^  e)
{
    MyForm::captureImg();

    roiDigit->Enabled = true;
    roiWave->Enabled = true;
    roiHand->Enabled = true;
}

Void MyForm::captureImg() 
{
	unsigned char *pData = NULL, *cData = NULL, *pin;
	int nc, nr;
	clock_t start, end;

	printf("[capture_Click] isRoi:%d\n", isRoi);

	if (isRoi && TRANSFER_ROI_IMAGE)
	{
		nr = roiH;
		nc = roiW;
	}
	else
		Utility::init(&nr, &nc);

	resultImage = gcnew Bitmap(nc, nr, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
	rect = System::Drawing::Rectangle(0, 0, nc, nr);

	cData = (unsigned char*)calloc(nr * nc * 3, sizeof(unsigned char));
	if (!cData)
		return;
	
	pData = (unsigned char*)calloc(nr * nc, sizeof(unsigned char));
	if (!pData)
		return;

	start = clock();

	Utility::startCapture();

	if (Utility::captureImage(pData, nc, nr, 50, isRoi)!=0)
		return;

    //2018/01/27 Simon : ROI after image processing is binarization
    if (isRoi && TRANSFER_ROI_IMAGE)
        memcpy(cData, pData, nr * nc);
    else
        Utility::bayer2rgb(pData, cData, nr, nc);
	
    //20180405 Simon: Get the result of PNN
    /*
    if (isRoi && roiType == ROI_TYPE_DIGIT)
    {
        char str[128];
        sprintf(str, "[Digit] result:%d \n", pData[0]);

        //out_message->Text = gcnew String(str);
        MyForm::setOutMessage(gcnew String(str));
    }
    else if (isRoi && roiType == ROI_TYPE_WAVE)
    {
        char str[128];
        sprintf(str, "[Wave] result:%d \n", pData[0]);

        //out_message->Text = gcnew String(str);
        MyForm::setOutMessage(gcnew String(str));
    }
    else if (isRoi && roiType == ROI_TYPE_HAND)
    {
        char str[128];
        sprintf(str, "[Hand] result:%d \n", pData[0]);

        //out_message->Text = gcnew String(str);
        MyForm::setOutMessage(gcnew String(str));
    }
    */
    if (isRoi)
    {
        char str[128];

        sprintf(str, " Result: %d \n", pData[0]);

        MyForm::setOutMessage(gcnew String(str));
    }

    printf("The time = %fs\n", (double)(end - start) / CLOCKS_PER_SEC);


	free(pData);

	end = clock();
	printf("The time = %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

	//Lock欲處理的像素範圍(避免其他程序修改該向素值)，參數一為限定像素處理範圍，參數二為設定處理模式(讀取、寫入、讀取寫入，第三個像素為設定該像素模式bit? channel?)
	resultImageData = resultImage->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format24bppRgb);

	//將int指標指向Image像素資料的最前面位置
	resultPtr = resultImageData->Scan0;

	//設定指標
	pin = cData;
	pout = (Byte*)((Void*)resultPtr);

	//巡迴每一個像素
	for (int y = 0; y < nr; y++) {
		for (int x = 0; x < nc; x++) {
            //2018/01/27 Simon : ROI after image processing is binarization
            if (isRoi && TRANSFER_ROI_IMAGE == 1)
            {
                pout[0] = pout[1] = pout[2] = (Byte)*pin;
                pin++;
            }
            else
            {
                //像素值填入
                pout[0] = (Byte)*pin;
                pout[1] = (Byte)*(pin + 1);
                pout[2] = (Byte)*(pin + 2);
                pin += 3;
            }
			//指到下一個像素資訊
			pout += 3;
		}
	}

	//Unlock處理完畢的像素範圍
	resultImage->UnlockBits(resultImageData);

	//將影像顯示在pictureBox
	UpdateImage();

    //20180316 Simon: Gather information for PNP data
    /*
    if (isRoi && roiType == ROI_TYPE_DIGIT)
        Utility::pnpDataCollect(cData, roiH, roiW);
    */

    //20180729 Simon: Print the value of ROI for testing
    /*
    //test-->
    if (isRoi)
    {
    printf("============================================================\n");
    printf("nr=%d,nc=%d\n",nr,nc);

    printf("    ");
    for(int i =0;i<nc+1;i++)
    printf("%3d ", i);
    printf("\n");

    unsigned char * p = cData;
    for (int i = 0; i < nr; i++)
    {
    printf("%3d ", i);

    for (int j = 0; j < nc; j++)
    {
    printf("%3d ",*p);
    p++;
    }
    printf("\n");
    }
    printf("============================================================\n");
    }
    //test<--
    */

	free(cData);
/*
	roiDigit->Enabled = true;
	roiWave->Enabled = true;
    roiHand->Enabled = true;
*/
}

void MyForm::ThreadMethod(/*Object^ state*/)
{
	unsigned char *pData = NULL, *cData = NULL, *pin;
	int nc, nr;
	clock_t start, end;

	Utility::init(&nr, &nc);
	resultImage = gcnew Bitmap(nc, nr, System::Drawing::Imaging::PixelFormat::Format24bppRgb);
	rect = System::Drawing::Rectangle(0, 0, nc, nr);

	pData = (unsigned char*)calloc(nr * nc, sizeof(unsigned char));
	if (!pData)
		return;

	cData = (unsigned char*)calloc(nr * nc * 3, sizeof(unsigned char));
	if (!cData)
		return;

	while (1) {

		start = clock();

		Utility::startCapture();

		if (Utility::captureImage(pData, nr, nc, 5000 ,isRoi) != 0)
			continue;

		Utility::bayer2rgb(pData, cData, nr, nc);

		end = clock();
		printf("The time = %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

		//Lock欲處理的像素範圍(避免其他程序修改該向素值)，參數一為限定像素處理範圍，參數二為設定處理模式(讀取、寫入、讀取寫入，第三個像素為設定該像素模式bit? channel?)
		resultImageData = resultImage->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format24bppRgb);

		//將int指標指向Image像素資料的最前面位置
		resultPtr = resultImageData->Scan0;

		//設定指標
		pin = cData;
		pout = (Byte*)((Void*)resultPtr);

		//巡迴每一個像素
		for (int y = 0; y < nr; y++) {
			for (int x = 0; x < nc; x++) {
				//像素值填入
				pout[0] = (Byte)*pin;
				pout[1] = (Byte)*(pin + 1);
				pout[2] = (Byte)*(pin + 2);
				pin += 3;

				//指到下一個像素資訊
				pout += 3;
			}
		}

		//Unlock處理完畢的像素範圍
		resultImage->UnlockBits(resultImageData);

		//將影像顯示在pictureBox
		//pictureBox1->Image = resultImage;
		UpdateImage();
	}

	free(pData);
	free(cData);
}

Void MyForm::video_Click(System::Object^  sender, System::EventArgs^  e) {
	//disable other button
	if (video->Text == "video") {
		capture->Enabled = false;
		video->Text = "stop video thread";
		oThread = gcnew Thread(gcnew ThreadStart(this, &MyForm::ThreadMethod));
		oThread->Start();
	}
	else {
		capture->Enabled = true;
		video->Text = "video";
		oThread->Abort();
	}
}

void MyForm::autorunThread(/*Object^ state*/)
{
    while (1)
    {
        MyForm::captureImg();

        Utility::screenCapture();

        Sleep(10000); //10 seconds

    }
}

Void MyForm::ok_Click(System::Object^  sender, System::EventArgs^  e) 
{
    int rNum = 0, gNum = 0, bNum=0;

	printf("[ok_Click]\n");

	roiDigit->Enabled = false;
	roiWave->Enabled = false;
    roiHand->Enabled = false;

    //20180414 Simon: Only digit neeeds to filter RGB
    if (roiType == ROI_TYPE_DIGIT)
    {
        if (textBox_R->TextLength == 0 || textBox_G->TextLength == 0 || textBox_B->TextLength == 0)
        {
            out_message->Text = "Error: The value within RGB filter must be 0~255\n";

            return;
        }

        rNum = Convert::ToInt32(textBox_R->Text);
        gNum = Convert::ToInt32(textBox_G->Text);
        bNum = Convert::ToInt32(textBox_B->Text);

        if (rNum < 0 || rNum>255 || gNum < 0 || gNum>255 || bNum < 0 || bNum>255)
        {
            out_message->Text = "Error: The value within RGB filter must be 0~255\n";

            return;
        }
        else
        {
            out_message->Text = "";
            printf("RGB filter: %d,%d,%d\n", rNum, gNum, bNum);
        }
    }

    if (isRoi != true)
    {
        Utility::sendRoiInfo(roiX, roiY, roiW, roiH, rNum, gNum, bNum);
        isRoi = true;
    }

    oThread = gcnew Thread(gcnew ThreadStart(this, &MyForm::autorunThread));
    oThread->Start();
}

Void MyForm::roiDigit_Click(System::Object^  sender, System::EventArgs^  e) 
{
	roiWave->Enabled = false;
    roiHand->Enabled = false;

    roiType = ROI_TYPE_DIGIT;
}

System::Void MyForm::roiWave_Click(System::Object^  sender, System::EventArgs^  e) 
{
	roiDigit->Enabled = false;
    roiHand->Enabled = false;

    roiType = ROI_TYPE_WAVE;
}

System::Void MyForm::roiHand_Click(System::Object^  sender, System::EventArgs^  e)
{
    roiDigit->Enabled = false;
    roiWave->Enabled = false;

    roiType = ROI_TYPE_HAND;
}

System::Void MyForm::pictureBox1_Click(System::Object^  sender, System::EventArgs^  e) 
{

}

Void MyForm::panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
{

}

void MyForm::pictureBox1_MouseDown(Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
{

	if (roiDigit->Enabled != true && roiWave->Enabled != true && roiHand->Enabled != true)
		return;

	printf("MouseDown(X:%d,Y:%d)\n",e->X, e->Y);

	// Set the isDrag variable to true and get the starting point 
	// by using the PointToScreen method to convert form 
	// coordinates to screen coordinates.
	if (e->Button == System::Windows::Forms::MouseButtons::Left)
	{
		printf("Reset Rectangle\n");

		// Reset the rectangle.
		//delete(theRectangle);
		theRectangle = System::Drawing::Rectangle::Rectangle(0, 0, 0, 0);


		isDrag = true;

		Control^ control = dynamic_cast<Control^>(sender);

		// Calculate the startPoint by using the PointToScreen 
		// method.
		startPoint = control->PointToScreen(Point(e->X, e->Y));
		mouseEventDown = e;
	}
}

void MyForm::pictureBox1_MouseMove(Object^ /*sender*/, System::Windows::Forms::MouseEventArgs^ e)
{
	//printf(" Form1_MouseMove\n");

	// If the mouse is being dragged, 
	// undraw and redraw the rectangle as the mouse moves.
	if (isDrag)
	{
		ControlPaint::DrawReversibleFrame(theRectangle, Color::Yellow, FrameStyle::Thick);

		// Calculate the endpoint and dimensions for the new 
		// rectangle, again using the PointToScreen method.
		Point endPoint = this->PointToScreen(Point(e->X, e->Y));
		int width = endPoint.X - startPoint.X;
		int height = endPoint.Y - startPoint.Y;
		theRectangle = Rectangle::Rectangle(startPoint.X, startPoint.Y, width, height);

		// Draw the new rectangle by calling DrawReversibleFrame
		// again.  
		ControlPaint::DrawReversibleFrame(theRectangle, Color::Yellow, FrameStyle::Thick);
	}
}

void MyForm::pictureBox1_MouseUp(Object^ /*sender*/, System::Windows::Forms::MouseEventArgs^ e)
{
	// If the MouseUp event occurs, the user is not dragging.
	if (e->Button == System::Windows::Forms::MouseButtons::Left && isDrag == true)
	{
		printf("MouseUp(X:%d,Y:%d)\n",e->X, e->Y);

		isDrag = false;

		ok->Enabled = true;
        textBox_R->Enabled = true;
        textBox_G->Enabled = true;
        textBox_B->Enabled = true;

		if (mouseEventDown->X < e->X)
		{
			roiX = mouseEventDown->X;
			roiW = e->X - roiX;
		}
		else
		{
			roiX = e->X;
			roiW = mouseEventDown->X - roiX;
		}

		if (mouseEventDown->Y < e->Y)
		{
			roiY = mouseEventDown->Y;
			roiH = e->Y - roiY;
		}
		else
		{
			roiY = e->Y;
			roiH = mouseEventDown->Y - roiY;
		}

		//20180103 Simon: A workaround for the abornormal coordinate.
		roiW -= 18;
		roiH -= 18;

		//20171230 Simon: A workaround for a strange issue.It makes the image abornormal.
		if (roiW % 4 != 0)
			roiW = (roiW / 4 + 1) * 4;

        //20180208 Simon: Because of the format is bayer filter
        if (roiH % 2 != 0)
            roiH++;

        if (roiX % 2 != 0)
            roiX--;

        if (roiY % 2 != 0)
            roiY--;

		//Point startPoint = theRectangle.Location;

		char str[128];

		sprintf(str, "roiX=%d , roiY=%d , roiW=%d , roiH=%d\n", roiX, roiY, roiW, roiH);
		out_message->Text = gcnew String(str);
        
        printf("%s", str);
	}
	// Draw the rectangle to be evaluated. Set a dashed frame style 
	// using the FrameStyle enumeration.
	//ControlPaint::DrawReversibleFrame(theRectangle, this->BackColor, FrameStyle::Dashed);

	// Find out which controls intersect the rectangle and 
	// change their color. The method uses the RectangleToScreen  
	// method to convert the Control's client coordinates 
	// to screen coordinates.
	/*
	System::Drawing::Rectangle controlRectangle;
	for (int i = 0; i < Controls->Count; i++)
	{
	controlRectangle = Controls[i]->RectangleToScreen(Controls[i]->ClientRectangle);
	if (controlRectangle.IntersectsWith(theRectangle))
	{
	Controls[i]->BackColor = Color::BurlyWood;
	}

	}
	*/

	// Reset the rectangle.
	//theRectangle = System::Drawing::Rectangle::Rectangle(0, 0, 0, 0);
	//delete(theRectangle);
}

delegate void outMessageDelegate(String^ text);
void MyForm::setOutMessage(String^ text)
{
    if (out_message->InvokeRequired)
    {
        outMessageDelegate^ d =
            gcnew outMessageDelegate(this, &MyForm::setOutMessage);
        this->Invoke(d, gcnew array<Object^> { text });
    }
    else
        out_message->Text = gcnew String(text);
}
/*
#using <System.dll>

using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;


void getSerialPorts()
{
	array<String^>^ serialPorts = nullptr;
	try
	{
		// Get a list of serial port names.
		serialPorts = SerialPort::GetPortNames();
	}
	catch (Win32Exception^ ex)
	{
		Console::WriteLine(ex->Message);
	}

	Console::WriteLine("The following serial ports were found:");

	// Display each port name to the console.
	for each(String^ port in serialPorts)
	{
		Console::WriteLine(port);
	}
}
*/

[STAThread]
int main(){
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	VirtualInstrument::MyForm form;
	Application::Run(%form);
	return 0;
}