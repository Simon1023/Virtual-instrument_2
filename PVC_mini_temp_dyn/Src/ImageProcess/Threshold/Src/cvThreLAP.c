/* Thresholding by using the two histogram peaks */

#include "../cvThreLAP.h"

static void peaks_LAP (uc1D *im, int *hist, f1D *lap, int lval, int *t);
static void fhist (f1D *data, int *hist, int nr, int nc);
static void hi_pct (int *hist, int NH, long N, float pct, int *val);
static void Laplacian (uc1D *input, f1D *output);
static float pix_lap (uc1D *im, int r, int c);

static float PCT=85.0;
//static int hist[2048];

int cvThreLAP (uc1D *im)
{
	f1D Lap;
	int t, v, *hist;

/* Compute the Laplacian of 'im' */
	//hist = (int*)malloc(2048*sizeof(int));
	Lap = f1D_Initial (im->nr, im->nc);
	Laplacian (im, &Lap); 

/* Find the high 85% of the Laplacian values */
	fhist (&Lap, hist, im->nr, im->nc);	
	hi_pct (hist, 2048, (long)im->nr*(long)im->nc, PCT, &v); 

/* Construct histogram of the grey levels of hi Laplacian pixels */
	peaks_LAP (im, hist, &Lap, v, &t);
	
	free(hist);
/* Threshold */
	return t;
}

/*      Return the level marking the high 85% of pixels */
void hi_pct (int *hist, int NH, long N, float pct, int *val)
{
	int i,j=0, m;

	*val = -1;
	m = (pct/100.0) * N;
	for (i=0; i<NH; i++)
	{
	  j += hist[i];
	  if (j>=m)
	  {
	    *val = i;
	    break;
	  }
	}
//	if (*val < 0) 
//		printf ("BAD HISTOGRAM in 'hi_pct'.\n");
}

/*      Construct a histogram of a float matrix         */
void fhist (f1D *data, int *hist, int nr, int nc)
{
	int i,j,temp;

	for (i=0; i<2048; i++) hist[i] = 0;
	for (i=0; i<nr; i++)
	  for (j=0; j<nc; j++){
			temp = (int)data->m[i*nc+j];
	    hist[(int)(data->m[i*nc+j])] += 1;
		}
}

void Laplacian (uc1D *input, f1D *output)
{
	int i,j;

	for (i=1; i<input->nr-1; i++)
	  for (j=1; j<input->nc-1; j++)
	    output->m[i*input->nc+j] = pix_lap (input, i, j);
}

float pix_lap (uc1D *im, int r, int c)
{
	int k=0, i,j;

/*
	k = (int)im->data[r-1][c]+(int)im->data[r+1][c] +
	    (int)im->data[r][c-1]+(int)im->data[r][c-1];
	k = k-(int)im->data[r][c]*4;
*/
	for (i= -1; i<=1; i++)
		for (j= -1; j<=1; j++)
		
	    if (i!=0 || j!=0)
		k += im->m[(i+r)*im->nc+(j+c)];
	k = k - 8*(int)im->m[r*im->nc+c];
	if (k<=0) return 0.0;
	return (float)k/8.0;
}

void peaks_LAP (uc1D *im, int *hist, f1D *lap, int lval, int *t)
{
	int N, i,j,k;

	for (i=0; i<256; i++) hist[i] = 0;
	*t = -1;

/* Find the histogram */
	N = im->nc*im->nr;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    if (lap->m[i*im->nc+j] >= lval)
	      hist[im->m[i*im->nc+j]] += 1;

/* Find the first peak */
	j = 0;
	for (i=0; i<256; i++)
	  if (hist[i] > hist[j]) j = i;

/* Find the second peak */
	k = 0;
	for (i=0; i<256; i++)
	  if (i>0 && hist[i-1]<=hist[i] && i<255 && hist[i+1]<=hist[i])
	    if ((k-j)*(k-j)*hist[k] < (i-j)*(i-j)*hist[i]) k = i;

	*t = j;
	if (j<k)
	{
	  for (i=j; i<k; i++)
	    if (hist[i] < hist[*t]) *t = i;
	} else {
	  for (i=k; i<j; i++)
	    if (hist[i] < hist[*t]) *t = i;
	}
}

