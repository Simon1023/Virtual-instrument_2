#include "..\grayClosing.h"


void grayClosing(uc1D *ImaSrc, uc1D *ImaDst)
{
	uc1D Ima2 = uc1D_Initial(ImaSrc->nr, ImaSrc->nc);

	grayDilation(ImaSrc, &Ima2);
	grayErosion(&Ima2, ImaDst);
	uc1D_Free(Ima2);
}
