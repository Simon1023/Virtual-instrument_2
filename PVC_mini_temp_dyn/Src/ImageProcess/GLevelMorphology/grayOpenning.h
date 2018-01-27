//GLevelMorphology -- grayOpenning
#ifndef _GRAYOPENNING_H
#define _GRAYOPENNING_H
#include "../../array_2d.h"

#include "grayDilation.h"
#include "grayErosion.h"

void grayOpenning(uc1D *ImaSrc, uc1D *ImaDst);

#endif
