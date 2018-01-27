//GLevelMorphology -- morphologicalContrastEnhance
#ifndef _MORPHOLOGICALCONTRASTENHANCE_H
#define _MORPHOLOGICALCONTRASTENHANCE_H
#include "../../array_2d.h"

#include "grayDilation.h"
#include "grayErosion.h"

void morphologicalContrastEnhance(uc1D *ImaSrc, uc1D *ImaDst, int scale);

#endif
