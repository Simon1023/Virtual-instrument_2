#include "..\grayGradient.h"
#include <string.h>

unsigned char Ima3_grayGradient[240*320]={0};
unsigned char Ima4_grayGradient[240*320]={0};

void grayGradient(uc1D *ImaSrc, uc1D *ImaDst)
{
	memset(Ima3_grayGradient,0,240*320);
	memset(Ima4_grayGradient,0,240*320);
	uc1D Ima3, Ima4;
	Ima3.nr=Ima4.nr=ImaSrc->nr;
	Ima4.nc=Ima4.nc=ImaSrc->nc;
	Ima3.m=Ima3_grayGradient;
	Ima4.m=Ima4_grayGradient;
//	Ima3 = uc1D_Initial(ImaSrc->nr, ImaSrc->nc);
//	Ima4 = uc1D_Initial(ImaSrc->nr, ImaSrc->nc);

	
	
	grayDilation(ImaSrc, &Ima3);
	grayErosion(ImaSrc, &Ima4);
	
	
	

	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			ImaDst->m[i*ImaSrc->nc+j] = (Ima3.m[i*ImaSrc->nc+j] - Ima4.m[i*ImaSrc->nc+j])/2;
		}
		
//	uc1D_Free(Ima3);
//	uc1D_Free(Ima4);
}
