#include "..\MinFilter.h"

void MinFilter(uc1D *ImaSrc, uc1D *ImaDst)
{
	int r,c,y,x,i,j;
	unsigned char Buf;
	unsigned char Lst[9];
	unsigned char *pout = ImaDst->m;
	
	for(r=0;r<(ImaSrc->nr * ImaSrc->nc);r++)
	{
		*pout=0;
		pout++;
	}
	pout = ImaDst->m;
	for(r=0;r<ImaSrc->nr;r++)
	{
		for(c=0;c<ImaSrc->nc;c++)
		{
			i=0;
			if(r == 0 || r == ImaSrc->nr-1 || c == 0 || c == ImaSrc->nc-1)
			{
				*pout = ImaSrc->m[r*ImaSrc->nc+c];
				pout++;
			}
			else
			{
				for(y=-1;y<=1;y++)
					for(x=-1;x<=1;x++)
					{
						Lst[i]=ImaSrc->m[(r+y)*ImaSrc->nc+(c+x)];
						i++;
					}
				for(i=0;i<9;i++)
					for(j=8;i<j;j--)
					{
						if(Lst[j-1]>Lst[j])
						{
							Buf=Lst[j-1];
							Lst[j-1]=Lst[j];
							Lst[j]=Buf;
						}
					}
				*pout = Lst[0];
				pout++;
			}
		}
	}
}
