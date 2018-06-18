#pragma once

namespace VirtualInstrument {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	/// <summary>
	/// MyForm ���K�n
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm();

	protected:
		~MyForm();

	private:
		System::ComponentModel::Container ^components;
		System::Windows::Forms::Button^  connect;
		System::Windows::Forms::Button^  capture;
		System::Windows::Forms::Button^  ok;
		System::Windows::Forms::Button^  roiDigit;
		System::Windows::Forms::Button^  roiWave;
        System::Windows::Forms::Button^  roiHand;
		System::Windows::Forms::Button^  video;
		System::Windows::Forms::Panel^  panel1;
		System::Windows::Forms::PictureBox^  pictureBox1;
		System::Windows::Forms::Label^  out_message;
        System::Windows::Forms::TextBox^  textBox_R;
        System::Windows::Forms::TextBox^  textBox_G;
        System::Windows::Forms::TextBox^  textBox_B;

        System::Windows::Forms::Label^  label_RGB;
        Bitmap^ resultImage;
		//�ŧi�Ϲ��ϰ�(�]�w����Ū���B���ץ����Ϲ��ϰ�)
		System::Drawing::Rectangle rect;
		//�ŧi���@BitmapData���A�A��ܹ����Ȧ�m
		Imaging::BitmapData ^resultImageData;
		//�ŧi�@int�榡������
		IntPtr resultPtr;
		//�ŧi�@��Byte�榡������
		Byte *pout;
		Thread^ oThread;
		int roiX;
		int roiY;
		int roiW;
		int roiH;
		bool isDrag;
		bool isRoi;

		System::Drawing::Rectangle theRectangle;
		Point startPoint;
		MouseEventArgs^ mouseEventDown;
		delegate void UpdateImageDelegate();
		void UpdateImage();

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �����]�p�u��䴩�һݪ���k - �ФŨϥε{���X�s�边
		/// �ק�o�Ӥ�k�����e�C
		/// </summary>
		void MyForm::InitializeComponent(void)
		{
			this->connect = (gcnew System::Windows::Forms::Button());
			this->capture = (gcnew System::Windows::Forms::Button());
			this->video = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->out_message = (gcnew System::Windows::Forms::Label());
			this->ok = (gcnew System::Windows::Forms::Button());
			this->roiDigit = (gcnew System::Windows::Forms::Button());
			this->roiWave = (gcnew System::Windows::Forms::Button());
            this->roiHand = (gcnew System::Windows::Forms::Button());
            this->textBox_R = (gcnew System::Windows::Forms::TextBox());
            this->textBox_G = (gcnew System::Windows::Forms::TextBox());
            this->textBox_B = (gcnew System::Windows::Forms::TextBox());
            this->label_RGB = (gcnew System::Windows::Forms::Label());
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// connect
			// 
			this->connect->Location = System::Drawing::Point(10, 522);
			this->connect->Name = L"connect";
			this->connect->Size = System::Drawing::Size(65, 23);
			this->connect->TabIndex = 0;
			this->connect->Text = L"connect";
			this->connect->UseVisualStyleBackColor = true;
			this->connect->Click += gcnew System::EventHandler(this, &MyForm::connect_Click);
			// 
			// capture
			// 
			this->capture->Location = System::Drawing::Point(80, 522);
			this->capture->Name = L"capture";
			this->capture->Size = System::Drawing::Size(65, 23);
			this->capture->TabIndex = 1;
			this->capture->Text = L"capture";
			this->capture->UseVisualStyleBackColor = true;
			this->capture->Click += gcnew System::EventHandler(this, &MyForm::capture_Click);
			// 
			// video
			// 
			this->video->Location = System::Drawing::Point(430, 522);
			this->video->Name = L"video";
			this->video->Size = System::Drawing::Size(65, 23);
			this->video->TabIndex = 2;
			this->video->Text = L"video";
			this->video->UseVisualStyleBackColor = true;
			this->video->Click += gcnew System::EventHandler(this, &MyForm::video_Click);
			// 
			// panel1
			// 
			this->panel1->AutoScroll = true;
			this->panel1->Controls->Add(this->pictureBox1);
			this->panel1->Location = System::Drawing::Point(13, 13);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(680, 503);
			this->panel1->TabIndex = 3;
			this->panel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::panel1_Paint);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(4, 4);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->SizeMode = PictureBoxSizeMode::AutoSize;
			this->pictureBox1->Size = System::Drawing::Size(640, 480);
			//this->pictureBox1->TabIndex = 0;
			//this->pictureBox1->TabStop = false;
			//this->pictureBox1->Click += gcnew System::EventHandler(this, &MyForm::pictureBox1_Click);
			this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox1_MouseDown);
			this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox1_MouseMove);
			this->pictureBox1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &MyForm::pictureBox1_MouseUp);
			// 
			// out_message
			// 
			this->out_message->AutoSize = true;
			this->out_message->Location = System::Drawing::Point(10, 560);
			this->out_message->Name = L"out_message";
			this->out_message->Size = System::Drawing::Size(85, 12);
			this->out_message->TabIndex = 4;
			this->out_message->Text = L"output message...";
			// 
			// ok
			// 
			this->ok->Location = System::Drawing::Point(360, 522);
			this->ok->Name = L"ok";
			this->ok->Size = System::Drawing::Size(65, 23);
			this->ok->TabIndex = 5;
			this->ok->Text = L"OK";
			this->ok->UseVisualStyleBackColor = true;
			this->ok->Click += gcnew System::EventHandler(this, &MyForm::ok_Click);
			// 
			// roiDigit
			// 
			this->roiDigit->Location = System::Drawing::Point(150, 522);
			this->roiDigit->Name = L"roiDigit";
			this->roiDigit->Size = System::Drawing::Size(65, 23);
			this->roiDigit->TabIndex = 6;
			this->roiDigit->Text = L"ROI(digit)";
			this->roiDigit->UseVisualStyleBackColor = true;
			this->roiDigit->Click += gcnew System::EventHandler(this, &MyForm::roiDigit_Click);
			// 
			// roiWave
			// 
			this->roiWave->Location = System::Drawing::Point(220, 522);
			this->roiWave->Name = L"roiWave";
			this->roiWave->Size = System::Drawing::Size(65, 23);
			this->roiWave->TabIndex = 7;
			this->roiWave->Text = L"ROI(wave)";
			this->roiWave->UseVisualStyleBackColor = true;
			this->roiWave->Click += gcnew System::EventHandler(this, &MyForm::roiWave_Click);
            // 
            // roiHand
            // 
            this->roiHand->Location = System::Drawing::Point(290, 522);
            this->roiHand->Name = L"roiHand";
            this->roiHand->Size = System::Drawing::Size(65, 23);
            this->roiHand->TabIndex = 8;
            this->roiHand->Text = L"ROI(hand)";
            this->roiHand->UseVisualStyleBackColor = true;
            this->roiHand->Click += gcnew System::EventHandler(this, &MyForm::roiHand_Click);
            // 
            // textBox_R
            // 
            this->textBox_R->Location = System::Drawing::Point(570, 534);
            this->textBox_R->Name = L"textBox_RGB";
            this->textBox_R->Size = System::Drawing::Size(25, 22);
            this->textBox_R->TabIndex = 9;
            // 
            // textBox_G
            // 
            this->textBox_G->Location = System::Drawing::Point(595, 534);
            this->textBox_G->Name = L"textBox_G";
            this->textBox_G->Size = System::Drawing::Size(25, 22);
            this->textBox_G->TabIndex = 10;
            // 
            // textBox_B
            // 
            this->textBox_B->Location = System::Drawing::Point(620, 534);
            this->textBox_B->Name = L"textBox_B";
            this->textBox_B->Size = System::Drawing::Size(25, 22);
            this->textBox_B->TabIndex = 11;
            // 
            // label_RGB
            // 
            this->label_RGB->AutoSize = true;
            this->label_RGB->Location = System::Drawing::Point(550, 522);
            this->label_RGB->Name = L"label_RGB";
            this->label_RGB->Size = System::Drawing::Size(54, 12);
            this->label_RGB->TabIndex = 12;
            this->label_RGB->Text = L"Filter R     G     B";
            // 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(722, 581);
			this->Controls->Add(this->out_message);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->video);
			this->Controls->Add(this->capture);
			this->Controls->Add(this->connect);
			this->Controls->Add(this->ok);
			this->Controls->Add(this->roiDigit);
			this->Controls->Add(this->roiWave);
            this->Controls->Add(this->roiHand);
            this->Controls->Add(this->label_RGB);
            this->Controls->Add(this->textBox_R);
            this->Controls->Add(this->textBox_G);
            this->Controls->Add(this->textBox_B);
            this->Name = L"MyForm";
			this->Text = L"Virtual instrument";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->panel1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion

		System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e);
		System::Void connect_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void capture_Click(System::Object^  sender, System::EventArgs^  e);
        void ThreadMethod(/*Object^ state*/);
		System::Void video_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void ok_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void roiDigit_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void roiWave_Click(System::Object^  sender, System::EventArgs^  e);
        System::Void roiHand_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void pictureBox1_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
		void pictureBox1_MouseDown(Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		void pictureBox1_MouseMove(Object^ /*sender*/, System::Windows::Forms::MouseEventArgs^ e);
		void pictureBox1_MouseUp(Object^ /*sender*/, System::Windows::Forms::MouseEventArgs^ e);
        Void MyForm::captureImg();
        void autorunThread(/*Object^ state*/);
        void setOutMessage(String^ text);
};
}
