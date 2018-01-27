#include "..\grayLaplacian.h"
#include <string.h>

unsigned char Ima3_grayLaplacian[240*320]={0};
unsigned char Ima4_grayLaplacian[240*320]={0};

void grayLaplacian(uc1D *ImaSrc, uc1D *ImaDst)
{
	memset(Ima3_grayLaplacian,0,240*320);
	memset(Ima4_grayLaplacian,0,240*320);
 	uc1D Ima3, Ima4;
//	Ima3 = uc2D_Initial(ImaSrc->nr, ImaSrc->nc);
//	Ima4 = uc2D_Initial(ImaSrc->nr, ImaSrc->nc);
	Ima3.nc=Ima4.nc=ImaSrc->nc;
	Ima3.nr=Ima4.nr=ImaSrc->nr;
	Ima3.m=Ima3_grayLaplacian;
	Ima4.m=Ima4_grayLaplacian;
	grayDilation(ImaSrc, &Ima3);
	grayErosion(ImaSrc, &Ima4);

	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			ImaDst->m[i*ImaSrc->nc+j] = (Ima3.m[i*ImaSrc->nc+j]+Ima4.m[i*ImaSrc->nc+j]-(ImaSrc->m[i*ImaSrc->nc+j]<<1))>>1;
		}
		
//	uc2D_Free(&Ima3);
//	uc2D_Free(&Ima4);
}
