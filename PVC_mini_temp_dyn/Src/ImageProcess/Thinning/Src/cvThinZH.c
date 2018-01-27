/* Zhang & Suen's thinnning algorithm */
/* Holt's variation */

#include "../cvThinZH.h"
#include <string.h>

#define TRUE 1
#define FALSE 0
#define NORTH 1
#define SOUTH 3
#define REMOVE_STAIR 1

static void thnzh (uc1D *im);
static void Delete_zh (uc1D *im, uc1D *tmp);
static void check_zh (int v1, int v2, int v3);
static int edge_zh (uc1D *im, int r, int c);
static void stair_zh (uc1D *im, uc1D *tmp, int dir);

static int t00, t01, t11, t01s;

unsigned char cvThinZH_im[242*322]={0};

void cvThinZH (uc1D *im_ori, uc1D *outimg)
{
	uc1D im;
	int i,j;
	im.nc=im_ori->nc+2;
	im.nr=im_ori->nr+2;	
	memset(cvThinZH_im,0,im.nr*im.nc);
	im.m=cvThinZH_im;	
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

	thnzh (&im);

	for (i=0; i<im_ori->nr; i++)
		for (j=0; j<im_ori->nc; j++)
			outimg->m[i*im_ori->nc+j] = im.m[(i+1)*im.nc+j+1];
}

void thnzh (uc1D *im)
{
	int i,j, again=1;
	uc1D tmp;

	tmp = uc1D_Initial (im->nr, im->nc);

/* BLACK = 1, WHITE = 0. */
	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	  {
	    if (im->m[i*im->nc+j] > 0)
				im->m[i*im->nc+j] = 0;
	    else
				im->m[i*im->nc+j] = 1;
	    tmp.m[i*im->nc+j] = 0;
	  }

/* Mark and delete */
	while (again)
	{
	  again = 0;

	  for (i=1; i<im->nr-1; i++)
	    for (j=1; j<im->nc-1; j++)
	    {
	      if (im->m[i*im->nc+j] == 1) 
	      {
	        if(!edge_zh(im,i,j) || 
						(edge_zh(im,i,j+1) && im->m[(i-1)*im->nc+j] && im->m[(i+1)*im->nc+j]) ||
						(edge_zh(im,i+1,j) && im->m[i*im->nc+j-1] && im->m[i*im->nc+j+1]) ||
						(edge_zh(im,i,j+1) && edge_zh(im, i+1,j+1) && edge_zh(im, i+1, j)) )
							tmp.m[i*im->nc+j] = 0;
	        else 
	        {
						tmp.m[i*im->nc+j] = 1;
						again = 1; 
	        }
	      }
				else 
					tmp.m[i*im->nc+j] = 1;
	    }

	  Delete_zh (im, &tmp);
	}

/* Staircase removal */
	if (REMOVE_STAIR)
	{
	  stair_zh (im, &tmp, NORTH);
	  Delete_zh (im, &tmp);
	  stair_zh (im, &tmp, SOUTH);
	  Delete_zh (im, &tmp);
	}

/* Restore levels */
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
			if (im->m[i*im->nc+j] > 0)
				im->m[i*im->nc+j] = 0;
	    else
				im->m[i*im->nc+j] = 255;
			
	uc1D_Free (tmp);
}

void stair_zh (uc1D *im, uc1D *tmp, int dir)
{
	int i,j;
	int N, S, E, W, NE, NW, SE, SW, C;

	if (dir == NORTH)
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	  {
			NW = im->m[(i-1)*im->nc+j-1];
			N = im->m[(i-1)*im->nc+j];
			NE = im->m[(i-1)*im->nc+j+1];
			W = im->m[i*im->nc+j-1];
			C = im->m[i*im->nc+j];
			E = im->m[i*im->nc+j+1];
			SW = im->m[(i+1)*im->nc+j-1];
			S = im->m[(i+1)*im->nc+j];
			SE = im->m[(i+1)*im->nc+j+1];

	   if (dir == NORTH)
	   {
	     if (C && !(N && 
		      ((E && !NE && !SW && (!W || !S)) || 
		       (W && !NW && !SE && (!E || !S)) )) )
	       tmp->m[i*im->nc+j] = 0;		/* Survives */
	     else
	       tmp->m[i*im->nc+j] = 1;
	   }
		 else if (dir == SOUTH)
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

void Delete_zh (uc1D *im, uc1D *tmp)
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

void check_zh (int v1, int v2, int v3)
{
	if (!v2 && (!v1 || !v3)) t00 = TRUE;
	if ( v2 && ( v1 ||  v3)) t11 = TRUE;
	if ( (!v1 && v2) || (!v2 && v3) )
	{
		t01s = t01;
		t01  = TRUE;
	}
}

int edge_zh (uc1D *im, int r, int c)
{
	if (im->m[r*im->nc+c] == 0) return 0;
	t00 = t01 = t01s = t11 = FALSE;

/* CHECK(vNW, vN, vNE) */
	check_zh (im->m[(r-1)*im->nc+c-1], im->m[(r-1)*im->nc+c], im->m[(r-1)*im->nc+c+1]);

/* CHECK (vNE, vE, vSE) */
	check_zh (im->m[(r-1)*im->nc+c+1], im->m[r*im->nc+c+1], im->m[(r+1)*im->nc+c+1]);

/* CHECK (vSE, vS, vSW) */
	check_zh (im->m[(r+1)*im->nc+c+1], im->m[(r+1)*im->nc+c], im->m[(r+1)*im->nc+c-1]);

/* CHECK (vSW, vW, vNW) */
	check_zh (im->m[(r+1)*im->nc+c-1], im->m[r*im->nc+c-1], im->m[(r-1)*im->nc+c-1]);

	return t00 && t11 && !t01s;
}
