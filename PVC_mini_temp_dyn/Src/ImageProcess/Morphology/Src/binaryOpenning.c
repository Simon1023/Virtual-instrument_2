#include "..\binaryOpenning.h"

void binaryOpenning(uc1D *ImaSrc, uc1D *ImaDst)
{
	
	uc1D Ima2 = uc1D_Initial(ImaSrc->nr, ImaSrc->nc);

	binaryErosion(ImaSrc, &Ima2);
	binaryDilation(&Ima2, ImaDst);
	uc1D_Free(Ima2);
}
