//GLevelMorphology -- grayGradient
#ifndef _GRAYGRADIENT_H
#define _GRAYGRADIENT_H
#include "../../array_2d.h"

#include "grayDilation.h"
#include "grayErosion.h"

void grayGradient(uc1D *ImaSrc, uc1D *ImaDst);
		
#endif
