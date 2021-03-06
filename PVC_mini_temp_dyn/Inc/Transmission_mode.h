
#ifndef TRANSMISSION_MODE_H
#define TRANSMISSION_MODE_H

//#define NR 120
//#define NC 160
#define NR 240
#define NC 320
//#define NR 480
//#define NC 640
//#define NR 1944
//#define NC 2592

#define	FORMAT_GRAY	1
#define	FORMAT_RGB	1
#define FORMAT FORMAT_RGB

#define NUM_OF_PIXELS (NC * NR * FORMAT)


typedef struct STRUCT_ROI {
	int x;
	int y;
	int w;
	int h;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char type;
}ROI;

#define ROI_TYPE_NONE   0
#define ROI_TYPE_DIGIT  1
#define ROI_TYPE_WAVE   2
#define ROI_TYPE_HAND   3

#define TRANSFER_ROI_IMAGE   0

#endif
