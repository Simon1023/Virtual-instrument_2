#include "..\downsampling.h"

void downsampling(uc1D *ImaSrc, uc1D *ImaDst, int times)
{
	ImaDst->nr = (int)ImaSrc->nr/times;
	ImaDst->nc = (int)ImaSrc->nc/times;
	for(int i=0;i<ImaDst->nr;i++)
	{
		for(int j=0;j<ImaDst->nc;j++)
		{
				ImaDst->m[i*ImaDst->nc+j]=ImaSrc->m[(i*times)*ImaSrc->nc+(j*times)];
		}
	}
}
