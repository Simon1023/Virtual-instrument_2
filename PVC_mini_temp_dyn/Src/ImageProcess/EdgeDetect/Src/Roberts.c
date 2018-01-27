/* Roberts edge detector */

#include "../Roberts.h"

void edRoberts (uc1D *im_ori, uc1D *result)
{
	int i,j,n,m,k;
	
//	for (i=0; i<im_ori->nr; i++)
//	  for (j=0; j<im_ori->nc; j++) 
//	    result->m[i*im_ori->nc+j] = 255;
	memset(result->m,255,im_ori->nr*im_ori->nc);
	
/* Now compute the convolution(filter), scaling */
	for (i=1; i<im_ori->nr-1; i++)
	  for (j=1; j<im_ori->nc-1; j++) 
	  {
	    n = im_ori->m[i*im_ori->nc+j] - im_ori->m[(i+1)*im_ori->nc+j+1];
	    m = im_ori->m[i*im_ori->nc+j+1] - im_ori->m[(i+1)*im_ori->nc+j];
	    k = (int)( sqrt( (double)(n*n + m*m) ));
	    result->m[i*im_ori->nc+j] = k;
	  }

}
