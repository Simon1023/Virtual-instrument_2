//
// Dynamic 2D array memory allocation in C
//  2015.3.29 by Pierre Chen
// MIAT lab, National Central University
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array_1d.h"

uc1D uc1D_Initial(int nr, int nc)
{
	uc1D img, errorimg;		/* new pixel array */
	int i;
	
	errorimg.nc = 0;
	errorimg.nr = 0;
	errorimg.m = 0;
	
	if (nr < 0 || nc < 0) {
		return errorimg;
	}
	
	img.nr = nr;       img.nc = nc;
/*      Allocate the pixel array        */
	img.m = (unsigned char*)calloc(nr*nc,sizeof(unsigned char));
/* Pointers to rows */
	if (!(img.m)) {
		return errorimg;
	}

	return img;
}

void uc1D_Free(uc1D myArray)
{
	free(myArray.m);
}

void uc1D_Copy(uc1D *a, uc1D *b)
{
	int i, j, index;
	
	if (a == b) return;
	*a = uc1D_Initial (b->nr, b->nc);
	if (a == 0) return;
	if (a->nr != b->nr || a->nc != b->nc) return;
		
	for (i=0, index = 0; i<b->nr; i++, index += b->nc)
	    memcpy(&a->m[index], &b->m[index], b->nc * sizeof(unsigned char));	
}

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */
int uc1D_Range (uc1D *im, int i, int j)
{
	if ((i<0) || (i>=im->nr)) return 0;
	if ((j<0) || (j>=im->nc)) return 0;
	return 1;
}

i1D i1D_Initial(int nr, int nc)
{
	i1D img, errorimg;		/* new pixel array */
	int i;
	
	errorimg.nc = 0;
	errorimg.nr = 0;
	errorimg.m = 0;
	
	if (nr < 0 || nc < 0) {
		return errorimg;
	}
	
	img.nr = nr;       img.nc = nc;
/*      Allocate the pixel array        */
	img.m = (int*)calloc(nr*nc,sizeof(int));
/* Pointers to rows */
	if (!(img.m)) {
		return errorimg;
	}

	return img;
}

void i1D_Free(i1D myArray)
{
	free(myArray.m);
}


f1D f1D_Initial(int nr, int nc)
{
	f1D img, errorimg;		/* new pixel array */
	int i;
	
	errorimg.nc = 0;
	errorimg.nr = 0;
	errorimg.m = 0;
	
	if (nr < 0 || nc < 0) {
		return errorimg;
	}
	
	img.nr = nr;       img.nc = nc;
/*      Allocate the pixel array        */
	img.m = (float*)calloc(nr*nc,sizeof(float));
/* Pointers to rows */
	if (!(img.m)) {
		return errorimg;
	}

	return img;
}

void f1D_Free(f1D myArray)
{
	free(myArray.m);
}
