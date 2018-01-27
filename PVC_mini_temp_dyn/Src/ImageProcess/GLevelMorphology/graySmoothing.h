//GLevelMorphology -- graySmoothing
#ifndef _GRAYSMOOTHING_H
#define _GRAYSMOOTHING_H
#include "../../array_2d.h"

#include "grayDilation.h"
#include "grayErosion.h"

void graySmoothing(uc1D *ImaSrc, uc1D *ImaDst);

#endif
