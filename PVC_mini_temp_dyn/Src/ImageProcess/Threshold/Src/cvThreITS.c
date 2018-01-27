/* Thresholding by using the iterative selection */

#define MAX
#include "../cvThreITS.h"

int cvThreITS (uc1D *im_ori)
{
	long i, j, told, tt, a, b, c, d;
	long N, *hist;

	hist = (long *) malloc(sizeof(long)*257);
	for (i=0; i<256; i++) hist[i] = 0;

/* Compute the mean and the histogram */
	N = (long)im_ori->nc * (long)im_ori->nr;
	tt = 0;
	for (i=0; i<im_ori->nr; i++)
	  for (j=0; j<im_ori->nc; j++)
	  {
	    hist[im_ori->m[i*im_ori->nc+j]] += 1;
	    tt = tt + (im_ori->m[i*im_ori->nc+j]);
	  }
	tt = (tt/(float)N);

	do
	{
	  told = tt;
	  a = 0; b = 0;
	  for (i=0; i<=told; i++)
	  {
	    a += i*hist[i];
	    b += hist[i];
	  }
	  b += b;

	  c = 0; d = 0;
	  for (i=told+1; i<256; i++)
	  {
	    c += i*hist[i];
	    d += hist[i];
	  }
	  d += d;

	  if (b==0) b = 1;
	  if (d==0) d = 1;
	  tt = a/b + c/d;
	} while (tt != told);
	//printf ("Fast threshold is %d\n", tt);
	free (hist);

/* Threshold */
	return tt;

}
