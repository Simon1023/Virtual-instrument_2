/* Minimum error thresholding */

#include "../cvThreME.h"

static void histogram_ME (uc1D* im, long *hist);
static float s2 (int t);
static float s1 (int t);
static float u2 (int t);
static float u1 (int t);
static float P2 (int t);
static float P1 (int t);
static float J(int t);
static float flog_ME (float x);

static long h[256];

int cvThreME (uc1D *im)
{
	int i,tbest;
	float *F;
	
	F = (float*)malloc(256*sizeof(float));
/* Histogram */
	histogram_ME (im, h);

/* Compute the factors */
	for (i=1; i<256; i++)
	{
	  F[i] = J(i);
	  if (F[i] < F[tbest]) tbest = i;
	}

	//fprintf (stderr, "Threshold is %d\n", tbest);

/* Threshold */
	return tbest;
}

float J(int t)
{
	float a, b, c, d, x1;

	a = P1(t); b = s1(t);
	c = P2(t); d = s2(t);

	x1 = 1.0 + 2.0*(a*flog_ME(b) + c*flog_ME(d)) -
		2.0*(a*flog_ME(a) + c*flog_ME(c));
	return x1;
}

void histogram_ME (uc1D *im, long *hist)
{
	int i, j;

	for (i=0; i<256; i++) hist[i] = 0;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    hist[im->m[i*im->nc+j]] += 1;
}

float flog_ME (float x)
{
	if (x > 0.0) return (float)log((double)x);
	 else return 0.0;
}

float P1 (int t)
{
	long i, sum = 0;

	for (i=0; i<=t; i++)
	  sum += h[i];
	return (float)sum;
}

float P2 (int t)
{
	long i, sum = 0;

	for (i=t+1; i<=255; i++)
	  sum += h[i];
	return (float)sum;
}

float u1 (int t)
{
	long i;
	float sum=0.0, p;

	p = P1 (t);
	if (p <= 0.0) return 0.0;

	for (i=0; i<=t; i++)
	  sum += h[i]*i;
	return sum/p;
}

float u2 (int t)
{
	long i;
	float sum=0.0, p;
 
	p = P2 (t);
	if (p <= 0.0) return 0.0;
 
	for (i=t+1; i<=255; i++)
	  sum += h[i]*i;
	return sum/p;
}

float s1 (int t)
{
	int i;
	float sum=0.0, p, u, x;

	p = P1(t);
	if (p<=0.0) return 0.0;
	u = u1(t);
	for (i=0; i<=t; i++)
	{
	  x = (i-u)*(i-u);
	  sum += x*h[i];
	}
	return sum/p;
}

float s2 (int t)
{
	int i;
	float sum=0.0, p, u, x;
 
	p = P2(t);
	if (p<=0.0) return 0.0;
	u = u2(t);
	for (i=t+1; i<=255; i++)
	{
	  x = (i-u)*(i-u);
	  sum += x*h[i];
	}
	return sum/p;
}
