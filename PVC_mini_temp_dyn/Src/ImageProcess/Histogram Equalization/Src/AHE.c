#include "..\AHE.h"
#include <math.h>
#include <string.h>


//void mean_stddev(uc1D *ima, float *mean, float *std_dev)
//{
//	unsigned int sum=0;
//	unsigned int N=ima->nr*ima->nc;
//	unsigned char *pin = ima->m;
//	for(int i=0;i<ima->nr;i++)
//	{
//		for(int j=0;j<ima->nc;j++)
//		{
//			sum+=*pin;
//			pin++;
//		}
//	}

//	*mean=(float)sum/(float)N; //Calculating the mean

//	float sumdev=0.0;
//	float d=0.0;
//	pin = ima->m;
//	for(int i=0;i<ima->nr;i++)
//		for(int j=0;j<ima->nc;j++)
//		{
//			d=(*pin)-*mean;
//			sumdev=sumdev+d*d;
//			pin++;
//		}
//	*std_dev=sqrt(sumdev/N); //Calculating the standard deviance
//}
//	unsigned char winsize=21;
//	unsigned char hsize=10;
//	
//	unsigned char window[441]={0};
//	
//	float globalmean=0.0, mean=0.0;
//	float std_dev=0.0;
//	
//	unsigned int t;
//	
//	unsigned int sum=0;
//	unsigned int N=240*320;
//	
//	float sumdev=0.0;
//	float d=0.0;
	
void AHE(uc1D *ImaSrc, uc1D *ImaDst, float k1, float k2)
{
	const int winsize=21;
	unsigned char hsize=winsize>>1;
	
	unsigned char window[winsize*winsize]={0};
	
	float globalmean=0.0, mean=0.0;
	float std_dev=0.0;
	
	unsigned int t;
	
	unsigned int sum=0;
	unsigned int N=winsize*winsize;
	
	float sumdev=0.0;
	float d=0.0;
	
//	unsigned char *pin = ImaSrc->m;
//	unsigned char *pout = ImaDst->m;
	
//	for(int i=0;i<ImaSrc->nr;i++)
//	{
//		for(int j=0;j<ImaSrc->nc;j++)
//		{
////			*pout = *pin;
////			pin++;
////			pout++;
//			ImaDst->m[i*ImaSrc->nc+j]=ImaSrc->m[i*ImaSrc->nc+j];
//		}
//	}
	
	
	
//	uc1D window=uc1D_Initial(winsize,winsize);
	
//	pin = ImaSrc->m;
	for(int i=0;i<ImaSrc->nr;i++)
	{
		for(int j=0;j<ImaSrc->nc;j++)
		{
//			globalmean+=*pin;
//			pin++;
			globalmean+=ImaSrc->m[i*ImaSrc->nc+j];
		}
	}
	globalmean=globalmean/(ImaSrc->nr*ImaSrc->nc);
	
	
			
//	unsigned char *pwin;
//	pin = ImaSrc->m + hsize*ImaSrc->nc + hsize;
//	pout = ImaDst->m + hsize*ImaSrc->nc + hsize;
	for(int i=hsize;i<(ImaSrc->nr-hsize);i++)
	{
		for(int j=hsize;j<(ImaSrc->nc-hsize);j++)
		{
//			pwin = window;
			for(int ii=-hsize;ii<=hsize;ii++)
			{
				for(int jj=-hsize;jj<=hsize;jj++)
				{
//					*pwin=*(pin+(i+ii)*ImaSrc->nc+(j+jj));
//					pwin++;
					window[(ii+hsize)*(hsize*2+1)+(jj+hsize)]=ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)];
				}
			}
			//------------------------------------------------------------------------
//			mean_stddev(&window, &mean, &std_dev);
					
			
//			pwin = window;
				sum=0;
			
			for(int i=0;i<winsize;i++)
			{
				for(int j=0;j<winsize;j++)
				{
//				sum+=*pwin;
//				pwin++;
					sum+=window[i*winsize+j];
				}
			}
			mean=(float)sum/(float)N; //Calculating the mean
		sumdev=0.0f;
//			pwin = window;
			for(int i=0;i<winsize;i++)
				for(int j=0;j<winsize;j++)
				{
//				d=(*pwin)-mean;
//				sumdev=sumdev+d*d;
//				pwin++;
					d=window[i*winsize+j]-mean;
					sumdev=sumdev+d*d;
				}
			std_dev=sqrt(sumdev/N); //Calculating the standard deviance
			//----------------------------------------------------------------------------
			if(std_dev==0.0)
					std_dev=0.0001;
//			t=(k1*(globalmean/std_dev)*((*pin)-mean))+(k2*mean);
			t=(k1*(globalmean/std_dev)*(ImaSrc->m[i*ImaSrc->nc+j]-mean))+(k2*mean);
			if(t>255)
//				*pout=255;
				ImaDst->m[i*ImaSrc->nc+j]=255;
			else
//				*pout=t;
				ImaDst->m[i*ImaSrc->nc+j]=t;

//			pin++;
//			pout++;
		}
	}
	
	
}
