#include "..\diagonal.h"

void diagonal(uc1D *ImaSrc, uc1D *ImaDst)
{
	unsigned char *pin=ImaSrc->m;
	unsigned char *pout=ImaDst->m;
	for(int i=0;i<ImaSrc->nr;i++)
	{
		for(int j=0;j<ImaSrc->nc;j++)
		{
			if(ImaSrc->nr*j==ImaSrc->nc*i || ImaSrc->nr*(ImaSrc->nc-1-j)==ImaSrc->nc*i)
			{
				*pout=255;
			}
			else
			{
				*pout=*pin;
			}
			pin++;
			pout++;
		}
	}
		
}
