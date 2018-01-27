#include "..\binaryClosing.h"

void binaryClosing(uc1D *ImaSrc, uc1D *ImaDst)
{
	uc1D Ima2 = uc1D_Initial(ImaSrc->nr, ImaSrc->nc);
	binaryDilation(ImaSrc, &Ima2);
	binaryErosion(&Ima2, ImaDst);
	uc1D_Free(Ima2);
}
