/* Zhang & Suen's thinnning algorithm */

#include "../cvThinZ.h"
#include <string.h>

unsigned char cvThinZ_im[242*322]={0};

static void thnz (uc1D *im);
static int nays8_z (uc1D *im, int r, int c);
static int Connectivity_z (uc1D *im, int r, int c);
static void Delete_z (uc1D *im, uc1D *tmp);

void cvThinZ(uc1D *im_ori, uc1D *outimg)
{
	uc1D im;
	int i,j;
	memset(cvThinZ_im,0,242*322);
	im.nc=im_ori->nc+2;
	im.nr=im_ori->nr+2;
	im.m=cvThinZ_im;
	
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

	thnz (&im);

	for (i=0; i<im_ori->nr; i++)
		for (j=0; j<im_ori->nc; j++)
			outimg->m[i*im_ori->nc+j] = im.m[(i+1)*im.nc+j+1];

}

void thnz (uc1D *im)
{
	int i,j,k, again=1;
	uc1D tmp;

	tmp = uc1D_Initial (im->nr, im->nc);

/* BLACK = 1, WHITE = 0. */
	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	  {
			if (im->m[i*im->nc+j] > 0) im->m[i*im->nc+j] = 0;
	    else im->m[i*im->nc+j] = 1;
	    tmp.m[i*im->nc+j] = 0;
	  }

/* Mark and delete */
	while (again)
	{
	  again = 0;

/* Second sub-iteration */
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	  {
	    if (im->m[i*im->nc+j] != 1) continue;
	    k = nays8_z(im, i, j);
	    if ((k >= 2 && k <= 6) && Connectivity_z(im, i,j)==1)
	    {
	      if (im->m[i*im->nc+j+1]*im->m[(i-1)*im->nc+j]*im->m[i*im->nc+j-1] == 0 &&
						im->m[(i-1)*im->nc+j]*im->m[(i+1)*im->nc+j]*im->m[i*im->nc+j-1] == 0)
				{
					tmp.m[i*im->nc+j] = 1;
					again = 1;
				}
	    }
	  }

	  Delete_z (im, &tmp);
	  if (again == 0) break;

/* First sub-iteration */
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	  {
	    if (im->m[i*im->nc+j] != 1) continue;
	    k = nays8_z(im, i, j);
	    if ((k >= 2 && k <= 6) && Connectivity_z(im, i,j)==1)
	    {
	      if (im->m[(i-1)*im->nc+j]*im->m[i*im->nc+j+1]*im->m[(i+1)*im->nc+j] == 0 &&
						im->m[i*im->nc+j+1]*im->m[(i+1)*im->nc+j]*im->m[i*im->nc+j-1] == 0)
				{
					tmp.m[i*im->nc+j] = 1;
					again = 1;
				}
	    }
	  }

	  Delete_z (im, &tmp);
	}

/* Restore levels */
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
			if (im->m[i*im->nc+j] > 0) im->m[i*im->nc+j] = 0;
	    else im->m[i*im->nc+j] = 255;

	uc1D_Free (tmp);
}

void Delete_z (uc1D *im, uc1D *tmp)
{
	int i,j;

/* Delete pixels that are marked  */
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	    if (tmp->m[i*im->nc+j])
	    {
				im->m[i*im->nc+j] = 0;
				tmp->m[i*im->nc+j] = 0;
	    }
}

int nays8_z (uc1D *im, int r, int c)
{
	int i,j,k=0;

	for (i=r-1; i<=r+1; i++)
	  for (j=c-1; j<=c+1; j++)
	    if (i!=r || c!=j)
	      if (im->m[i*im->nc+j] >= 1) k++;
	return k;
}

int Connectivity_z (uc1D *im, int r, int c)
{
	int N=0;

	if (im->m[r*im->nc+c+1]   >= 1 && im->m[(r-1)*im->nc+c+1] == 0) N++;
	if (im->m[(r-1)*im->nc+c+1] >= 1 && im->m[(r-1)*im->nc+c]   == 0) N++;
	if (im->m[(r-1)*im->nc+c]   >= 1 && im->m[(r-1)*im->nc+c-1] == 0) N++;
	if (im->m[(r-1)*im->nc+c-1] >= 1 && im->m[r*im->nc+c-1]   == 0) N++;
	if (im->m[r*im->nc+c-1]   >= 1 && im->m[(r+1)*im->nc+c-1] == 0) N++;
	if (im->m[(r+1)*im->nc+c-1] >= 1 && im->m[(r+1)*im->nc+c]   == 0) N++;
	if (im->m[(r+1)*im->nc+c]   >= 1 && im->m[(r+1)*im->nc+c+1] == 0) N++;
	if (im->m[(r+1)*im->nc+c+1] >= 1 && im->m[r*im->nc+c+1]   == 0) N++;

	return N;
}
