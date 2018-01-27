//Morphology -- binaryClosing
#ifndef _BINARYCLOSING_H
#define _BINARYCLOSING_H
#include "../../array_2d.h"

#include "binaryDilation.h"
#include "binaryErosion.h"

void binaryClosing(uc1D *ImaSrc, uc1D *ImaDst);

#endif
