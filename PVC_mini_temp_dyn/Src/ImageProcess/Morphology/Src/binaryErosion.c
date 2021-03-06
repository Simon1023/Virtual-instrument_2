#include "..\binaryErosion.h"

void binaryErosion(uc1D *ImaSrc, uc1D *ImaDst)
{
	signed char seX[5]={0,-1,0,1,0}, seY[5]={-1,0,0,0,1};
	unsigned char *pin = ImaSrc->m;
	unsigned char *pout = ImaDst->m;
	
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
            //20180228 Simon: No need to do binaration here
            /*
			if(*pin >= 150)
				*pin = 255;
			else
				*pin = 0;
            */
            
			*pout = *pin;
			pin++;
			pout++;
		}
	
	pin = ImaSrc->m + 1 + ImaSrc->nc;
	pout = ImaDst->m + 1 + ImaDst->nc;

	for(int i=1;i<ImaSrc->nr-1;i++)
	{
		for(int j=1;j<ImaSrc->nc-1;j++)
		{
			if((*pin)==0)
			{
				for(int k=0;k<5;k++)
					if(ImaSrc->m[(i+seY[k])*ImaSrc->nc+(j+seX[k])]==255)
					{
						*pout=255;
						break;
					}
			}
			pin++;
			pout++;
		}
		pin += 2;
		pout += 2;
	}
}
