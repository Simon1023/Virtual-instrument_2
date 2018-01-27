/* Digital Desk - adaptive thresholding */

#include "../cvThreRDD.h"

static float pct = 15.0;       /* Make smaller to darken the image */
static float Navg = 8.0;       /* Fraction of a row in the average (ie 1/8) */

void cvThreRDD (uc1D *im, uc1D *imOut)
{
	int NC, row, col, inc;
	float mean, s, sum;
	unsigned char *p;
	long N, i;
	
//	uc1D_Copy(imOut, im);
	for(int i=0;i<im->nr;i++)
		for(int j=0;j<im->nc;j++)
			imOut->m[i*im->nc+j]=im->m[i*im->nc+j];
	N = (long)im->nc * (long)im->nr;
	NC = im->nc;
	s = (int)(float)(NC/Navg);
	sum = 127*s;

	row = col = 0;
	p = &(imOut->m[0*im->nc+0]);
	inc = 1;

	for (i=0; i<N-1; i++)
	{
	  if (col >= NC)
	  {
	    col = NC-1; row++;
	    p = &(imOut->m[row*im->nc+col]);
	    inc = -1;
	  } else if (col < 0)
	  {
	    col = 0;
	    row++;
	    p = &(imOut->m[row*im->nc+col]);
	    inc = 1;
	  }

/* Estimate the mean of the last NC/8 pixels. */
	  sum = sum - sum/s + *p;
	  mean = sum/s;
	  if (*p < mean*(100-pct)/100.0) *p = 0;
		else *p = 255;
	  p += inc;
	  col += inc;
	}
}
