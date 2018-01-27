#include "..\grayOpenning.h"

void grayOpenning(uc1D *ImaSrc, uc1D *ImaDst)
{
	uc1D Ima2 = uc1D_Initial(ImaSrc->nr, ImaSrc->nc);

	grayErosion(ImaSrc, &Ima2);
	grayDilation(&Ima2, ImaDst);
	uc1D_Free(Ima2);
}
