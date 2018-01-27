#include "..\morphologicalThresholding.h"
#include <string.h>
//#include "..\grayDilation.h"
//#include "..\grayErosion.h"

unsigned char Ima3_mt[240*320]={0};
unsigned char Ima4_mt[240*320]={0};

void morphologicalThresholding(uc1D *ImaSrc, uc1D *ImaDst)
{
	memset(Ima3_mt,0,240*320);
	memset(Ima4_mt,0,240*320);
	uc1D Ima3, Ima4;
	Ima3.nr=Ima4.nr=ImaSrc->nr;
	Ima4.nc=Ima4.nc=ImaSrc->nc;
	Ima3.m=Ima3_mt;
	Ima4.m=Ima4_mt;
//	Ima3 = uc1D_Initial(ImaSrc->nr, ImaSrc->nc);
//	Ima4 = uc1D_Initial(ImaSrc->nr, ImaSrc->nc);

	grayDilation(ImaSrc, &Ima3);
	grayErosion(ImaSrc, &Ima4);
	
	//unsigned char *pin = ImaSrc->m;
	//unsigned char *pout = ImaDst->m;
	//unsigned char *pout3 = Ima3.m;
	//unsigned char *pout4 = Ima4.m;

	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			//if((*pin) > ((*pout3)+(*pout4))/2)
			//	*pout=0;
			//else
			//	*pout=255;
			
			//pin++;
			//pout++;
			//pout3++;
			//pout4++;
			if(ImaSrc->m[i*ImaSrc->nc+j] > (Ima3.m[i*ImaSrc->nc+j] + Ima4.m[i*ImaSrc->nc+j])>>1)
				ImaDst->m[i*ImaSrc->nc+j] = 0;
			else
				ImaDst->m[i*ImaSrc->nc+j] = 255;
			
		}
		
//	uc1D_Free(Ima3);
//	uc1D_Free(Ima4);
}
