#ifndef __CVCANNY_H
#define __CVCANNY_H

#include "array_2d.h"

//low=3 high=5 s=4.0
void cvCanny(uc1D *im, uc1D *result, int low, int high, float s);

#endif
