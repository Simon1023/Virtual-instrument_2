#include "array_2d.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------
// unsigned char 2-dimensional dynamic memory allocation
//------------------------------------------

/*	Initial of uc2D	*/
uc2D uc2D_Initial(int nr, int nc)
{
	uc2D img, errorimg;		/* new pixel array */
	int i;
	
	errorimg.nc = 0;
	errorimg.nr = 0;
	errorimg.m = 0;
	
	if (nr < 0 || nc < 0) {
		return errorimg;
	}
	
	img.nr = nr;       img.nc = nc;
/*      Allocate the pixel array        */
	img.m = (unsigned char**) calloc(nr,sizeof(unsigned char*));  
	for (i = 0; i < nr; i++)  
		img.m[i] = (unsigned char*) calloc(nc,sizeof(unsigned char));  
/* Pointers to rows */
	if (!(img.m)) {
		return errorimg;
	}

	return img;
}

/*	Free the storage associated with the image	*/
void uc2D_Free(uc2D* img)
{
	int i;
	if (img != 0)
	{
		for (i = 0; i < img->nr; i++) 
			free (img->m[i]);
		free(img->m);
	}
}

void uc2D_Copy(uc2D *a, uc2D *b)
{
	int i, j, index;
	
	if (a == b) return;
	*a = uc2D_Initial (b->nr, b->nc);
	if (a == 0) return;
	if (a->nr != b->nr || a->nc != b->nc) return;
		
	for (i=0, index = 0; i<b->nr; i++, index += b->nc)
	    memcpy(&a->m[index], &b->m[index], b->nc * sizeof(unsigned char));	
}

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */
int uc2D_Range (uc2D *im, int i, int j)
{
	if ((i<0) || (i>=im->nr)) return 0;
	if ((j<0) || (j>=im->nc)) return 0;
	return 1;
}

//------------------------------------------
// float 2-dimensional dynamic memory allocation
//------------------------------------------

/*	Initial of uc2D	*/
f2D f2D_Initial(int nr, int nc)
{
	f2D img, errorimg;		/* new pixel array */
	int i;
	
	errorimg.nc = 0;
	errorimg.nr = 0;
	errorimg.m = 0;
	
	if (nr < 0 || nc < 0) {
		return errorimg;
	}
	
	img.nr = nr;       img.nc = nc;
/*      Allocate the pixel array        */
	img.m = (float**) calloc(nr,sizeof(float*));  
	for (i = 0; i < nr; i++)  
		img.m[i] = (float*) calloc(nc,sizeof(float));  
/* Pointers to rows */
	if (!(img.m)) {
		return errorimg;
	}

	return img;
}

/*	Free the storage associated with the image	*/
void f2D_Free(f2D* img)
{
	int i;
	if (img != 0)
	{
		for (i = 0; i < img->nr; i++)  
			free (img->m[i]);
		free(img->m);
	}
}

void f2D_Copy(f2D *a, f2D *b)
{
	int i, j, index;
	
	if (a == b) return;
	*a = f2D_Initial (b->nr, b->nc);
	if (a == 0) return;
	if (a->nr != b->nr || a->nc != b->nc) return;
		
	for (i=0, index = 0; i<b->nr; i++, index += b->nc)
	    memcpy(&a->m[index], &b->m[index], b->nc * sizeof(float));	
}

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */
int f2D_Range (f2D *im, int i, int j)
{
	if ((i<0) || (i>=im->nr)) return 0;
	if ((j<0) || (j>=im->nc)) return 0;
	return 1;
}

//------------------------------------------
// int 2-dimensional dynamic memory allocation
//------------------------------------------

/*	Initial of uc2D	*/
i2D i2D_Initial(int nr, int nc)
{
	i2D img, errorimg;		/* new pixel array */
	int i;
	
	errorimg.nc = 0;
	errorimg.nr = 0;
	errorimg.m = 0;
	
	if (nr < 0 || nc < 0) {
		return errorimg;
	}
	
	img.nr = nr;       img.nc = nc;
/*      Allocate the pixel array        */
	img.m = (int**) calloc(nr,sizeof(int*));  
	for (i = 0; i < nr; i++)  
		img.m[i] = (int*) calloc(nc,sizeof(int));  
/* Pointers to rows */
	if (!(img.m)) {
		return errorimg;
	}

	return img;
}

/*	Free the storage associated with the image	*/
void i2D_Free(i2D* img)
{
	int i;
	if (img != 0)
	{
		for (i = 0; i < img->nr; i++)  
			free (img->m[i]);
		free(img->m);
	}
}

void i2D_Copy(i2D *a, i2D *b)
{
	int i, j, index;
	
	if (a == b) return;
	*a = i2D_Initial (b->nr, b->nc);
	if (a == 0) return;
	if (a->nr != b->nr || a->nc != b->nc) return;
		
	for (i=0, index = 0; i<b->nr; i++, index += b->nc)
	    memcpy(&a->m[index], &b->m[index], b->nc * sizeof(int));	
}

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */
int i2D_Range (i2D *im, int i, int j)
{
	if ((i<0) || (i>=im->nr)) return 0;
	if ((j<0) || (j>=im->nc)) return 0;
	return 1;
}

//----------------------------------------------------
// Conversion between 2-dimensional and 1-dimensional
//----------------------------------------------------

/* 1d to 2d */
void dimension_1to2(uc1D* src, uc2D* dst)
{
	int i, index;
	if(src->nc != dst->nc || src->nr != dst->nr) return;
	for(i=0, index=0; i<dst->nr; i++, index+=dst->nc)
		memcpy(dst->m[i], &src->m[index], sizeof(unsigned char) * dst->nc);
}

/* 2d to 1d */
void dimension_2to1(uc2D* src, uc1D* dst)
{
	int i, index;
	if(src->nc != dst->nc || src->nr != dst->nr) return;
	for(i=0, index=0; i<dst->nr; i++, index+=dst->nc)
		memcpy(&dst->m[index], src->m[i], sizeof(unsigned char) * dst->nc);
}

