#include "..\LoGFilter.h"

void LoG_Convolution(uc1D *ImaSrc, uc1D *ImaDst, f1D *h, int WinSize)
{
	int i,j,ii,jj;
	int N= WinSize/2;
	int mac;
	unsigned char *pout = ImaDst->m + ImaSrc->nc + N;
	for(i=N;i<ImaSrc->nr-N;i++)
	{
		for(j=N;j<ImaSrc->nc-N;j++)
		{
			mac = 0;
			for(ii=-N;ii<=N;ii++)for(jj=-N;jj<=N;jj++)
			{
				mac = mac + ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)]*h->m[(ii+N)*h->nc+(jj+N)];
			}
			if(mac>255) mac=255;
			else if(mac<0) mac=0;
			*pout=mac;
			pout++;
		}
		pout += 2*N;
	}
}

void LoGFilter(uc1D *ImaSrc, uc1D *ImaDst, int WinSize)
{
	float sigma=1.4,sum=0;
	f1D h = f1D_Initial(WinSize, WinSize);;
	int N=(WinSize-1)>>1;
	for(int i=-N;i<=N;i++){
		for(int j=-N;j<=N;j++){
			h.m[(i+N)*h.nc+(j+N)]=482.75*(-1/((3.14)*(sigma*sigma*sigma*sigma)))*(1-((i)*(i)+(j)*(j))/(2*sigma*sigma))*(exp(-((i)*(i)+(j)*(j))/(2*sigma*sigma)));
			sum += h.m[(i+N)*h.nc+(j+N)];
		}
	}
	/*---------¥¿³W¤Æ---------*/
	float *hout = h.m;
	for(int i=0;i<WinSize;i++){
		for(int j=0;j<WinSize;j++){
			*hout/=sum;
			hout++;
		}
	}
	LoG_Convolution(ImaSrc, ImaDst, &h, WinSize);
}
