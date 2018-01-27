/* Thresholding by using the mean */

#include "../cvThreMEAN.h"

int cvThreMEAN (uc1D *im)
{
	long N, i, j;
	float mean = 0;

/* Find the mean */
	N = (long)im->nc*(long)im->nr;
	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	  mean += (float)(im->m[i*im->nc+j]);
	mean /= (float)N;

	//fprintf (stderr, "Threshold is %d\n", (int)mean);

/* Threshold */
	return (int)mean;
}
