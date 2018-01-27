//
// Dynamic 2D array memory allocation in C
//  2015.3.29 by Pierre Chen
// MIAT lab, National Central University
//
#ifndef ARRAY_1D_H
#define ARRAY_1D_H

typedef struct {
		int nr;
		int nc;
		unsigned char *m;           /* Pixel values */
} uc1D;

uc1D uc1D_Initial(int nr, int nc);

/* Copy data of b to a */
void uc1D_Copy(uc1D *a, uc1D *b);

/*      Check that a pixel index is in range. Return TRUE(1) if so.     */
int uc1D_Range (uc1D *im, int i, int j);

void uc1D_Free(uc1D myArray);

typedef struct {
		int nr;
		int nc;
		int *m;           /* Pixel values */
} i1D;

i1D i1D_Initial(int nr, int nc);

void i1D_Free(i1D myArray);

typedef struct {
		int nr;
		int nc;
		float *m;           /* Pixel values */
} f1D;

f1D f1D_Initial(int nr, int nc);

void f1D_Free(f1D myArray);

#endif
