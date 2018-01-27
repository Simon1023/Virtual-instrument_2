#include "../cvShen.h"

#define OUTLINE 25

/* Function prototypes */
static void shen(uc2D *im, uc2D *res);
static void compute_ISEF (f2D *x, f2D *y, int nrows, int ncols);
static void apply_ISEF_vertical (f2D *x, f2D *y, f2D *A, f2D *B,int nrows, int ncols);
static void apply_ISEF_horizontal (f2D *x, f2D *y, f2D *A, f2D *B,int nrows, int ncols);
static uc2D compute_bli (f2D *buff1, f2D *buff2, int nrows, int ncols);
static void locate_zero_crossings (f2D *orig, f2D *smoothed, uc2D *bli, int nrows, int ncols);
static void threshold_edges (f2D *in, uc2D *out, int nrows, int ncols);
static int mark_connected (int i, int j,int level);
static int is_candidate_edge (uc2D *buff, f2D *orig, int row, int col);
static float compute_adaptive_gradient (uc2D *BLI_buffer, f2D *orig_buffer, int row, int col);
static void estimate_thresh_shen (double *low, double *hi, int nr, int nc);
static void debed (uc1D *outImg, uc2D *im, int width);
static void embed (uc1D *im_ori, uc2D *im, int width);

/* globals for shen operator*/
static double b = 0.91;				/* smoothing factor 0 < b < 1 */
static double low_thresh=20, high_thresh=22;	/* threshold for hysteresis */
static double ratio_shen = 0.035;
static int window_size = 10;
static int do_hysteresis = 1;
static f2D lap;			/* keep track of laplacian of image */
static int nr, nc;			/* nrows, ncols */
static uc2D edges;			/* keep track of edge points (thresholded) */
static int thinFactor = 1;

void cvShen(uc1D *im_ori, uc1D *outImg)
{
	uc2D im, res;
	
	embed (im_ori, &im, OUTLINE);
	res = uc2D_Initial (im.nr, im.nc);
	shen (&im, &res);
	debed (outImg, &res, OUTLINE);

}

void shen (uc2D *im, uc2D *res)
{
	register int i,j;
	f2D buffer;	
	f2D smoothed_buffer;
	uc2D bli_buffer;
   
/* Convert the input image to floating point */
	buffer = f2D_Initial (im->nr, im->nc);
	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	  {
	  	buffer.m[i][j] = (float)(im->m[i][j]);
	  }
	    

/* Smooth input image using recursively implemented ISEF filter */
	smoothed_buffer =  f2D_Initial( im->nr,  im->nc);
	compute_ISEF (&buffer, &smoothed_buffer, im->nr, im->nc);
	
/* Compute bli image band-limited laplacian image from smoothed image */
	bli_buffer = compute_bli(&smoothed_buffer,&buffer,im->nr,im->nc); 

/* Perform edge detection using bli and gradient thresholding */
	locate_zero_crossings (&buffer, &smoothed_buffer, &bli_buffer, im->nr, im->nc); 
      
	f2D_Free(&smoothed_buffer);
	uc2D_Free(&bli_buffer);
	
	threshold_edges (&buffer, res, im->nr, im->nc);
	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	  {
	  	if (res->m[i][j] > 0)
	  		res->m[i][j] = 0;
	    else
			res->m[i][j] = 255;
	  }
	    
	
	f2D_Free(&buffer);
}

/*	Recursive filter realization of the ISEF 
	(Shen and Castan CVIGP March 1992)	 */
void compute_ISEF (f2D *x, f2D *y, int nrows, int ncols)
{
	f2D A, B;
   
	A = f2D_Initial(nrows, ncols); /* store causal component */
	B = f2D_Initial(nrows, ncols); /* store anti-causal component */
   
/* first apply the filter in the vertical direcion (to the rows) */
	apply_ISEF_vertical (x, y, &A, &B, nrows, ncols);
   
/* now apply the filter in the horizontal direction (to the columns) and */
/* apply this filter to the results of the previous one */
	apply_ISEF_horizontal (x, y, &A, &B, nrows, ncols);
   
   /* free up the memory */
	f2D_Free (&B);
	f2D_Free (&A);
}

void apply_ISEF_vertical (f2D *x, f2D *y, f2D *A, f2D *B, int nrows, int ncols)
{
	register int row, col;
	float b1, b2;
   
	b1 = (1.0 - b)/(1.0 + b);
	b2 = b*b1;
	   
/* compute boundary conditions */
	for (col=0; col<ncols; col++)
	{

/* boundary exists for 1st and last column */
	   A->m[0][col] = b1 * x->m[0][col];	
	   B->m[nrows-1][col] = b2 * x->m[nrows-1][col];
	}
   
/* compute causal component */
	for (row=1; row<nrows; row++)
	  for (col=0; col<ncols; col++)
	    A->m[row][col] = b1 * x->m[row][col] + b * A->m[row-1][col];

/* compute anti-causal component */
	for (row=nrows-2; row>=0; row--)
	  for (col=0; col<ncols; col++)
	    B->m[row][col] = b2 * x->m[row][col] + b * B->m[row+1][col];

/* boundary case for computing output of first filter */
	for (col=0; col<ncols-1; col++)
	  y->m[nrows-1][col] = A->m[nrows-1][col]; 

/* now compute the output of the first filter and store in y */
/* this is the sum of the causal and anti-causal components */
	for (row=0; row<nrows-2; row++)
	  for (col=0; col<ncols-1; col++)
	    y->m[row][col] = A->m[row][col] + B->m[row+1][col];
}  


void apply_ISEF_horizontal (f2D *x, f2D *y, f2D *A, f2D *B, int nrows, int ncols)
{
	register int row, col;
	float b1, b2;
   
	b1 = (1.0 - b)/(1.0 + b);
	b2 = b*b1;
   
/* compute boundary conditions */
	for (row=0; row<nrows; row++)
	{
	   A->m[row][0] = b1 * x->m[row][0];
	   B->m[row][ncols-1] = b2 * x->m[row][ncols-1];
	}

/* compute causal component */
	for (col=1; col<ncols; col++)
	  for (row=0; row<nrows; row++)
	    A->m[row][col] = b1 * x->m[row][col] + b * A->m[row][col-1];

/* compute anti-causal component */
	for (col=ncols-2; col>=0; col--)
	  for (row=0; row<nrows;row++)
	    B->m[row][col] = b2 * x->m[row][col] + b * B->m[row][col+1];

/* boundary case for computing output of first filter */     
	for (row=0; row<nrows; row++)
	  y->m[row][ncols-1] = A->m[row][ncols-1];

/* now compute the output of the second filter and store in y */
/* this is the sum of the causal and anti-causal components */
	for (row=0; row<nrows; row++)
	  for (col=0; col<ncols-1; col++)
	    y->m[row][col] = A->m[row][col] + B->m[row][col+1];
}  

/* compute the band-limited laplacian of the input image */
uc2D compute_bli (f2D *buff1, f2D *buff2, int nrows, int ncols)
{
	register int row, col;
	uc2D bli_buffer;
	float t;
   
	bli_buffer = uc2D_Initial(nrows, ncols);
	for (row=0; row<nrows; row++)
	  for (col=0; col<ncols; col++)
	    bli_buffer.m[row][col] = 0;
   
/* The bli is computed by taking the difference between the smoothed image */
/* and the original image.  In Shen and Castan's paper this is shown to */
/* approximate the band-limited laplacian of the image.  The bli is then */
/* made by setting all values in the bli to 1 where the laplacian is */
/* positive and 0 otherwise. 						*/
	for (row=0; row<nrows; row++)
	  for (col=0; col<ncols; col++)
	  {
        if (row<OUTLINE || row >= nrows-OUTLINE || col<OUTLINE || col >= ncols-OUTLINE) continue;
		t = buff1->m[row][col] - buff2->m[row][col];
		
		if(t>0.0)
			bli_buffer.m[row][col] = 1;
		else
			bli_buffer.m[row][col] = 0;
	  }
	  
	return bli_buffer;
}

void locate_zero_crossings (f2D *orig, f2D *smoothed, uc2D *bli, int nrows, int ncols)
{
	register int row, col;
	float grad;
   
	for (row=0; row<nrows; row++)
	{
	   for (col=0; col<ncols; col++)
	   {

/* ignore pixels around the boundary of the image */
		if (row<OUTLINE || row >= nrows-OUTLINE || col<OUTLINE || col >= ncols-OUTLINE)
		{
			orig->m[row][col] = 0.0;
		}

 /* next check if pixel is a zero-crossing of the laplacian  */
		else if (is_candidate_edge (bli, smoothed, row, col))
		{

/* now do gradient thresholding  */
			grad = compute_adaptive_gradient (bli, smoothed, row, col);
			orig->m[row][col] = grad;
		}
		else
			orig->m[row][col] = 0.0;		    
	  }
	}
}

void threshold_edges (f2D *in, uc2D *out, int nrows, int ncols)
{
	register int i, j;
   
	lap = *in;
	edges = *out;
	nr = nrows;
	nc = ncols;
   
	estimate_thresh_shen (&low_thresh, &high_thresh, nr, nc);
	if (!do_hysteresis)
	  low_thresh = high_thresh;

	for (i=0; i<nrows; i++)
	  for (j=0; j<ncols; j++)
	    edges.m[i][j] = 0; /*<<------ª`·N-------<<*/
   
	for (i=0; i<nrows; i++)
	  for (j=0; j<ncols; j++)
	  {
            if (i<OUTLINE || i >= nrows-OUTLINE || j<OUTLINE || j >= ncols-OUTLINE) continue;

/* only check a contour if it is above high_thresh */
	    if ((lap.m[i][j]) > high_thresh) 

/* mark all connected points above low thresh */
	      mark_connected (i,j,0);	
	  }
   
	for (i=0; i<nrows; i++)	/* erase all points which were 255 */
	  for (j=0; j<ncols; j++)
	    if (edges.m[i][j] == 255) edges.m[i][j] = 0;
}

/*	return true if it marked something */ 
int mark_connected (int i, int j, int level)
{
	 int notChainEnd;
    
   /* stop if you go off the edge of the image */
	if (i >= nr || i < 0 || j >= nc || j < 0) return 0;
   
   /* stop if the point has already been visited */
	if (edges.m[i][j] != 0) return 0;	
   
   /* stop when you hit an image boundary */
	if (lap.m[i][j] == 0.0) return 0;
   
	if ((lap.m[i][j]) > low_thresh)
	{
	   edges.m[i][j] = 1;
	}
	else
	{
	   edges.m[i][j] = 255;
	}
   
	notChainEnd =0;
    
	notChainEnd |= mark_connected(i  ,j+1, level+1);
	notChainEnd |= mark_connected(i  ,j-1, level+1);
	notChainEnd |= mark_connected(i+1,j+1, level+1);
	notChainEnd |= mark_connected(i+1,j  , level+1);
	notChainEnd |= mark_connected(i+1,j-1, level+1);
	notChainEnd |= mark_connected(i-1,j-1, level+1);
	notChainEnd |= mark_connected(i-1,j  , level+1);
	notChainEnd |= mark_connected(i-1,j+1, level+1);

	if (notChainEnd && ( level > 0 ) )
	{
	/* do some contour thinning */
	  if ( thinFactor > 0 )
	  if ( (level%thinFactor) != 0  )
	  {
	    /* delete this point */  
	    edges.m[i][j] = 255;
	  }
	}
    
	return 1;
}

/* finds zero-crossings in laplacian (buff)  orig is the smoothed image */
int is_candidate_edge (uc2D *buff, f2D *orig, int row, int col)
{
/* test for zero-crossings of laplacian then make sure that zero-crossing */
/* sign correspondence principle is satisfied.  i.e. a positive z-c must */
/* have a positive 1st derivative where positive z-c means the 2nd deriv */
/* goes from positive to negative as we pass through the step edge */
   
	if (buff->m[row][col] == 1 && buff->m[row+1][col] == 0) /* positive z-c */
	{ 
	   if (orig->m[row+1][col] - orig->m[row-1][col] > 0) return 1;
	   else return 0;
	}
	else if (buff->m[row][col] == 1 && buff->m[row][col+1] == 0 ) /* positive z-c */
	{
	   if (orig->m[row][col+1] - orig->m[row][col-1] > 0) return 1;
	   else return 0;
	}
	else if ( buff->m[row][col] == 1 && buff->m[row-1][col] == 0) /* negative z-c */
	{
	   if (orig->m[row+1][col] - orig->m[row-1][col] < 0) return 1;
	   else return 0;
	}
	else if (buff->m[row][col] == 1 && buff->m[row][col-1] == 0 ) /* negative z-c */
	{
	   if (orig->m[row][col+1] - orig->m[row][col-1] < 0) return 1;
	   else return 0;
	}
	else			/* not a z-c */
	  return 0;
}

float compute_adaptive_gradient (uc2D *BLI_buffer, f2D *orig_buffer, int row, int col)
{
	register int i, j;
	float sum_on, sum_off;
	float avg_on, avg_off;
	int num_on, num_off;
   
	sum_on = sum_off = 0.0;
	num_on = num_off = 0;
   
	for (i= (-window_size/2); i<=(window_size/2); i++)
	{
	   for (j=(-window_size/2); j<=(window_size/2); j++)
	   {
	     if (BLI_buffer->m[row+i][col+j])
	     {
	        sum_on += orig_buffer->m[row+i][col+j];
	        num_on++;
	     }
	     else
	     {
	        sum_off += orig_buffer->m[row+i][col+j];
	        num_off++;
	     }
	   }
	}
   
	if (sum_off) avg_off = sum_off / num_off;
	else avg_off = 0;
   
	if (sum_on) avg_on = sum_on / num_on;
	else avg_on = 0;
   
	return (avg_off - avg_on);
}

void estimate_thresh_shen (double *low, double *hi, int nr, int nc)
{
	float vmax, vmin, scale, x;
	int i,j,k, *hist, count;

	hist = (int *)malloc(256*sizeof(int));
/* Build a histogram of the Laplacian image. */
	vmin = vmax = fabs((float)(lap.m[20][20]));
	for (i=0; i<nr; i++)
	  for (j=0; j<nc; j++)
	  {
            if (i<OUTLINE || i >= nr-OUTLINE ||
                  j<OUTLINE || j >= nc-OUTLINE) continue;
	    x = lap.m[i][j];
	    if (vmin > x) vmin = x;
	    if (vmax < x) vmax = x;
	  }
	for (k=0; k<256; k++) hist[k] = 0;

	scale = 256.0/(vmax-vmin + 1);

	for (i=0; i<nr; i++)
	  for (j=0; j<nc; j++)
	  {
            if (i<OUTLINE || i >= nr-OUTLINE ||
                  j<OUTLINE || j >= nc-OUTLINE) continue;
	    x = lap.m[i][j];
	    k = (int)((x - vmin)*scale);
	    hist[k] += 1;
	  }

/* The high threshold should be > 80 or 90% of the pixels */
	k = 255;
	j = (int)(ratio_shen*nr*nc);
	count = hist[255];
	while (count < j)
	{
	  k--;
	  if (k<0) break;
	  count += hist[k];
	}
	*hi = (double)k/scale   + vmin ;
	*low = (*hi)/2;
}

void embed (uc1D* im_ori, uc2D *im, int width)
{
	int i,j,I,J;
	width += 2;
	*im = uc2D_Initial (im_ori->nr+width+width, im_ori->nc+width+width);

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	  {
	  		
	    I = (i-width+im_ori->nr)%im_ori->nr;
	    
	    J = (j-width+im_ori->nc)%im_ori->nc;
	    
//		im->m[i][j] = im_ori->m[I][J];
		im->m[i][j] = im_ori->m[I*im_ori->nc+J];
	  }
	
}

void debed (uc1D* outImg, uc2D *im, int width)
{
	int i,j;

	width +=2;
	for (i=0; i<outImg->nr-1; i++)
	{
	  for (j=1; j<outImg->nc; j++)
	  {
	  	outImg->m[i*outImg->nc+j]= im->m[i+width][j+width];
//	    outImg->m[i][j] = im->m[i+width][j+width];
//	    outImg->m[outImg->nr-1][j] = 255;
	    outImg->m[(outImg->nr-1)*outImg->nc+j]=255;
	  }
//	  outImg->m[i][0] = 255;
	  outImg->m[i*outImg->nc+0]=255;
	}

}
