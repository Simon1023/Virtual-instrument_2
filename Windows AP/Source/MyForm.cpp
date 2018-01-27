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

MyForm::MyForm()
{
	InitializeComponent();
	roiX = roiY = roiW = roiH = 0;
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
	ok->Enabled = false;
	isRoi = false;
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
	unsigned char *pData = NULL, *cData = NULL, *pin;
	int nc, nr;
	clock_t start, end;

	printf("[capture_Click] isRoi:%d\n", isRoi);

	if (isRoi)
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

	if (Utility::captureImage(pData, nr, nc, 50, isRoi)!=0)
		return;

	Utility::bayer2rgb(pData, cData, nr, nc);
	free(pData);

	end = clock();
	printf("The time = %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

	//Lock���B�z�������d��(�קK��L�{�ǭק�ӦV����)�A�ѼƤ@�����w�����B�z�d��A�ѼƤG���]�w�B�z�Ҧ�(Ū���B�g�J�BŪ���g�J�A�ĤT�ӹ������]�w�ӹ����Ҧ�bit? channel?)
	resultImageData = resultImage->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format24bppRgb);

	//�Nint���Ы��VImage������ƪ��̫e����m
	resultPtr = resultImageData->Scan0;

	//�]�w����
	pin = cData;
	pout = (Byte*)((Void*)resultPtr);

	//���j�C�@�ӹ���
	for (int y = 0; y < nr; y++) {
		for (int x = 0; x < nc; x++) {
			//�����ȶ�J
			pout[0] = (Byte)*pin;
			pout[1] = (Byte)*(pin + 1);
			pout[2] = (Byte)*(pin + 2);
			pin += 3;

			//����U�@�ӹ�����T
			pout += 3;
		}
	}

	//Unlock�B�z�����������d��
	resultImage->UnlockBits(resultImageData);

	//�N�v����ܦbpictureBox
	UpdateImage();

	free(cData);

	roiDigit->Enabled = true;
	roiWave->Enabled = true;
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

		//Lock���B�z�������d��(�קK��L�{�ǭק�ӦV����)�A�ѼƤ@�����w�����B�z�d��A�ѼƤG���]�w�B�z�Ҧ�(Ū���B�g�J�BŪ���g�J�A�ĤT�ӹ������]�w�ӹ����Ҧ�bit? channel?)
		resultImageData = resultImage->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, System::Drawing::Imaging::PixelFormat::Format24bppRgb);

		//�Nint���Ы��VImage������ƪ��̫e����m
		resultPtr = resultImageData->Scan0;

		//�]�w����
		pin = cData;
		pout = (Byte*)((Void*)resultPtr);

		//���j�C�@�ӹ���
		for (int y = 0; y < nr; y++) {
			for (int x = 0; x < nc; x++) {
				//�����ȶ�J
				pout[0] = (Byte)*pin;
				pout[1] = (Byte)*(pin + 1);
				pout[2] = (Byte)*(pin + 2);
				pin += 3;

				//����U�@�ӹ�����T
				pout += 3;
			}
		}

		//Unlock�B�z�����������d��
		resultImage->UnlockBits(resultImageData);

		//�N�v����ܦbpictureBox
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

Void MyForm::ok_Click(System::Object^  sender, System::EventArgs^  e) 
{
	printf("[ok_Click]\n");

	roiDigit->Enabled = false;
	roiWave->Enabled = false;
	isRoi = true;

	Utility::sendRoiInfo(roiX, roiY, roiW, roiH);

	MyForm::capture_Click(sender, e);
}

Void MyForm::roiDigit_Click(System::Object^  sender, System::EventArgs^  e) 
{
	roiWave->Enabled = false;
}

System::Void MyForm::roiWave_Click(System::Object^  sender, System::EventArgs^  e) 
{
	roiDigit->Enabled = false;
}

System::Void MyForm::pictureBox1_Click(System::Object^  sender, System::EventArgs^  e) 
{

}

Void MyForm::panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
{

}

void MyForm::pictureBox1_MouseDown(Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
{

	if (roiDigit->Enabled != true && roiWave->Enabled != true)
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

		//Point startPoint = theRectangle.Location;

		char str[128];

		sprintf(str, "roiX=%d , roiY=%d , roiW=%d , roiH=%d\n", roiX, roiY, roiW, roiH);
		out_message->Text = gcnew String(str);

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