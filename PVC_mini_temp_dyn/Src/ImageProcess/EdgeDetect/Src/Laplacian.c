/* Laplacian edge detector */

#include "../Laplacian.h"
#include <string.h>
//unsigned char edLaplacian_im[240*320]={0};

static void creatLapMask(i1D *mask, int WinSize)
{
	int i, j, half;
	half = WinSize / 2;
	for(i=0;i<WinSize;i++)for(j=0;j<WinSize;j++)
	{
		if(i == half && j == half)	//center;
			mask->m[i*WinSize+j] = WinSize*WinSize - 1;
		else
			mask->m[i*WinSize+j] = -1;
	}
}

void edLaplacian(uc1D *im_ori, uc1D *result, int WinSize)
{
	int i,j,ii,jj,sum, max=0, min=255;
	int N = WinSize / 2;
	i1D mask;
	i1D temp;
	
	i1D z = i1D_Initial(im_ori->nr, im_ori->nc);
	temp = i1D_Initial(im_ori->nr, im_ori->nc);   

	//creatmask
  mask = i1D_Initial(WinSize, WinSize);
  	
	creatLapMask(&mask, WinSize);

/* Now compute the convolution(filter), scaling */
	for(i=0;i<im_ori->nr;i++)for(j=0;j<im_ori->nc;j++)
	{
		sum = 0;
		for(ii=-N;ii<=N;ii++)
		{
			if(	i+ii<0 || i+ii>= im_ori->nr) continue;	//out of range
			for(jj=-N;jj<=N;jj++)
			{
				if(	j+jj<0 || j+jj>= im_ori->nc) continue;	//out of range
				sum = sum + im_ori->m[(i+ii)*im_ori->nc+j+jj]*mask.m[(ii+N)*WinSize+(jj+N)];
			}
		}
		z.m[i*im_ori->nc+j]=sum;
		if(sum > max)
			max = sum;
		if(sum < min)
			min = sum;
	}
	
	for (i=0; i<im_ori->nr; i++)
	  for (j=0; j<im_ori->nc; j++)
	  {
		//resize range:0~255
	    temp.m[i*im_ori->nc+j] = (z.m[im_ori->nc*i+j])*255/(max-min);
	  }
		
	for (i=1; i<im_ori->nr-1; i++)
	  for (j=1; j<im_ori->nc-1; j++)
	  {
		//resize range:0~255
	    result->m[i*im_ori->nc+j] = 0;
		if(temp.m[(i-1)*im_ori->nc+j]*temp.m[(i+1)*im_ori->nc+j]<0){result->m[i*im_ori->nc+j]=255;continue;}
	    if(temp.m[i*im_ori->nc+j-1]*temp.m[i*im_ori->nc+j+1]<0) {result->m[i*im_ori->nc+j]=255; continue;}
	    if(temp.m[(i+1)*im_ori->nc+(j-1)]*temp.m[(i-1)*im_ori->nc+(j+1)]<0) {result->m[i*im_ori->nc+j]=255; continue;}
		if(temp.m[(i-1)*im_ori->nc+(j-1)]*temp.m[(i+1)*im_ori->nc+(j+1)]<0) {result->m[i*im_ori->nc+j]=255; continue;}
	  }	  
}
