#include "../convolution.h"

static void creatConMask(i1D *mask, int WinSize)
{
	int i, j, half;
	(*mask)=i1D_Initial(WinSize,WinSize);
	half = WinSize / 2;
	for(i=0;i<WinSize;i++)for(j=0;j<WinSize;j++)
	{
		if(i == half && j == half)	//center;
			mask->m[i*WinSize+j] = (int)(( 10+(10*half) )*half/2);
		else
			mask->m[i*WinSize+j] = -1;
	}
}

void convolution(uc1D* ima1, uc1D* ima2, int WinSize)
{
	int i,j,ii,jj;
  int N=WinSize/2;
  int mac;
	i1D mask;
	creatConMask(&mask, WinSize);
  for(i=0;i<ima1->nr;i++)
		for(j=0;j<ima1->nc;j++)
		{
			mac = 0;
			for(ii=-N;ii<=N;ii++)
			{
				if(	i+ii<0 || i+ii>=ima1->nr) continue;	//out of range
				for(jj=-N;jj<=N;jj++)
				{
					if(	j+jj<0 || j+jj>=ima1->nc) continue;	//out of range
						mac = mac + ima1->m[(i+ii)*ima1->nc+(j+jj)]*mask.m[(ii+N)*WinSize+(jj+N)];
				}
			}
			ima2->m[i*ima1->nc+j]=mac;
		}
}
