/* Kapur method for using entropy */

#include "../cvThreKAPUR.h"

static float entropy (float *h, int a, float p);
static void histogram_kapur (uc1D *im, float *hist);

int cvThreKAPUR (uc1D *im)
{
	int i, j, t;
	float Hb, Hw, *Pt, *hist, *F;

/* Histogram */
	Pt = (float *)malloc(sizeof(float)*256);
	hist = (float *)malloc(sizeof(float)*256);  
	F = (float *)malloc(sizeof(float)*256);  
	
	histogram_kapur (im, hist);

/* Compute the factors */
	Pt[0] = hist[0];
	for (i=1; i<256; i++)
	  Pt[i] = Pt[i-1] + hist[i];

/* Calculate the function to be maximized at all levels */
	t = 0;
	for (i=0; i<256; i++)
	{
	  Hb = Hw = 0.0;
	  for (j=0; j<256; j++)
	    if (j<=i)
	      Hb += entropy (hist, j, Pt[i]);
	    else 
	      Hw += entropy (hist, j, 1.0-Pt[i]);

	  F[i] = Hb+Hw;
	  if (i>0 && F[i] > F[t]) t = i;
	}
	//fprintf (stderr, "Threshold is %d\n", t);
	free(hist); free(Pt); free(F);

/* Threshold */
	return t;
}

void histogram_kapur (uc1D *im, float *hist)
{
	int i, j;
	long N;

	for (i=0; i<256; i++) hist[i] = 0.0;
	N = (long)im->nc * (long)im->nr;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    hist[im->m[i*im->nc+j]] += 1.0;

	for (i=0; i<256; i++)
	  hist[i] /= (float)N;
}

float entropy (float *h, int a, float p)
{
	if (h[a] > 0.0 && p>0.0)
	  return -(h[a]/p * (float)log((double)(h[a])/p));
	return 0.0;
}

