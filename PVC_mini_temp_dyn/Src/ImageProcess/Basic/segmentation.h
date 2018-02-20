//Basic -- character segmentation
#ifndef _SEGMENTATION_H
#define _SEGMENTATION_H

#include "../../array_2d.h"

#define V_PROJECT 1  //vertical
#define H_PROJECT 2  //horizational

#define NO_ASSIGN	-1

typedef struct charInfo{
	unsigned char x;
	unsigned char y;
	unsigned char nr;
	unsigned char nc;
}CHAR_INFO;


int segment(uc1D *imageSrc);
unsigned char segmentGetCount();
CHAR_INFO * segmentGetInfo();

#endif
