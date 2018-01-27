#include "../FltLowPass.h"

void smLowPass(uc1D *ima1, uc1D *ima2, int WinSize)
{
	int i,j,ii,jj;
  int N=WinSize/2;
  int sum, weight;
	
	for(i=0;i<ima1->nr;i++)for(j=0;j<ima1->nc;j++)
	{
		sum = 0;
		weight = 0;
		for(ii=-N;ii<=N;ii++)
		{
			if(!(WinSize%2) && ii==N) break;	//winsize is Even then out of range
			if(	i+ii<0 || i+ii>=ima1->nr) continue;	//out of range
			for(jj=-N;jj<=N;jj++)
			{
				if(!(WinSize%2) && jj==N) break;	//winsize is Even then out of range
				if(	j+jj<0 || j+jj>=ima1->nc) continue;	//out of range
				sum = sum + ima1->m[(i+ii)*ima1->nc+(j+jj)];
				weight++;
			}
		}
		ima2->m[i*ima1->nc+j]=sum/weight;
	}
}
