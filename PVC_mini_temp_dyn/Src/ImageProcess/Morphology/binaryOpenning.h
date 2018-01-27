//Morphology -- binaryOpenning
#ifndef _BINARYOPENNING_H
#define _BINARYOPENNING_H
#include "../../array_2d.h"

#include "binaryErosion.h"
#include "binaryDilation.h"

void binaryOpenning(uc1D *ImaSrc, uc1D *ImaDst);

#endif
