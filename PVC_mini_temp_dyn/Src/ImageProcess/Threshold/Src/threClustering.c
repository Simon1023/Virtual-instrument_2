/* Otsu's method of 'grey level histograms' */

#include "../threClustering.h"
#include <string.h>

static float nu (float *p, int k, float ut, float vt);
static float u (float *p, int k);

int threClustering (uc1D *im_ori)
{
/*      Threshold selection using grey level histograms. SMC-9 No 1 Jan 1979
		N. Otsu                                                 */

	int i,j,k,m, *h, t;
	float y, z, *p;
	float ut, vt;
	long N;

	h = (int *)malloc(260*sizeof(int));
	p = (float *)malloc(260*sizeof(float));
	N = (long)im_ori->nr * (long)im_ori->nc;
//	for (i=0; i<260; i++) {         /* Zero the histograms  */
//		h[i] = 0;
//		p[i] = 0.0;
//	}
	memset(h,0,260);
	memset(p,0,260);

	/* Accumulate a histogram */
	for (i=0; i<im_ori->nr; i++)
	   for (j=0; j<im_ori->nc; j++) {
		k = im_ori->m[i*im_ori->nc+j];
		h[k+1] += 1;
	   }

	for (i=1; i<=256; i++)          /* Normalize into a distribution */
		p[i] = (float)h[i]/(float)N;

	ut = u(p, 256);         /* Global mean */
	vt = 0.0;               /* Global Variance */
	for (i=1; i<=256; i++)
		vt += (i-ut)*(i-ut)*p[i];

	j = -1; k = -1;
	for (i=1; i<=256; i++) {
		if ((j<0) && (p[i] > 0.0)) j = i;       /* First index */
		if (p[i] > 0.0) k = i;                  /* Last index  */
	}
	z = -1.0;
	m = -1;
	for (i=j; i<=k; i++) {
		y = nu (p, i, ut, vt);          /* Compute NU */
		if (y>=z) {                     /* Is it the biggest? */
			z = y;                  /* Yes. Save value and i */
			m = i;
		}
	}

	t = m;
	//printf("Threshold found is %d\n", t);

/* Threshold */
	return t;
}

float w (float *p, int k)
{
	int i;
	float x=0.0;

	for (i=1; i<=k; i++) x += p[i];
	return x;
}

float u (float *p, int k)
{
	int i;
	float x=0.0;

	for (i=1; i<=k; i++) x += (float)i*p[i];
	return x;
}

float nu (float *p, int k, float ut, float vt)
{
	float x, y;

	y = w(p,k);
	x = ut*y - u(p,k);
	x = x*x;
	y = y*(1.0-y);
	if (y>0) x = x/y;
	 else x = 0.0;
	return x/vt;
}

