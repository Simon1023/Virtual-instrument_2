//GLevelMorphology -- morphologicalThresholding
#ifndef _MORPHOLOGICALTHRESHOLDING_H
#define _MORPHOLOGICALTHRESHOLDING_H
#include "../../array_2d.h"

#include "grayDilation.h"
#include "grayErosion.h"

void morphologicalThresholding(uc1D *ImaSrc, uc1D *ImaDst);

#endif
