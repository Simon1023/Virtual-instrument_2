#include "..\grayDilation.h"

void grayDilation(uc1D *ImaSrc, uc1D *ImaDst)
{
	const unsigned char sb[3][3]={{0,1,0},{1,1,1},{0,1,0}};
	unsigned char *pout = ImaDst->m;

	int max;
	for(int i=0;i<ImaSrc->nr;i++)
	{
		for(int j=0;j<ImaSrc->nc;j++)
		{
			max=0;
			if(i == 0 || i == ImaSrc->nr-1 || j == 0 || j == ImaSrc->nc-1)
			{
				*pout = ImaSrc->m[i*ImaSrc->nc+j];
				pout++;
			}
			else
			{
				for(int k=-1;k<2;k++)
					for(int l=-1;l<2;l++)
					{
						if(sb[k+1][l+1] == 1)
						{
							if(ImaSrc->m[(i+k)*ImaSrc->nc+(j+l)]>max)
								max=ImaSrc->m[(i+k)*ImaSrc->nc+(j+l)];
						}
					}
				
				*pout=max;
				pout++;
			}
		}
	}
}
