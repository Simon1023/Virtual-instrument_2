//GLevelMorphology -- grayClosing
#ifndef _GRAYCLOSING_H
#define _GRAYCLOSING_H
#include "../../array_2d.h"

#include "grayDilation.h"
#include "grayErosion.h"

void grayClosing(uc1D *ImaSrc, uc1D *ImaDst);

#endif
