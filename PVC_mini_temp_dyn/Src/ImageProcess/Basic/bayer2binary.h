//Basic -- Tranform bayer to RGB and thansfer it to binary.
#ifndef _BAYER2BINARY_H
#define _BAYER2BINARY_H

#include "../../array_2d.h"

void bayer2binary(uc1D *pSrc, uc1D *pDst,unsigned char thrR, unsigned char thrG, unsigned char thrB);

#endif
