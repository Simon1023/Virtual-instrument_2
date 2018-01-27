/* Thresholding by using the black percentage   */

#include "../cvThrePCT.h"

//black percent : 0~100
int cvThrePCT (uc1D *im, float pct)
{
	int i, j, *hist, count= 0, t= -1;
	long N, M;

	hist = (int*)malloc(256*sizeof(int));
	for (i=0; i<256; i++) hist[i] = 0;

/* Find the histogram */
	N = (long)im->nc*(long)im->nr;
	M = (long)((pct/100.0)*N);

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    hist[im->m[i*im->nc+j]] += 1;

/* Find the correct bin */
	for (i=0; i<256; i++)
	{
	  count += hist[i];
	  if (count >= M) 
	  {
	    t = i;
	    break;
	  }
	}

	if (t < 0)
	{
	  //fprintf (stderr, "THRESHOLD (pct): No threshold.\n");
	  return -1;
	}
	//fprintf (stderr, "Threshold is %d\n", t);

/* Threshold */
	return t;
}
