#ifndef _AMF_H
#define _AMF_H

#include "../../array_2d.h"

#define AMF_RESULT_ERROR    0
#define AMF_RESULT_INIT     1
#define AMF_RESULT_OK       2

int doAmf(uc1D *pSrc);
void resetAmf();

#endif