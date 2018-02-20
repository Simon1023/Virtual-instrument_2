#pragma once

#include <windows.h>

#define packed __attribute__((packed))

#define TRUE 1
#define FALSE 0

//SCSI command
#define SCSI_GET_IMAGE_INFO		0xD0
#define SCSI_CHECK_IMAGE_READY	0xD1
#define SCSI_GET_IMAGE			0xD2
#define SCSI_CLEAR_FLAG			0xD3
#define SCSI_SEND_ROI_INFO		0xD4
#define SCSI_GET_ROI_IMAGE		0xD5
#define SCSI_GET_SEGMENT_COUNT	0xD6
#define SCSI_GET_SEGMENT_INFO	0xD7

//image type
#define BINARY_IMAGE 8
#define GRAY_IMAGE 8
#define COLOR_IMAGE 24

typedef struct segCharInfo {
    unsigned char x;
    unsigned char y;
    unsigned char nr;
    unsigned char nc;
}SEG_CHAR_INFO;

class Utility
{
public:
	Utility();
	~Utility();

	static const int imageWidth = 320;
	static const int imageHeight = 240;
	static const int imageFormat = COLOR_IMAGE;

	static int init(int *nc, int *nr);
	static int connectCamera();
	static void disconnectCamera();
	static int checkImageFlag();
	static int startCapture();
	static int captureImage(unsigned char* pData, int nc, int nr, int timeout, bool bRoi);
	static int sendRoiInfo(USHORT x, USHORT y, USHORT w, USHORT h);
	static int getRoiImage(unsigned char* pData, int type, int nc, int nr);
    static int getSegmentCount();
    static int getSegmentInfo(UCHAR *buf);
	static void bayer2rgb(unsigned char *pData, unsigned char *cData, int nr, int nc);

private:
	static HANDLE fileHandle;
};

