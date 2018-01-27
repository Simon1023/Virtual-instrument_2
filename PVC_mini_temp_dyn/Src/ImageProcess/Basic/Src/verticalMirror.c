#include "..\verticalMirror.h"

void verticalMirror(uc1D *ImaSrc, uc1D *ImaDst)
{
	unsigned char *pout = ImaDst->m;
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			*pout = ImaSrc->m[(ImaSrc->nr-i-1)*ImaSrc->nc+j];
			pout++;
		}
}
