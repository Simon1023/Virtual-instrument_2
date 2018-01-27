#include "..\morphologicalContrastEnhance.h"
#include <string.h>

unsigned char Ima3_mc[240*320]={0};
unsigned char Ima4_mc[240*320]={0};

void morphologicalContrastEnhance(uc1D *ImaSrc, uc1D *ImaDst, int scale)
{
	memset(Ima3_mc,0,240*320);
	memset(Ima4_mc,0,240*320);
	uc1D Ima3, Ima4;
	Ima3.nr=Ima4.nr=ImaSrc->nr;
	Ima4.nc=Ima4.nc=ImaSrc->nc;
	Ima3.m=Ima3_mc;
	Ima4.m=Ima4_mc;
	
//	Ima3 = uc2D_Initial(ImaSrc->nr, ImaSrc->nc);
//	Ima4 = uc2D_Initial(ImaSrc->nr, ImaSrc->nc);

	grayDilation(ImaSrc, &Ima3);
	grayErosion(ImaSrc, &Ima4);

	for(int i=0;i<ImaSrc->nr;i++)
	{
		for(int j=0;j<ImaSrc->nc;j++)
		{
//			if((*pout3)-(*pout4) == 0)
//				*pout=0;
//			else if((scale*((*pin)-(*pout4)))/((*pout3)-(*pout4))<=0)
//				*pout=0;
//			else if((scale*((*pin)-(*pout4)))/((*pout3)-(*pout4))>=255)
//				*pout=255;
//			else
//				*pout=(scale*((*pin)-(*pout4)))/((*pout3)-(*pout4));
//			
//			pin++;
//			pout++;
//			pout3++;
//			pout4++;
//			
			if(Ima3.m[i*ImaSrc->nc+j]-Ima4.m[i*ImaSrc->nc+j] == 0)
				ImaDst->m[i*ImaSrc->nc+j] = 0;
			else if((scale*(ImaSrc->m[i*ImaSrc->nc+j] - Ima4.m[i*ImaSrc->nc+j])) / (Ima3.m[i*ImaSrc->nc+j] - Ima4.m[i*ImaSrc->nc+j]) <= 0)
				ImaDst->m[i*ImaSrc->nc+j] = 0;
			else if((scale*(ImaSrc->m[i*ImaSrc->nc+j] - Ima4.m[i*ImaSrc->nc+j])) / (Ima3.m[i*ImaSrc->nc+j] - Ima4.m[i*ImaSrc->nc+j]) >= 255)
				ImaDst->m[i*ImaSrc->nc+j] = 255;
			else
				ImaDst->m[i*ImaSrc->nc+j] = (scale*(ImaSrc->m[i*ImaSrc->nc+j] - Ima4.m[i*ImaSrc->nc+j])) / (Ima3.m[i*ImaSrc->nc+j] - Ima4.m[i*ImaSrc->nc+j]);
		}
	}
	
//	uc2D_Free(&Ima3);
//	uc2D_Free(&Ima4);
}
