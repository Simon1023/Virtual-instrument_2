#ifndef ARRAY_2D_H
#define ARRAY_2D_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "array_1d.h"

//------------------------------------------
// unsigned char 2-dimensional dynamic memory allocation
//------------------------------------------

typedef struct {
		int nr;
		int nc;
		unsigned char **m;           /* Pixel values */
} uc2D;

/*	Initial of uc2D	*/
uc2D uc2D_Initial(int nr, int nc);

/*	Free the storage associated with the image	*/
void uc2D_Free(uc2D* img);

/* Copy data of b to a */
void uc2D_Copy(uc2D *a, uc2D *b);

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */
int uc2D_Range (uc2D *im, int i, int j);

//------------------------------------------
// float 2-dimensional dynamic memory allocation
//------------------------------------------

typedef struct {
		int nr;
		int nc;
		float **m;           /* Pixel values */
} f2D;

/*	Initial of uc2D	*/
f2D f2D_Initial(int nr, int nc);

/*	Free the storage associated with the image	*/
void f2D_Free(f2D* img);

/* Copy data of b to a */
void f2D_Copy(f2D *a, f2D *b);

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */
int f2D_Range (f2D *im, int i, int j);

//------------------------------------------
// int 2-dimensional dynamic memory allocation
//------------------------------------------

typedef struct {
		int nr;
		int nc;
		int **m;           /* Pixel values */
} i2D;

/*	Initial of uc2D	*/
i2D i2D_Initial(int nr, int nc);

/*	Free the storage associated with the image	*/
void i2D_Free(i2D* img);

/* Copy data of b to a */
void i2D_Copy(i2D *a, i2D *b);

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */
int i2D_Range (i2D *im, int i, int j);

//----------------------------------------------------
// Conversion between 2-dimensional and 1-dimensional
//----------------------------------------------------

/* 1d to 2d */
void dimension_1to2(uc1D* src, uc2D* dst);

/* 2d to 1d */
void dimension_2to1(uc2D* src, uc1D* dst);

#endif
