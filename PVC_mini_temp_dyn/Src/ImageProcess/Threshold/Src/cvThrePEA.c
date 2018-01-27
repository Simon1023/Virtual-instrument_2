/* Thresholding by using the two histogram peaks */

#include "../cvThrePEA.h"

int cvThrePEA (uc1D *im)
{
	int i,j,k, *hist, t= -1;

	hist = (int*)malloc(256*sizeof(int));
	for (i=0; i<256; i++) hist[i] = 0;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    hist[(im->m[i*im->nc+j])] += 1;

/* Find the first peak (first max) */
	j = 0;
	for (i=1; i<256; i++)
	  if (hist[i] > hist[j]) j = i;

/* Find the second peak */
	k = 0;
	for (i=0; i<256; i++)
	  if (i>0 && hist[i-1]<=hist[i] && i<255 && hist[i+1]<=hist[i])
	    if ((k-j)*(k-j)*hist[k] < (i-j)*(i-j)*hist[i]) k = i;

	t = j;
	if (j<k)
	{
	  for (i=j; i<k; i++)
	    if (hist[i] < hist[t]) t = i;
	} else {
	  for (i=k; i<j; i++)
	    if (hist[i] < hist[t]) t = i;
	}

/* Threshold */
	return t;
}

