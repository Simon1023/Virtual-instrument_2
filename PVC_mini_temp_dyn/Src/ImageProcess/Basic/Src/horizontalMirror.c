#include "..\horizontalMirror.h"

void horizontalMirror(uc1D *ImaSrc, uc1D *ImaDst)
{
	unsigned char *pout = ImaDst->m;
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			*pout = ImaSrc->m[i*ImaSrc->nc+ImaSrc->nc-j-1];
			pout++;
		}
}
