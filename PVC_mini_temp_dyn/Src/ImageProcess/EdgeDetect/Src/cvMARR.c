/* Marr/Hildreth edge detection */

#include "../cvMARR.h"

static void convolution_marr (uc1D *im, f1D *mask, int nr, int nc, f1D *res,int NR, int NC);
static float gauss_marr(float x, float sigma);
static float LoG (float x, float sigma);
static void marr (float s, uc1D *im);
static void zero_cross (f1D *lapim, uc1D *im);
static float norm_marr (float x, float y);
static float distance (float a, float b, float c, float d);

//s=2.0
void cvMarr (uc1D* im_ori, uc1D* outImg, float s)
{
	int i,j;
	uc1D im1, im2;

	uc1D_Copy (&im1, im_ori);
	uc1D_Copy (&im2, im_ori);
	
/* Apply the filter */
	marr (s-0.8, &im1);
	marr (s+0.8, &im2);
    
	for (i=0; i<im1.nr; i++)
	  for (j=0; j<im1.nc; j++)
		if (im1.m[i*im1.nc+j] > 0 && im2.m[i*im1.nc+j] > 0)
			outImg->m[i*im1.nc+j] = 0;
		else outImg->m[i*im1.nc+j] = 255;

}

float norm_marr (float x, float y)
{
	return (float) sqrt ( (double)(x*x + y*y) );
}

float distance (float a, float b, float c, float d)
{
	return norm_marr ( (a-c), (b-d) );
}

void marr (float s, uc1D *im)
{
	int width;
	f1D smx, lgau;
	int i,j,n;

/* Create a Gaussian and a derivative of Gaussian filter mask  */
	width = 3.35*s + 0.33;
	n = width+width + 1;
	//Smoothing with a Gaussian of size
	lgau = f1D_Initial(n, n);
	for (i=0; i<n; i++)
	  for (j=0; j<n; j++)
			lgau.m[i*n+j] = LoG (distance ((float)i, (float)j,(float)width, (float)width), s);

/* Convolution of source image with a Gaussian in X and Y directions  */
	smx = f1D_Initial(im->nr, im->nc);
	//Convolution with LoG
	convolution_marr (im, &lgau, n, n, &smx, im->nr, im->nc);

/* Locate the zero crossings */
	zero_cross (&smx, im);

/* Clear the boundary */
	for (i=0; i<im->nr; i++)
	{
	  for (j=0; j<=width; j++) im->m[i*im->nc+j] = 0;
	  for (j=im->nc-width-1; j<im->nc; j++)
		im->m[i*im->nc+j] = 0;
	}
	for (j=0; j<im->nc; j++)
	{
	  for (i=0; i<= width; i++) im->m[i*im->nc+j] = 0;
	  for (i=im->nr-width-1; i<im->nr; i++)
		im->m[i*im->nc+j] = 0;
	}

}

/*	Gaussian	*/
float gauss_marr(float x, float sigma)
{
    return (float)exp((double) ((-x*x)/(2*sigma*sigma)));
}

float LoG (float x, float sigma)
{
	float x1;

	x1 = gauss_marr (x, sigma);
	return (x*x-2*sigma*sigma)/(sigma*sigma*sigma*sigma) * x1;
}

void convolution_marr (uc1D *im, f1D *mask, int nr, int nc, f1D *res,int NR, int NC)
{
	int i,j,ii,jj, n, m, k, kk;
	float x;

	k = nr/2; kk = nc/2;
	for (i=0; i<NR; i++)
	  for (j=0; j<NC; j++)
	  {
		x = 0.0;
		for (ii=0; ii<nr; ii++)
		{
		  n = i - k + ii;
		  if (n<0 || n>=NR) continue;
		  for (jj=0; jj<nc; jj++)
		  {
			m = j - kk + jj;
			if (m<0 || m>=NC) continue;
			x += mask->m[ii*nc+jj] * (float)(im->m[n*NC+m]);
		  }
		}
		res->m[i*NC+j] = x;
	  }
}

void zero_cross (f1D *lapim, uc1D *im)
{
	int i,j;
	
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	  {
	    im->m[i*im->nc+j] = 0;
			if(lapim->m[(i-1)*im->nc+j]*lapim->m[(i+1)*im->nc+j]<0) {im->m[i*im->nc+j]=255; continue;}
	    if(lapim->m[i*im->nc+j-1]*lapim->m[i*im->nc+j+1]<0) {im->m[i*im->nc+j]=255; continue;}
	    if(lapim->m[(i+1)*im->nc+(j-1)]*lapim->m[(i-1)*im->nc+(j+1)]<0) {im->m[i*im->nc+j]=255; continue;}
			if(lapim->m[(i-1)*im->nc+(j-1)]*lapim->m[(i+1)*im->nc+(j+1)]<0) {im->m[i*im->nc+j]=255; continue;}
	  }
}
