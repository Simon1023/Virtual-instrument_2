#include "..\ZeroCrossFilter.h"
#include <limits.h>

//void Convolution(uc1D *ImaSrc, uc1D *ImaDst, f1D *h, int WinSize)
//{
//	int data[ImaSrc->nr][ImaSrc->nc];
//	int N= WinSize>>1;
//	int mac;
//	for(int i=N;i<ImaSrc->nr-N;i++)
//	{
//		for(int j=N;j<ImaSrc->nc-N;j++)
//		{
//			mac = 0;
//			for(int ii=-N;ii<=N;ii++)
//			{
//				for(int jj=-N;jj<=N;jj++)
//				{
//					mac = mac + ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)]*h->m[(ii+N)*h->nc+(jj+N)];
//				}
//			}
//			data[i][j]=mac;
//		}
//	}
//	/*------------------過零點檢測------------------*/
//	unsigned char *pout = ImaDst->m + ImaSrc->nc + 1;
//	for(int i=1;i<ImaSrc->nr-1;i++)
//	{
//		for(int j=1;j<ImaSrc->nc-1;j++)
//		{
//			int value[4]={0};
//			value[0]+=data[i][j]+data[i-1][j-1]+data[i][j-1]+data[i-1][j];
//			value[1]+=data[i][j]+data[i-1][j]+data[i-1][j+1]+data[i][j+1];
//			value[2]+=data[i][j]+data[i][j-1]+data[i+1][j-1]+data[i+1][j];
//			value[3]+=data[i][j]+data[i][j+1]+data[i+1][j+1]+data[i+1][j];
//			int min=INT_MAX,max=INT_MIN;
//			for(int z=0;z<4;z++)
//			{
//				if(value[z]<min)
//					min=value[z];
//				if(value[z]>max)
//					max=value[z];
//			}
//			if(min<0 && max>0)
//				*pout=255;
//			else
//				*pout=0;
//			pout++;
//		}
//		pout += 2*N;
//	}
//}
void ZeroCrossFilter(uc1D *ImaSrc, uc1D *ImaDst, int WinSize)
{
	float sigma=1.4,sum=0;
	f1D h=f1D_Initial(WinSize, WinSize);
	int N=(WinSize-1)>>1;
	for(int i=-N;i<=N;i++){
		for(int j=-N;j<=N;j++){
			h.m[(i+N)*h.nc+(j+N)]=482.75*(-1/((3.14)*(sigma*sigma*sigma*sigma)))*(1-((i)*(i)+(j)*(j))/(2*sigma*sigma))*(exp(-((i)*(i)+(j)*(j))/(2*sigma*sigma)));
			sum += h.m[(i+N)*h.nc+(j+N)];
		}
	}
	/*---------正規化---------*/
	float *hout = h.m;
	for(int i=0;i<WinSize;i++){
		for(int j=0;j<WinSize;j++){
//			*hout/=sum;
//			hout++;
			h.m[i*WinSize+j]/=sum;
		}
	}
	
	//------
	float data[3][320]={0};
	
	
//	int N= WinSize>>1;
	float mac;
	for(int i=N;i<ImaSrc->nr-N;i++)
	{
		for(int j=N;j<ImaSrc->nc-N;j++)
		{
			mac = 0.0;
			for(int ii=-N;ii<=N;ii++)
			{
				for(int jj=-N;jj<=N;jj++)//
				{
					mac = mac + ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)]*h.m[(ii+N)*h.nc+(jj+N)];
					//mac = mac + ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)];
				}
			}
			//data[i][j]=mac;
		}
	}
	/*------------------過零點檢測------------------*/
	unsigned char *pout = ImaDst->m;
	pout=ImaDst->m + ImaSrc->nc + 1;
	for(int i=1;i<ImaSrc->nr-1;i++)
	{
		for(int j=1;j<ImaSrc->nc-1;j++)
		{
			float value[4]={0.0};
			//value[0]+=data[i][j]+data[i-1][j-1]+data[i][j-1]+data[i-1][j];
			//value[1]+=data[i][j]+data[i-1][j]+data[i-1][j+1]+data[i][j+1];
			//value[2]+=data[i][j]+data[i][j-1]+data[i+1][j-1]+data[i+1][j];
			//value[3]+=data[i][j]+data[i][j+1]+data[i+1][j+1]+data[i+1][j];
			int min=INT_MAX,max=INT_MIN;
			for(int z=0;z<4;z++)
			{
				if(value[z]<min)
					min=value[z];
				if(value[z]>max)
					max=value[z];
			}
			if(min<0 && max>0)
				*pout=255;
			else
				*pout=0;
			pout++;
		}
		pout += 2*N;
	}
	
	//------
	
//	Convolution(ImaSrc, ImaDst, &h, WinSize);
}
