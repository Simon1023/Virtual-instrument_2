/* Pun method for using entropy */

#include "../cvThrePUN.h"

static void histogram_PUN (uc1D *im, float *hist);
static float flog_PUN (float x);
static float entropy_PUN (float *h, int a);
static float maxfromt (float *h, int i);
static float maxtot (float *h, int i);

int cvThrePUN (uc1D* im)
{
	int i, t;
	float *Ht, HT, *Pt, x, *F, y, z;
	float *hist, to, from;

/* Histogram */
	Ht = (float *)malloc (sizeof(float)*256);
	Pt = (float *)malloc (sizeof(float)*256);        
	F  = (float *)malloc (sizeof(float)*256);        
	hist = (float *)malloc (sizeof(float)*256);        
	histogram_PUN (im, hist);

/* Compute the factors */
	HT = Ht[0] = entropy_PUN (hist, 0);
	Pt[0] = hist[0];
	for (i=1; i<256; i++)
	{
	  Pt[i] = Pt[i-1] + hist[i];
	  x = entropy_PUN(hist, i);
	  Ht[i] = Ht[i-1] + x;
	  HT += x;
	}

/* Calculate the function to be maximized at all levels */
	t = 0;
	for (i=0; i<256; i++)
	{
	  to = (maxtot(hist,i));
	  from = maxfromt(hist, i);
	  if (to > 0.0 && from > 0.0)
	  {
	    x = (Ht[i]/HT)* flog_PUN(Pt[i])/flog_PUN(to);
	    y = 1.0 - (Ht[i]/HT);
	    z = flog_PUN(1 - Pt[i])/flog_PUN(from);
	  }
	  else x = y = z = 0.0;
	  F[i] = x + y*z;
	  if (i>0 && F[i] > F[t]) t = i;
	}
	//fprintf (stderr, "Threshold is %d\n", t);
	free(Ht); free(Pt); free(F);  free(hist); 

/* Threshold */
	return t;
}

void histogram_PUN (uc1D *im, float *hist)
{
	int i, j;
	long N;

	for (i=0; i<256; i++) hist[i] = 0.0;
	N = (long)im->nc * (long)im->nr;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    hist[(im->m[i*im->nc+j])] += 1.0;

	for (i=0; i<256; i++)
	  hist[i] /= (float)N;
}

float entropy_PUN (float *h, int a)
{
	if (h[a] > 0.0)
	  return -(h[a] * (float)log((double)h[a]));
	return 0.0;
}

float flog_PUN (float x)
{
	if (x <= 0.0) return 0.0;
	return (float)log((double)x);
}

float maxtot (float *h, int i)
{
	float x;
	int j;

	x = h[0];
	for (j=1; j<=i; j++)
	  if (x < h[j]) x = h[j];
	return x;
}

float maxfromt (float *h, int i)
{
	int j;
	float x;

	x = h[i+1];
	for (j=i+2; j<=255; j++)
	  if (x < h[j]) x = h[j];
	return x;
}
