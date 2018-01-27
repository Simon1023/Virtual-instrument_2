#include "../FltGaussian.h"

static void createGauMask(f1D *mask, int WinSize)
{
    // set standard deviation to 1.0
    double sigma = 1.0;
    double r, s = 2.0 * sigma * sigma;
 
    // sum is for normalization
    double sum = 0.0;
	
		int half = WinSize/2;
		int x, y, i, j;
	
		(*mask) = f1D_Initial(WinSize,WinSize);
	
    // generate NxN kernel
    for (x = -half; x <= half; x++)
    {
        for(y = -half; y <= half; y++)
        {
            r = x*x + y*y;
            mask->m[(x + half)*WinSize+(y + half)] = (exp(-r/s))/(3.141592 * s);
            sum += mask->m[(x + half)*WinSize+(y + half)];
        }
    }
 
    // normalize the Kernel
    for(i = 0; i < WinSize; ++i)
        for(j = 0; j < WinSize; ++j)
            mask->m[i*WinSize+j] /= sum; 
}

void smGaussian(uc1D *ima1, uc1D *ima2, int WinSize)
{
    int i,j,ii,jj;
    int N=WinSize/2;
    double sum;
		//double weight;
	f1D mask;
	
	createGauMask(&mask, WinSize);
	for(i=0;i<ima1->nr;i++)for(j=0;j<ima1->nc;j++)
	{
		sum = 0;
        //weight = 0;
		for(ii=-N;ii<=N;ii++)
		{
			if(	i+ii<0 || i+ii>=ima1->nr) continue;	//out of range
			for(jj=-N;jj<=N;jj++)
			{
				if(	j+jj<0 || j+jj>=ima1->nc) continue;	//out of range
				sum = sum + ima1->m[(i+ii)*ima1->nc+(j+jj)]*mask.m[(ii+N)*WinSize+(jj+N)];
				//weight = weight + mask->m[ii+N][jj+N];
			}
		}
		//(*ima2)->m[i][j]=sum/weight;
		ima2->m[i*ima1->nc+j]=(int)sum;
	}
}
