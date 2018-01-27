/* Zhang & Suen's thinnning algorithm */
/* Stentiford's pre-processing
   Holt's post-processing 	*/

#include "..\cvThinBest.h"
#include <string.h>

#define TRUE 1
#define FALSE 0
#define NORTH 1
#define SOUTH 3
#define REMOVE_STAIR 1

unsigned char cvThinBest_im[242*322]={0};
//unsigned char cvThinBest_tmp[240*320]={0};

static void thnz_best (uc1D *im);
static int nays8_best (uc1D *im, int r, int c);
static int Connectivity (uc1D *im, int r, int c);
static void Delete_best (uc1D *im, uc1D *tmp);
static void stair_best (uc1D *im, uc1D *tmp, int dir);
static int Yokoi_best (uc1D *im, int r, int c);
static void pre_smooth_best (uc1D *im);
static void match_du (uc1D *im, int r, int c, int k);
static void aae (uc1D *image);
static int snays_best (uc1D *im, int r, int c);

void cvThinBest (uc1D *im_ori, uc1D *outimg)
{
	uc1D im;
	int i,j;
	memset(cvThinBest_im,0,242*322);
	im.nc=im_ori->nc+2;
	im.nr=im_ori->nr+2;
	im.m=cvThinBest_im;
	
/* Embed input into a slightly larger image */
//	im = uc1D_Initial(im_ori->nr+2, im_ori->nc+2);
	for (i=0; i<im_ori->nr; i++)
	  for (j=0; j<im_ori->nc; j++)
		{
	    if (im_ori->m[i*im_ori->nc+j])
				im.m[(i+1)*im.nc+j+1] = 1;
	    else
				im.m[(i+1)*im.nc+j+1] = 0;
		}
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

/* Pre_process */
	pre_smooth_best (&im);
	aae (&im);

/* Thin */
	thnz_best (&im);

	for (i=0; i<outimg->nr; i++)
           for (j=0; j<outimg->nc; j++)
	      outimg->m[i*outimg->nc+j] = im.m[(i+1)*im.nc+j+1];
	
//	uc1D_Free(im);
}

/*	Zhang-Suen with Holt's staircase removal */
void thnz_best (uc1D *im)
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
	    k = nays8_best(im, i, j);
	    if ((k >= 2 && k <= 6) && Connectivity(im, i,j)==1)
	    {
	      if (im->m[i*im->nc+j+1]*im->m[(i-1)*im->nc+j]*im->m[i*im->nc+j-1] == 0 &&
		  im->m[(i-1)*im->nc+j]*im->m[(i+1)*im->nc+j]*im->m[i*im->nc+j-1] == 0)
		{
		  tmp.m[i*im->nc+j] = 1;
		  again = 1;
		}
	    }
	  }

	  Delete_best (im, &tmp);
	  if (again == 0) break;

/* First sub-iteration */
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	  {
	    if (im->m[i*im->nc+j] != 1) continue;
	    k = nays8_best(im, i, j);
	    if ((k >= 2 && k <= 6) && Connectivity(im, i,j)==1)
	    {
	      if (im->m[(i-1)*im->nc+j]*im->m[i*im->nc+j+1]*im->m[(i+1)*im->nc+j] == 0 &&
		  im->m[i*im->nc+j+1]*im->m[(i+1)*im->nc+j]*im->m[i*im->nc+j-1] == 0)
		{
		  tmp.m[i*im->nc+j] = 1;
		  again = 1;
		}
	    }
	  }

	  Delete_best (im, &tmp);
	}

/* Post_process */
        stair_best (im, &tmp, NORTH);
        Delete_best (im, &tmp);
        stair_best (im, &tmp, SOUTH);
        Delete_best (im, &tmp);

/* Restore levels */
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	  if (im->m[i*im->nc+j] > 0) im->m[i*im->nc+j] = 0;
	    else im->m[i*im->nc+j] = 255;

	uc1D_Free (tmp);
}

/*	Delete any pixel in IM corresponding to a 1 in TMP	*/
void Delete_best (uc1D *im, uc1D *tmp)
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

/*	Number of neighboring 1 pixels	*/
int nays8_best (uc1D *im, int r, int c)
{
	int i,j,k=0;

	for (i=r-1; i<=r+1; i++)
	  for (j=c-1; j<=c+1; j++)
	    if (i!=r || c!=j)
	      if (im->m[i*im->nc+j] >= 1) k++;
	return k;
}

/*	Number of neighboring 0 pixels	*/
int snays_best (uc1D *im, int r, int c)
{
        int i,j,k=0;

	for (i=r-1; i<=r+1; i++)
	  for (j=c-1; j<=c+1; j++)
	    if (i!=r || c!=j)
	      if (im->m[i*im->nc+j] == 0) k++;
	return k;
}

/*	Connectivity by counting black-white transitions on the boundary	*/
int Connectivity (uc1D *im, int r, int c)
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

/*	Stentiford's boundary smoothing method		*/
void pre_smooth_best (uc1D *im)
{
	int i,j;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    if (im->m[i*im->nc+j] == 0)
		if (snays_best(im, i, j) <= 2 && Yokoi_best (im, i, j)<2)
		  im->m[i*im->nc+j] = 2;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    if (im->m[i*im->nc+j] == 2) im->m[i*im->nc+j] = 1;
}

/*	Stentiford's Acute Angle Emphasis	*/
void aae (uc1D *im)
{
	int i,j, again = 0, k;

	again = 0;
	for (k=5; k>= 1; k-=2)
	{
	  for (i=2; i<im->nr-2; i++)
	    for (j=2; j<im->nc-2; j++)
	      if (im->m[i*im->nc+j] == 0)
		match_du (im, i, j, k);

	  for (i=2; i<im->nr-2; i++)
	    for (j=2; j<im->nc-2; j++)
	    if (im->m[i*im->nc+j] == 2)
	    {
		again = 1;
		im->m[i*im->nc+j] = 1;
	    }

	  if (again == 0) break;
	} 
}

/*	Template matches for acute angle emphasis	*/
void match_du (uc1D *im, int r, int c, int k)
{

/* D1 */
	if (im->m[(r-2)*im->nc+c-2] == 0 && im->m[(r-2)*im->nc+c-1] == 0 &&
	    im->m[(r-2)*im->nc+c]   == 1 && im->m[(r-2)*im->nc+c+1] == 0 &&
	    im->m[(r-2)*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 1 && im->m[(r-1)*im->nc+c+1] == 0 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 0 &&
	    im->m[(r+1)*im->nc+c]   == 0 && im->m[(r+1)*im->nc+c+1] == 0 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[(r+2)*im->nc+c-1] == 0 &&
	    im->m[(r+2)*im->nc+c]   == 0 && im->m[(r+2)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}

/* D2 */
	if (k >= 2)
	if (im->m[(r-2)*im->nc+c-2] == 0 && im->m[(r-2)*im->nc+c-1] == 1 &&
	    im->m[(r-2)*im->nc+c]   == 1 && im->m[(r-2)*im->nc+c+1] == 0 &&
	    im->m[(r-2)*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 1 && im->m[(r-1)*im->nc+c+1] == 0 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 0 &&
	    im->m[(r+1)*im->nc+c]   == 0 && im->m[(r+1)*im->nc+c+1] == 0 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[(r+2)*im->nc+c-1] == 0 &&
	    im->m[(r+2)*im->nc+c]   == 0 && im->m[(r+2)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}

/* D3 */
	if (k>=3)
	if (im->m[(r-2)*im->nc+c-2] == 0 && im->m[(r-2)*im->nc+c-1] == 0 &&
	    im->m[(r-2)*im->nc+c]   == 1 && im->m[(r-2)*im->nc+c+1] == 1 &&
	    im->m[(r-2)*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 1 && im->m[(r-1)*im->nc+c+1] == 0 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 0 &&
	    im->m[(r+1)*im->nc+c]   == 0 && im->m[(r+1)*im->nc+c+1] == 0 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[(r+2)*im->nc+c-1] == 0 &&
	    im->m[(r+2)*im->nc+c]   == 0 && im->m[(r+2)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}

/* D4 */
	if (k>=4)
	if (im->m[(r-2)*im->nc+c-2] == 0 && im->m[(r-2)*im->nc+c-1] == 1 &&
	    im->m[(r-2)*im->nc+c]   == 1 && im->m[(r-2)*im->nc+c+1] == 0 &&
	    im->m[(r-2)*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 1 &&
	    im->m[(r-1)*im->nc+c]   == 1 && im->m[(r-1)*im->nc+c+1] == 0 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 0 &&
	    im->m[(r+1)*im->nc+c]   == 0 && im->m[(r+1)*im->nc+c+1] == 0 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[(r+2)*im->nc+c-1] == 0 &&
	    im->m[(r+2)*im->nc+c]   == 0 && im->m[(r+2)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}

/* D5 */
	if (k>=5)
	if (im->m[(r-2)*im->nc+c-2] == 0 && im->m[(r-2)*im->nc+c-1] == 0 &&
	    im->m[(r-2)*im->nc+c]   == 1 && im->m[(r-2)*im->nc+c+1] == 1 &&
	    im->m[(r-2)*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 1 && im->m[(r-1)*im->nc+c+1] == 1 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 0 &&
	    im->m[(r+1)*im->nc+c]   == 0 && im->m[(r+1)*im->nc+c+1] == 0 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[(r+2)*im->nc+c-1] == 0 &&
	    im->m[(r+2)*im->nc+c]   == 0 && im->m[(r+2)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}

/* U1 */
	if (im->m[(r+2)*im->nc+c-2] == 0 && im->m[(r+2)*im->nc+c-1] == 0 &&
	    im->m[(r+2)*im->nc+c]   == 1 && im->m[(r+2)*im->nc+c+1] == 0 &&
	    im->m[(r+2)*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 0 &&
	    im->m[(r+1)*im->nc+c]   == 1 && im->m[(r+1)*im->nc+c+1] == 0 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 0 && im->m[(r-1)*im->nc+c+1] == 0 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 0 && im->m[(r-1)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}

/* U2 */
	if (k>=2)
	if (im->m[(r+2)*im->nc+c-2] == 0 && im->m[(r+2)*im->nc+c-1] == 1 &&
	    im->m[(r+2)*im->nc+c]   == 1 && im->m[(r+2)*im->nc+c+1] == 0 &&
	    im->m[(r+2)*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 0 &&
	    im->m[(r+1)*im->nc+c]   == 1 && im->m[(r+1)*im->nc+c+1] == 0 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 0 && im->m[(r-1)*im->nc+c+1] == 0 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[(r-2)*im->nc+c-1] == 0 &&
	    im->m[(r-2)*im->nc+c]   == 0 && im->m[(r-2)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}

/* U3 */
	if (k>=3)
	if (im->m[(r+2)*im->nc+c-2] == 0 && im->m[(r+2)*im->nc+c-1] == 0 &&
	    im->m[(r+2)*im->nc+c]   == 1 && im->m[(r+2)*im->nc+c+1] == 1 &&
	    im->m[(r+2)*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 0 &&
	    im->m[(r+1)*im->nc+c]   == 1 && im->m[(r+1)*im->nc+c+1] == 0 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 0 && im->m[(r-1)*im->nc+c+1] == 0 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[(r-2)*im->nc+c-1] == 0 &&
	    im->m[(r-2)*im->nc+c]   == 0 && im->m[(r-2)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}

/* U4 */
	if (k>=4)
	if (im->m[(r+2)*im->nc+c-2] == 0 && im->m[(r+2)*im->nc+c-1] == 1 &&
	    im->m[(r+2)*im->nc+c]   == 1 && im->m[(r+2)*im->nc+c+1] == 0 &&
	    im->m[(r+2)*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 1 &&
	    im->m[(r+1)*im->nc+c]   == 1 && im->m[(r+1)*im->nc+c+1] == 0 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 0 && im->m[(r-1)*im->nc+c+1] == 0 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[(r-2)*im->nc+c-1] == 0 &&
	    im->m[(r-2)*im->nc+c]   == 0 && im->m[(r-2)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}

/* U5 */
	if (k>=5)
	if (im->m[(r+2)*im->nc+c-2] == 0 && im->m[(r+2)*im->nc+c-1] == 0 &&
	    im->m[(r+2)*im->nc+c]   == 1 && im->m[(r+2)*im->nc+c+1] == 1 &&
	    im->m[(r+2)*im->nc+c+2] == 0 &&
	    im->m[(r+1)*im->nc+c-2] == 0 && im->m[(r+1)*im->nc+c-1] == 0 &&
	    im->m[(r+1)*im->nc+c]   == 1 && im->m[(r+1)*im->nc+c+1] == 1 &&
	    im->m[(r+1)*im->nc+c+2] == 0 &&
	    im->m[r*im->nc+c-2] == 0 && im->m[r*im->nc+c-1] == 0 &&
	    im->m[r*im->nc+c]   == 0 && im->m[r*im->nc+c+1] == 0 &&
	    im->m[r*im->nc+c+2] == 0 &&
	    im->m[(r-1)*im->nc+c-2] == 0 && im->m[(r-1)*im->nc+c-1] == 0 &&
	    im->m[(r-1)*im->nc+c]   == 0 && im->m[(r-1)*im->nc+c+1] == 0 &&
	    im->m[(r-1)*im->nc+c+2] == 0 &&
	    im->m[(r-2)*im->nc+c-1] == 0 &&
	    im->m[(r-2)*im->nc+c]   == 0 && im->m[(r-2)*im->nc+c+1] == 0 )
	{
		im->m[r*im->nc+c] = 2;
		return;
	}
}

/*	Yokoi's connectivity measure	*/
int Yokoi_best (uc1D *im, int r, int c)
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

void stair_best (uc1D *im, uc1D *tmp, int dir)
{
	int i,j;
	int N, S, E, W, NE, NW, SE, SW, C;

	if (dir == NORTH)
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	  {
	   NW = im->m[(i-1)*im->nc+j-1]; N = im->m[(i-1)*im->nc+j]; NE = im->m[(i-1)*im->nc+j+1];
	   W = im->m[i*im->nc+j-1]; C = im->m[i*im->nc+j]; E = im->m[i*im->nc+j+1];
	   SW = im->m[(i+1)*im->nc+j-1]; S = im->m[(i+1)*im->nc+j]; SE = im->m[(i+1)*im->nc+j+1];

	   if (dir == NORTH)
	   {
	     if (C && !(N && 
		      ((E && !NE && !SW && (!W || !S)) || 
		       (W && !NW && !SE && (!E || !S)) )) )
	       tmp->m[i*im->nc+j] = 0;		/* Survives */
	     else
	       tmp->m[i*im->nc+j] = 1;
	   } else if (dir == SOUTH)
	   {
	     if (C && !(S && 
		      ((E && !SE && !NW && (!W || !N)) || 
		       (W && !SW && !NE && (!E || !N)) )) )
	       tmp->m[i*im->nc+j] = 0;		/* Survives */
	     else
	       tmp->m[i*im->nc+j] = 1;
	   }
	  }
}
