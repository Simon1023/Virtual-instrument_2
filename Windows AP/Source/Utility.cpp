#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <windows.h>
#include "Utility.h"
#include "SCSI\SCSI.h"

#using <System.dll>

using namespace System;
using namespace System::IO::Ports;
using namespace System::ComponentModel;
using namespace std;

HANDLE Utility::fileHandle = NULL;

HANDLE fileHandle;
//HANDLE fileHandle1;
HANDLE fileHandlelist[26];
char drive;
char drivelist[26];
int drive_count;
UCHAR databuffer[65536];

Utility::Utility()
{

}

Utility::~Utility()
{

}

int Utility::init(int *nr, int *nc)
{
	*nc = Utility::imageWidth;
	*nr = Utility::imageHeight;

	return TRUE;
}

int Utility::connectCamera()
{
	int i;
	fileHandle = NULL;
	drive = 0;
	HANDLE fHandle;

	for (i = 0; i<26; i++)
	{
		fileHandlelist[i] = NULL;
		drivelist[i] = 0;
	}
	drive_count = 0;
	char c;
	for (c = 'C'; c <= 'Z'; c++)
	{
		if (find_device(&fHandle, c, "MIAT    STM03V", 8, "STM03V1  DISK2  1.0", 13))
		{
			fileHandlelist[drive_count] = fHandle;
			drivelist[drive_count] = c;
			drive_count++;
		}
	}
	if (drive_count>0)
	{
		fileHandle = fileHandlelist[0];
		drive = drivelist[0];
		return 1;
	}

	return 0;
}

void Utility::disconnectCamera()
{
	int i;
	for (i = 0; i<drive_count; i++)
	{
		CloseHandle(fileHandlelist[i]);
		fileHandlelist[i] = NULL;
		drivelist[i] = 0;
	}
}

//check whether image is ready to read
//Return 0:Not ready; 1:Ready
int Utility::checkImageFlag()
{
	BYTE SCSI_Cmd[9] = { SCSI_CHECK_IMAGE_READY,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };//check flag
	BYTE status;

	UCHAR CMD[CDB12GENERIC_LENGTH];
	int datalength, j;

	//printf("waiting for DCMI ready...\n");

	datalength = 1;
	for (j = 0; j<CDB12GENERIC_LENGTH; j++)
		CMD[j] = SCSI_Cmd[j];

	status = SCSICMD(fileHandle, SCSI_IOCTL_DATA_IN, CMD, datalength, databuffer);

	return databuffer[0];
}

int Utility::startCapture()
{
	BYTE SCSI_Cmd[9] = { SCSI_CLEAR_FLAG,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };//check flag
	BYTE status;

	UCHAR CMD[CDB12GENERIC_LENGTH];
	int datalength, j;

	//printf("clear flag...\n");

	datalength = 1;
	for (j = 0; j<CDB12GENERIC_LENGTH; j++)
		CMD[j] = SCSI_Cmd[j];

	status = SCSICMD(fileHandle, SCSI_IOCTL_DATA_IN, CMD, datalength, databuffer);

	return databuffer[0];
}

//captureImage
int Utility::captureImage(unsigned char* pData,int nc, int nr, int timeout)
{
	BYTE m_byCmd[MAX_CMD_LEN];
	UCHAR CMD[CDB12GENERIC_LENGTH];
	const int perdata = 51200;
	int last_data_len;
	BYTE **SCSI_Cmd;
	BYTE status;

	int i, j, size, t;
	int counter, imageSize = nr * nc;

	//BYTE SCSI_Cmd[2][9]={SCSI_GET_IMAGE,0x00,0x00,0x00,0x00,0x00,0x00, datalength & 0xFF00, datalength & 0x00FF,
	//					 SCSI_GET_IMAGE,0x00,0x00,0x00,0x00,0x00,0x00, datalength & 0xFF00, datalength & 0x00FF};		

	//parameter[2]~[5] for start address
	//parameter[7]~[8] for data length
	size = imageSize / perdata;
	last_data_len = imageSize % perdata;
	if (last_data_len != 0) size++;
	SCSI_Cmd = (BYTE**)calloc(size, sizeof(BYTE*));

	//printf("size=%d\n", size);
	//printf("last_data_len=%d\n", last_data_len);
	for (i = 0; i < size; i++) {
		int start_address = (perdata * i) >> 9;
		SCSI_Cmd[i] = (BYTE*)calloc(9, sizeof(BYTE));
		SCSI_Cmd[i][0] = SCSI_GET_IMAGE;
		SCSI_Cmd[i][2] = ((start_address) & 0xFF000000) >> 24;
		SCSI_Cmd[i][3] = ((start_address) & 0x00FF0000) >> 16;
		SCSI_Cmd[i][4] = ((start_address) & 0x0000FF00) >> 8;
		SCSI_Cmd[i][5] = (start_address) & 0x000000FF;
		SCSI_Cmd[i][7] = ((perdata >> 9) & 0xFF00) >> 8;
		SCSI_Cmd[i][8] = (perdata >> 9) & 0x00FF;
	}

	if (last_data_len != 0) {
		int tmp = (last_data_len >> 9);
		if (last_data_len % 512 != 0) tmp++;
		SCSI_Cmd[size - 1][7] = ((tmp) & 0xFF00) >> 8;
		SCSI_Cmd[size - 1][8] = (tmp) & 0x00FF;
	}

	//printf("get flag...\n");
	i = counter = 0;

	t = clock();
	while (checkImageFlag() == 0 && clock() - t <= timeout * CLOCKS_PER_SEC) {}

	if (clock() - t > timeout * CLOCKS_PER_SEC) {
		printf("timeout\n");

		return NULL;
	}

	//printf("get image...\n");

	while (i < size)
	{
		int datalength = ((SCSI_Cmd[i][7] << 8) | (SCSI_Cmd[i][8])) << 9;
		memcpy(m_byCmd, &SCSI_Cmd[i][0], 9);
		for (j = 0; j<CDB12GENERIC_LENGTH; j++)
			CMD[j] = m_byCmd[j];
		status = SCSICMD(fileHandle, SCSI_IOCTL_DATA_IN, CMD, datalength, databuffer);

		//printf("%d\n", databuffer[0]);
		if (counter + datalength >= imageSize)
		{
			//writewav(sampleRate, fileSize, bps, channel, PCM_data);
			memcpy((pData)+counter, databuffer, imageSize - counter);
		}
		else
		{
			memcpy((pData)+counter, databuffer, datalength);
			counter += datalength;
		}
		i++;
	}
	for (i = 0; i < size; i++) {
		free(SCSI_Cmd[i]);
	}
	free(SCSI_Cmd);

	return 0;
}

//20180109 Simon Windows AP sends the information to PVC mini 
int Utility::sendRoiInfo(USHORT x, USHORT y, USHORT w, USHORT h)
{
	UCHAR command[CDB12GENERIC_LENGTH] = {0};
	BYTE status;
	int datalength=0;

	printf("[sendRoiInfo] x:0x%x, y:0x%x, w:0x%x, h:0x%x\n",x,y,w,h);

	command[0] = SCSI_SEND_ROI_INFO;
	command[1] = x & 0xFF;
	command[2] = x >> 8 & 0xFF;
	command[3] = y & 0xFF;
	command[4] = y >> 8 & 0xFF;
	command[5] = w & 0xFF;
	command[6] = w >> 8 & 0xFF;
	command[7] = h & 0xFF;
	command[8] = h >> 8 & 0xFF;

	status = SCSICMD(fileHandle, SCSI_IOCTL_DATA_OUT, command, datalength, databuffer);

	return TRUE;
}

int Utility::getRoiImage(unsigned char* pData, int type, int nc, int nr)
{
	DWORD	bytesRead = 0;
	DWORD	imageSize = type * nr * nc / 8;
	DWORD	bytesReadTotal = 0;
	const int perData = 16380;

	printf("[getRoiImage]nc:%d,nr:%d\n", nc, nr);

	if (pData == NULL)
	{
		printf("No enough memory space to allocation.\n");

		return FALSE;
	}

	printf("Start to read %d bytes\n", imageSize);

	while (bytesReadTotal < imageSize)
	{
		if (ReadFile(fileHandle, pData + bytesReadTotal, nc, &bytesRead, NULL) != FALSE)
		{
			if (bytesRead > 0)
			{
				bytesReadTotal += bytesRead;

				//printf("Read %d bytes, total:%d\n", bytesRead, bytesReadTotal);
			}
		}
	}

	printf("End to read %d bytes\n", bytesReadTotal);

	return TRUE;
}

void Utility::bayer2rgb(unsigned char *pData, unsigned char *cData, int nr, int nc)
{
	int i, j;
	unsigned char R, G, B;
	int index;

	for (i = 1; i<nr - 1; i++)
	{
		for (j = 1; j<nc - 1; j++)
		{
			index = (i * nc + j);
			if (i % 2 == 1 && j % 2 == 0)	//G
			{
				//					R=(pData[i-1][j-1]+pData[i-1][j+1]+pData[i+1][j-1]+pData[i+1][j+1])/4;
				//					G=(pData[i][j-1]+pData[i][j+1]+pData[i-1][j]+pData[i+1][j])/4;
				//					B=pData[i][j];
				R = (pData[index - 1] + pData[index + 1]) / 2;
				G = pData[index];
				B = (pData[index - nc] + pData[index + nc]) / 2;
				//R = (pData[i][j - 1] + pData[i][j + 1]) / 2;
				//G = pData[i][j];
				//B = (pData[i - 1][j] + pData[i + 1][j]) / 2;
			}
			else if (i % 2 == 1 && j % 2 == 1)	//R
			{
				//					R=(pData[i-1][j]+pData[i+1][j])/2;
				//					G=pData[i][j];
				//					B=(pData[i][j-1]+pData[i][j+1])/2;
				R = pData[index];
				G = (pData[index - nc] + pData[index + nc] + pData[index - 1] + pData[index + 1]) / 4;
				B = (pData[index - nc - 1] + pData[index - nc + 1] + pData[index + nc - 1] + pData[index + nc + 1]) / 4;
				//R = pData[i][j];
				//G = (pData[i][j - 1] + pData[i][j + 1] + pData[i - 1][j] + pData[i + 1][j]) / 4;
				//B = (pData[i - 1][j - 1] + pData[i - 1][j + 1] + pData[i + 1][j - 1] + pData[i + 1][j + 1]) / 4;
			}
			else if (i % 2 == 0 && j % 2 == 0)	//B
			{
				//					R=(pData[i][j-1]+pData[i][j+1])/2;jm
				//					G=pData[i][j];
				//					B=(pData[i-1][j]+pData[i+1][j])/2;
				R = (pData[index - nc - 1] + pData[index - nc + 1] + pData[index + nc - 1] + pData[index + nc + 1]) / 4;
				G = (pData[index - nc] + pData[index + nc] + pData[index - 1] + pData[index + 1]) / 4;
				B = pData[index];
				//R = (pData[i - 1][j - 1] + pData[i - 1][j + 1] + pData[i + 1][j - 1] + pData[i + 1][j + 1]) / 4;
				//G = (pData[i][j - 1] + pData[i][j + 1] + pData[i - 1][j] + pData[i + 1][j]) / 4;
				//B = pData[i][j];
			}
			else if (i % 2 == 0 && j % 2 == 1)	//G
			{
				//					R=pData[i][j];
				//					G=(pData[i][j-1]+pData[i][j+1]+pData[i-1][j]+pData[i+1][j])/4;
				//					B=(pData[i-1][j-1]+pData[i-1][j+1]+pData[i+1][j-1]+pData[i+1][j+1])/4;
				R = (pData[index - 1] + pData[index + 1]) / 2;
				G = pData[index];
				B = (pData[index - nc] + pData[index + nc]) / 2;
				//R = (pData[i][j - 1] + pData[i][j + 1]) / 2;
				//G = pData[i][j];
				//B = (pData[i - 1][j] + pData[i + 1][j]) / 2;
			}
			/**/
			//				uint32_t RGBindex = (i * nc + j) * 3;
			//				CAMBuffer[RGBindex+0]=R;
			//				CAMBuffer[RGBindex+1]=G;
			//				CAMBuffer[RGBindex+2]=B;
			/**/
			cData[index * 3] = B;
			cData[index * 3 + 1] = G;
			cData[index * 3 + 2] = R;
		}
	}
}