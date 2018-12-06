#pragma once

#include <windows.h>

#define packed __attribute__((packed))

#define TRUE 1
#define FALSE 0

#define DEVLOP_MODE

#define TRANSFER_ROI_IMAGE   1

//SCSI command
#define SCSI_GET_IMAGE_INFO		    0xD0
#define SCSI_CHECK_IMAGE_READY	    0xD1
#define SCSI_GET_IMAGE			    0xD2
#define SCSI_CLEAR_FLAG			    0xD3
#define SCSI_SEND_ROI_INFO		    0xD4
#define SCSI_GET_ROI_IMAGE		    0xD5
#define SCSI_GET_SEGMENT_COUNT	    0xD6
#define SCSI_GET_SEGMENT_INFO	    0xD7
#define SCSI_SEND_ROI_DIGIT_INFO	0xD8
#define SCSI_SEND_ROI_WAVE_INFO	    0xD9
#define SCSI_SEND_ROI_HAND_INFO		0xDA

//image type
#define BINARY_IMAGE 8
#define GRAY_IMAGE 8
#define COLOR_IMAGE 24

#define ROI_TYPE_NONE   0
#define ROI_TYPE_DIGIT  1
#define ROI_TYPE_WAVE   2
#define ROI_TYPE_HAND   3


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
	static int sendRoiInfo(USHORT x, USHORT y, USHORT w, USHORT h, UCHAR r, UCHAR g, UCHAR b);
	static int getRoiImage(unsigned char* pData, int type, int nc, int nr);
    static int getSegmentCount();
    static void getSegmentInfo(UCHAR *buf);
    static void pnpDataCollect(unsigned char *img, int roiH, int roiW);
	static void bayer2rgb(unsigned char *pData, unsigned char *cData, int nr, int nc);
    static void screenCapture(bool bResult);

private:
	static HANDLE fileHandle;
};

