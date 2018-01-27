#ifndef _CONVOLUTION_H
#define _CONVOLUTION_H
/* convolution, kernel(mask): -1 -1 -1  and so on. */
//					 		  						-1 10 -1
//							 							-1 -1 -1

#include "../../array_2d.h"

void convolution(uc1D* ima1, uc1D* ima2, int WinSize);

#endif
