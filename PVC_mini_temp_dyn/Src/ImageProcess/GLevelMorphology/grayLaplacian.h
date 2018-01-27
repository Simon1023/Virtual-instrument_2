//GLevelMorphology -- grayLaplacian
#ifndef _GRAYLAPLACIAN_H
#define _GRAYLAPLACIAN_H
#include "../../array_2d.h"

#include "grayDilation.h"
#include "grayErosion.h"

void grayLaplacian(uc1D *ImaSrc, uc1D *ImaDst);

#endif
