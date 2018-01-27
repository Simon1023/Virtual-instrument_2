/* threshold Niblack method */
#ifndef __THRENIBLACK_H
#define __THRENIBLACK_H

#include "../../array_2d.h"
//row,col : row and column of window
//deviation : (0~10)/10, default:2/10
void threNiblack(uc1D *image, uc1D *outImg, int row, int col, int deviation);

#endif
