#include "../Canny.h"
/* Scale floating point magnitudes and angles to 8 bits */
#define ORI_SCALE 40.0f
#define MAG_SCALE 20.0f
/* Biggest possible filter mask */
#define MAX_MASK_SIZE 20
/* Fraction of pixels that should be above the HIGH threshold */
//static float ratio_canny = 0.1f;
static int WIDTH = 0;

/*		Canny Edge DETECTOR 	*/
static void trace (int high, int low, uc1D *im, uc1D *mag, uc1D *ori);
static float gauss(float x, float sigma);
static float dGauss (float x, float sigma);
static float meanGauss (float x, float sigma);
static void hysteresis (int high, int low, uc1D *im, uc1D *mag, uc1D *oriim);
static void canny_filter (float s, uc1D *im, uc1D *mag, uc1D *ori);
static void seperable_convolution (uc1D *im, float *gau, int width,f1D *smx, f1D *smy);
static void dxy_seperable_convolution (f1D *im, int nr, int nc, float *gau,int width, f1D *sm, int which);
static void nonmax_suppress (f1D *dx, f1D *dy, int nr, int nc,uc1D *mag, uc1D *ori);
static void estimate_thresh_canny (uc1D *mag, int *low, int *hi);

//low=3 high=5 s=4.0
void cvCanny(uc1D *im, uc1D *result, int low, int high, float s)
{
	int i,j;
	uc1D magim, oriim;
	
	//Initial
	magim = uc1D_Initial (im->nr, im->nc);
	oriim = uc1D_Initial (im->nr, im->nc);
	/* Apply the filter */
	canny_filter(s, im, &magim, &oriim);
	
	/* Hysteresis thresholding of edge pixels */
	hysteresis (high, low, result, &magim, &oriim);
	
	for (i=0; i<WIDTH; i++)
	  for (j=0; j<im->nc; j++)
	    result->m[i*im->nc+j] = 255;

	for (i=im->nr-1; i>im->nr-1-WIDTH; i--)
	  for (j=0; j<im->nc; j++)
	    result->m[i*im->nc+j] = 255;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<WIDTH; j++)
	    result->m[i*im->nc+j] = 255;

	for (i=0; i<im->nr; i++)
	  for (j=im->nc-WIDTH-1; j<im->nc; j++)
		result->m[i*im->nc+j] = 255;

	WIDTH = 0;
	
}

float norm (float x, float y)
{
	return (float) sqrt ( (double)(x*x + y*y) );
}

void canny_filter (float s, uc1D *im, uc1D *mag, uc1D *ori)
{
	int width;
	f1D smx, smy;
	f1D dx, dy;
	int i,j;//,n;
	float *gau, *dgau, z;
	
	gau = (float*)malloc(MAX_MASK_SIZE*sizeof(float));
	dgau = (float*)malloc(MAX_MASK_SIZE*sizeof(float));

/* Create a Gaussian and a derivative of Gaussian filter mask */
	for(i=0; i<MAX_MASK_SIZE; i++)
	{
	  gau[i] = meanGauss ((float)i, s);
	  if (gau[i] < 0.005f)
	  {
		width = i;
		break;
	  }
	  dgau[i] = dGauss ((float)i, s);
	}

	//n = width+width + 1;
	WIDTH = width/2;
	
	smx = f1D_Initial (im->nr, im->nc);
	smy = f1D_Initial (im->nr, im->nc);

/* Convolution of source image with a Gaussian in X and Y directions  */
	seperable_convolution (im, gau, width, &smx, &smy);

/* Now convolve smoothed data with a derivative */
	dx = f1D_Initial (im->nr, im->nc);
	dxy_seperable_convolution (&smx, im->nr, im->nc,
		 dgau, width, &dx, 1);
	//f1D_Free(&smx);

	dy = f1D_Initial (im->nr, im->nc);
	dxy_seperable_convolution (&smy, im->nr, im->nc,
		 dgau, width, &dy, 0);
	//f1D_Free(&smy);

/* Create an image of the norm of dx,dy */
	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	  {
	      z = norm (dx.m[i*im->nc+j], dy.m[i*im->nc+j]);
	      mag->m[i*im->nc+j] = (unsigned char)(z*MAG_SCALE);
	  }

/* Non-maximum suppression - edge pixels should be a local max */

	nonmax_suppress (&dx, &dy, im->nr, im->nc, mag, ori);

	//f1D_Free(&dx);
	//f1D_Free(&dy);
	
}
/*      Gaussian        */
float gauss(float x, float sigma)
{
    float xx;

	if (sigma == 0) return 0.0f;
    xx = (float)exp((double) ((-x*x)/(2*sigma*sigma)));
    return xx;
}
float meanGauss (float x, float sigma)
{
	float z;

	z = (gauss(x,sigma)+gauss(x+0.5f,sigma)+gauss(x-0.5f,sigma))/3.0f;
	z = z/(3.141592*2.0*sigma*sigma);
	return z;
}
/*      First derivative of Gaussian    */
float dGauss (float x, float sigma)
{
	return -x/(sigma*sigma) * gauss(x, sigma);
}
/*      HYSTERESIS thersholding of edge pixels. Starting at pixels with a
	value greater than the HIGH threshold, trace a connected sequence
	of pixels that have a value greater than the LOW threhsold.        */

void hysteresis (int high, int low, uc1D *result, uc1D *mag, uc1D *oriim)
{
	int i,j;

	// Beginning hysteresis thresholding
	for (i=0; i<result->nr; i++)
	  for (j=0; j<result->nc; j++)
	    result->m[i*result->nc+j] = 0;

	if (high<low)
	{
	  estimate_thresh_canny (mag, &high, &low);
	  //printf ("Hysteresis thresholds (from image): HI %d LOW %D\n",
			//high, low);
	}
	
/* For each edge with a magnitude above the high threshold, begin
   tracing edge pixels that are above the low threshold.                */
  trace (high, low, result, mag, oriim);

/* Make the edge black (to be the same as the other methods) */
	for (i=0; i<result->nr; i++)
	  for (j=0; j<result->nc; j++)
	    if (result->m[i*result->nc+j] == 0) result->m[i*result->nc+j] = 255;
	    else result->m[i*result->nc+j] = 0;
	    
}

/*      TRACE - recursively trace edge pixels that have a threshold > the low
	edge threshold, continuing from the pixel at (i,j).                     */

void trace (int high, int low, uc1D *im, uc1D *mag, uc1D *ori)
{
	int i, j, k, count, *i_stack, *j_stack, *nb_i, *nb_j, it, jt;
	
	i_stack = (int*)malloc( im->nr*im->nc/2*sizeof(int) );
	j_stack = (int*)malloc( im->nr*im->nc/2*sizeof(int) );
	//around points
	nb_i = (int*)malloc( 8*sizeof(int) );
	nb_j = (int*)malloc( 8*sizeof(int) );
	
	for (i = 1; i < im->nr - 1; i++)for (j = 1; j < im->nc - 1; j++)
	{
		if(mag->m[i*im->nc+j] >= high && im->m[i*im->nc+j] == 0)
		{
			im->m[i*im->nc+j] = 255;
			count = 1;	//stack times
			i_stack[0] = i;
			j_stack[0] = j;
			
			do{
				count--;
				
				nb_i[0]=i_stack[count]-1; nb_j[0]=j_stack[count]-1;	//left top
				nb_i[1]=i_stack[count]-1; nb_j[1]=j_stack[count];	//Top
				nb_i[2]=i_stack[count]-1; nb_j[2]=j_stack[count]+1;	//Right top
				nb_i[3]=i_stack[count]; nb_j[3]=j_stack[count]-1;	//left
				nb_i[4]=i_stack[count]; nb_j[4]=j_stack[count]+1;	//Right
				nb_i[5]=i_stack[count]+1; nb_j[5]=j_stack[count]-1;	//left bottom
				nb_i[6]=i_stack[count]+1; nb_j[6]=j_stack[count];	//Bottom
				nb_i[7]=i_stack[count]+1; nb_j[7]=j_stack[count]+1;	//Right bottom
				
				for(k=0;k<8;k++)
				{
					if (uc1D_Range(mag, nb_i[k], nb_j[k]))
					{
						it = nb_i[k];
						jt = nb_j[k];
						if (mag->m[it*im->nc+jt] >= low && im->m[it*im->nc+jt] == 0)
						{
							im->m[it*im->nc+jt] = 255;
							i_stack[count] = it;
							j_stack[count] = jt;
							count++;
						}
					}
				}
			}while(count>0);
		}
	}
}

void seperable_convolution (uc1D *im, float *gau, int width,
		f1D *smx, f1D *smy)
{
	int i,j,k, I1, I2, nr, nc;
	float x, y;

	nr = im->nr;
	nc = im->nc;

	for (i=0; i<nr; i++)
	  for (j=0; j<nc; j++)
	  {
	    x = gau[0] * im->m[i*nc+j]; y = gau[0] * im->m[i*nc+j];
	    for (k=1; k<width; k++)
	    {
	      I1 = (i+k)%nr; I2 = (i-k+nr)%nr;
	      y += gau[k]* im->m[I1*nc+j] + gau[k]* im->m[I2*nc+j];
	      I1 = (j+k)%nc; I2 = (j-k+nc)%nc;
	      x += gau[k]* im->m[i*nc+I1] + gau[k]* im->m[i*nc+I2];
	    }
	    smx->m[i*nc+j] = x; smy->m[i*nc+j] = y;
	  }
}

void dxy_seperable_convolution (f1D *im, int nr, int nc,  float *gau,
		int width, f1D *sm, int which)
{
	int i,j,k, I1, I2;
	float x;

	for (i=0; i<nr; i++)
	  for (j=0; j<nc; j++)
	  {
	    x = 0.0;
	    for (k=1; k<width; k++)
	    {
	      if (which == 0)
	      {
					I1 = (i+k)%nr; I2 = (i-k+nr)%nr;
					x += -gau[k]*im->m[I1*nc+j] + gau[k]*im->m[I2*nc+j];
	      }
	      else
	      {
					I1 = (j+k)%nc; I2 = (j-k+nc)%nc;
					x += -gau[k]*im->m[i*nc+I1] + gau[k]*im->m[i*nc+I2];
	      }
	    }
	    sm->m[i*nc+j] = x;
	  }
}

void nonmax_suppress (f1D *dx, f1D *dy, int nr, int nc,
		uc1D* mag, uc1D* ori)
{
	int i,j;
	float xx, yy, g2, g1, g3, g4, g, xc, yc;

	for (i=1; i<mag->nr-1; i++)
	{
	  for (j=1; j<mag->nc-1; j++)
	  {
	    mag->m[i*mag->nc+j] = 0;

/* Treat the x and y derivatives as components of a vector */
	    xc = dx->m[i*nc+j];
	    yc = dy->m[i*nc+j];
	    if (fabs(xc)<0.01 && fabs(yc)<0.01) continue;

	    g  = norm (xc, yc);

/* Follow the gradient direction, as indicated by the direction of
   the vector (xc, yc); retain pixels that are a local maximum. */

	    if (fabs(yc) > fabs(xc))
	    {

/* The Y component is biggest, so gradient direction is basically UP/DOWN */
	      xx = fabs(xc)/fabs(yc);
	      yy = 1.0;

	      g2 = norm (dx->m[(i-1)*nc+j], dy->m[(i-1)*nc+j]);
	      g4 = norm (dx->m[(i+1)*nc+j], dy->m[(i+1)*nc+j]);
	      if (xc*yc > 0.0f)
	      {
					g3 = norm (dx->m[(i+1)*nc+j+1], dy->m[(i+1)*nc+j+1]);
					g1 = norm (dx->m[(i-1)*nc+j-1], dy->m[(i-1)*nc+j-1]);
	      } else
	      {
					g3 = norm (dx->m[(i+1)*nc+j-1], dy->m[(i+1)*nc+j-1]);
					g1 = norm (dx->m[(i-1)*nc+j+1], dy->m[(i-1)*nc+j+1]);
	      }

	    } else
	    {

/* The X component is biggest, so gradient direction is basically LEFT/RIGHT */
	      xx = fabs(yc)/fabs(xc);
	      yy = 1.0;

	      g2 = norm (dx->m[i*nc+j+1], dy->m[i*nc+j+1]);
	      g4 = norm (dx->m[i*nc+j-1], dy->m[i*nc+j-1]);
	      if (xc*yc > 0.0f)
	      {
					g3 = norm (dx->m[(i-1)*nc+j-1], dy->m[(i-1)*nc+j-1]);
					g1 = norm (dx->m[(i+1)*nc+j+1], dy->m[(i+1)*nc+j+1]);
	      }
	      else
	      {
					g1 = norm (dx->m[(i-1)*nc+j+1], dy->m[(i-1)*nc+j+1]);
					g3 = norm (dx->m[(i+1)*nc+j-1], dy->m[(i+1)*nc+j-1]);
	      }
	    }

/* Compute the interpolated value of the gradient magnitude */
	    if ( (g > (xx*g1 + (yy-xx)*g2)) &&
		 (g > (xx*g3 + (yy-xx)*g4)) )
	    {
				if (g*MAG_SCALE <= 255)
					mag->m[i*mag->nc+j] = (unsigned char)(g*MAG_SCALE);
				else
					mag->m[i*mag->nc+j] = 255;
				ori->m[i*mag->nc+j] = atan2 (yc, xc) * ORI_SCALE;
		} else {
				mag->m[i*mag->nc+j] = 0;
				ori->m[i*mag->nc+j] = 0;
	    }
	  }
	}
}

void estimate_thresh_canny (uc1D *mag, int *hi, int *low)
{
	int i,j,k, *hist, count;
	
	hist = (int *)malloc(256*sizeof(int));
/* Build a histogram of the magnitude image. */
	for (k=0; k<256; k++) hist[k] = 0;

	for (i=WIDTH; i<mag->nr-WIDTH; i++)
	  for (j=WIDTH; j<mag->nc-WIDTH; j++)
	    hist[mag->m[i*mag->nc+j]]++;

/* The high threshold should be > 80 or 90% of the pixels
	j = (int)(ratio_canny*mag->info->nr*mag->info->nc);
*/
	j = mag->nr;
	if (j<mag->nc) j = mag->nc;
	j = (int)(0.9*j);
	k = 255;

	count = hist[255];
	while (count < j)
	{
	  k--;
	  if (k<0) break;
	  count += hist[k];
	}
	*hi = k;

	i=0;
	while (hist[i]==0) i++;

	*low = (*hi+i)/2.0;
}
