/* Johannsen method for using entropy */

#include "../cvThreJOH.h"

static float entropy (float h);
static void histogram (uc1D *im_ori, float *hist);
static float flog (float x);

int cvThreJOH (uc1D *im_ori)
{
	int i, t, start, end;
	float Sb, Sw, *Pt, *hist, *F, *Pq;

/* Histogram */
	Pt = (float *)malloc(sizeof(float)*256);
	hist = (float *)malloc(sizeof(float)*256);
	F = (float *)malloc(sizeof(float)*256);
	Pq = (float *)malloc(sizeof(float)*256);

	histogram (im_ori, hist);

/* Compute the factors */
	Pt[0] = hist[0];
	Pq[0] = 1.0 - Pt[0];
	for (i=1; i<256; i++)
	{
	  Pt[i] = Pt[i-1] + hist[i];
	  Pq[i] = 1.0 - Pt[i-1];
	}

	start = 0;
	while (hist[start++] <= 0.0) ;
	end = 255;
	while (hist[end--] <= 0.0) ;

/* Calculate the function to be minimized at all levels */
	t = -1;
	for (i=start; i<=end; i++)
	{
	  if (hist[i] <= 0.0) continue;
	  Sb = flog(Pt[i]) + (1.0/Pt[i])*
		(entropy(hist[i])+entropy(Pt[i-1]));
	  Sw = flog (Pq[i]) + (1.0/Pq[i])*
		(entropy(hist[i]) + entropy(Pq[i+1]));
	  F[i] = Sb+Sw;
	  if (t<0) t = i;
	  else if (F[i] < F[t]) t = i;
	}
	//fprintf (stderr, "Threshold is %d\n", t);
	free(hist); free (Pt); free (F); free (Pq);

/* Threshold */
	return t;
}

void histogram (uc1D *im_ori, float *hist)
{
	int  i, j;
	long N;

	for (i=0; i<256; i++) hist[i] = 0.0;
	N = (long)im_ori->nc * (long)im_ori->nr;

	for (i=0; i<im_ori->nr; i++)
	  for (j=0; j<im_ori->nc; j++)
	    hist[(im_ori->m[i*im_ori->nc+j])] += 1.0;

	for (i=0; i<256; i++)
	  hist[i] /= (float)N;
}

float entropy (float h)
{
	if (h > 0.0)
	  return (-h * (float)log((double)(h)));
	else return 0.0;
}

float flog (float x)
{
	if (x > 0.0) return (float)log((double)x);
	 else return 0.0;
}

