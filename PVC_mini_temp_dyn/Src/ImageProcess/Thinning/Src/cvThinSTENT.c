/* Stentiford's thinning algorithm */

#include "../cvThinSTENT.h"
#include <string.h>


unsigned char cvThinSTENT_im[242*322]={0};
static void thnstent (uc1D *im);
static int nays8_stent (uc1D *im, int r, int c);
static int Yokoi_stent (uc1D *im, int r, int c);

void cvThinSTENT (uc1D *im_ori, uc1D *outimg)
{
	uc1D im;
	int i,j;
	memset(cvThinSTENT_im,0,242*322);
	im.nc=im_ori->nc+2;
	im.nr=im_ori->nr+2;
	im.m=cvThinSTENT_im;
		
//	im = uc1D_Initial (im_ori->nr+2, im_ori->nc+2);
	
	for (i=0; i<im_ori->nr; i++)
	  for (j=0; j<im_ori->nc; j++)
	    im.m[(i+1)*im.nc+j+1] = im_ori->m[i*im_ori->nc+j];
	for (i=0; i<im.nr; i++) 
	{
	  im.m[i*im.nc+0] = 1;
	  im.m[i*im.nc+im.nc-1] = 1;
	}
	for (j=0; j<im.nc; j++)
	{
	  im.m[0*im.nc+j] = 1;
	  im.m[(im.nr-1)*im.nc+j] = 1;
	}

	thnstent (&im);

	for (i=0; i<im_ori->nr; i++)
	   for (j=0; j<im_ori->nc; j++)
	      outimg->m[i*im_ori->nc+j] = im.m[(i+1)*im.nc+j+1];
}

void thnstent (uc1D *im)
{
	int i,j, again=1;

/* BLACK = 0, WHITE = 1. */
	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    if (im->m[i*im->nc+j] > 0)
				im->m[i*im->nc+j] = 1;

/* Mark and delete */
	while (again)
	{
	  again = 0;

/* Matching template M1 - scan top-bottom, left - right */
	  for (i=1; i<im->nr-1; i++)
	    for (j=1; j<im->nc-1; j++)
	      if (im->m[i*im->nc+j] == 0)
					if (im->m[(i-1)*im->nc+j] == 1 && im->m[(i+1)*im->nc+j] != 1)
						if (nays8_stent(im, i, j) != 1  && Yokoi_stent (im, i, j) == 1)
							im->m[i*im->nc+j] = 2;

/* Template M2 bottom-top, left-right */
	  for (j=1; j<im->nc-1; j++)
	    for (i=im->nr-2; i>=1; i--)
	      if (im->m[i*im->nc+j] == 0)
					if (im->m[i*im->nc+j-1] == 1 && im->m[i*im->nc+j+1] != 1)
						if (nays8_stent(im, i, j) != 1  && Yokoi_stent (im, i, j) == 1)
							im->m[i*im->nc+j] = 2;

/* Template M3 right-left, bottom-top */
	  for (i=im->nr-2; i>=1; i--)
	    for (j=im->nc-2; j>=1; j--)
	      if (im->m[i*im->nc+j] == 0)
					if (im->m[(i-1)*im->nc+j] != 1 && im->m[(i+1)*im->nc+j] == 1)
						if (nays8_stent(im, i, j) != 1  && Yokoi_stent (im, i, j) == 1)
							im->m[i*im->nc+j] = 2;

/* Template M4 */
	  for (j=im->nc-2; j>=1; j--)
	    for (i=1; i<im->nr-1; i++)
	      if (im->m[i*im->nc+j] == 0)
					if (im->m[i*im->nc+j-1] != 1 && im->m[i*im->nc+j+1] == 1)
						if (nays8_stent(im, i, j) != 1  && Yokoi_stent (im, i, j) == 1)
							im->m[i*im->nc+j] = 2;

/* Delete pixels that are marked (== 2) */
	  for (i=1; i<im->nr-1; i++)
	    for (j=1; j<im->nc-1; j++)
	      if (im->m[i*im->nc+j] == 2)
	      {
					im->m[i*im->nc+j] = 1;
					again = 1;
	      }

	  /*Display (im); */
	}

	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	    if (im->m[i*im->nc+j] > 0)
				im->m[i*im->nc+j] = 255;

	/* Display (im); */
}

int nays8_stent (uc1D *im, int r, int c)
{
	int i,j,k=0;

	for (i=r-1; i<=r+1; i++)
	  for (j=c-1; j<=c+1; j++)
	    if (i!=r || c!=j)
	      if (im->m[i*im->nc+j] == 0)
					k++;
	return k;
}

int Yokoi_stent (uc1D *im, int r, int c)
{
	int N[9];
	int i,k, i1, i2;

	N[0] = im->m[r*im->nc+c]      != 0;
	N[1] = im->m[r*im->nc+c+1]    != 0;
	N[2] = im->m[(r-1)*im->nc+c+1]  != 0;
	N[3] = im->m[(r-1)*im->nc+c]    != 0;
	N[4] = im->m[(r-1)*im->nc+c-1]  != 0;
	N[5] = im->m[r*im->nc+c-1]    != 0;
	N[6] = im->m[(r+1)*im->nc+c-1]  != 0;
	N[7] = im->m[(r+1)*im->nc+c]    != 0;
	N[8] = im->m[(r+1)*im->nc+c+1]  != 0;

	k = 0;
	for (i=1; i<=7; i+=2)
	{
	  i1 = i+1; if (i1 > 8) i1 -= 8;
	  i2 = i+2; if (i2 > 8) i2 -= 8;
	  k += (N[i] - N[i]*N[i1]*N[i2]);
	}

	return k;
}
