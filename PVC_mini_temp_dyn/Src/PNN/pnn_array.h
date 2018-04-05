#include <stdlib.h>
/*---global variable---*/
int i=0,j=0;

//------------------------------------------
// unsigned char 1-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr;
	unsigned char *m;	//array value
}uchar1d;

void freeuchar1d(uchar1d myArray)
{
	free(myArray.m);
}

uchar1d uchar1d_initialize(int nr)
{
	uchar1d theArray;
	theArray.nr=nr;
	theArray.m = (unsigned char *) malloc(nr*sizeof(unsigned char*));
	return theArray;
}
//------------------------------------------
// unsigned int 1-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr;
	unsigned int *m;
}uint1d;

void freeuint1d(uint1d myArray)
{
	free(myArray.m);
}

uint1d uint1d_initialize(int nr)
{
	uint1d theArray;
	theArray.nr=nr;
	theArray.m = (unsigned int *) malloc(nr*sizeof(unsigned int*));
	return theArray;
}
//------------------------------------------
// int 1-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr;
	int *m;
}int1d;

void freeint1d(int1d myArray)
{
	free(myArray.m);
}

int1d int1d_initialize(int nr)
{
	int1d theArray;
	theArray.nr=nr;
	theArray.m = (int *) malloc(nr*sizeof(int*));
	return theArray;
}
//------------------------------------------
// float 1-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr;
	float *m;
}float1d;

void freefloat1d(float1d myArray)
{
	free(myArray.m);
}

float1d float1d_initialize(int nr)
{
	float1d theArray;
	theArray.nr=nr;
	theArray.m = (float *) malloc(nr*sizeof(float*));
	return theArray;
}


//------------------------------------------
// unsigned char 2-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr,nc;
	unsigned char **m;
}uchar2d;

void freeuchar2d(uchar2d myArray)
{
	for(i=0;i<myArray.nr;i++)
		free(myArray.m[i]);
	free(myArray.m);
}

uchar2d uchar2d_initialize(int nr, int nc) 
{  
	uchar2d theArray;  
	theArray.nr=nr;
	theArray.nc=nc;
	theArray.m = (unsigned char **) malloc(nr*sizeof(unsigned char*));  
	for (i = 0; i < nr; i++)  
   	theArray.m[i] = (unsigned char *) malloc(nc*sizeof(unsigned char));  
   	return theArray;  
}

//------------------------------------------
// unsigned int 2-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr,nc;
	unsigned int **m;
}uint2d;

void freeuint2d(uint2d myArray)
{
	for(i=0;i<myArray.nr;i++)
		free(myArray.m[i]);
	free(myArray.m);
}

uint2d uint2d_initialize(int nr, int nc) 
{  
	uint2d theArray;  
	theArray.nr=nr;
	theArray.nc=nc;
	theArray.m = (unsigned int **) malloc(nr*sizeof(unsigned int*));  
	for (i = 0; i < nr; i++)  
   	theArray.m[i] = (unsigned int *) malloc(nc*sizeof(unsigned int));  
   	return theArray;  
}

//------------------------------------------
// int 2-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr,nc;
	int **m;
}int2d;

void freeint2d(int2d myArray)
{
	for(i=0;i<myArray.nr;i++)
		free(myArray.m[i]);
	free(myArray.m);
}

int2d int2d_initialize(int nr, int nc) 
{  
	int2d theArray;  
	theArray.nr=nr;
	theArray.nc=nc;
	theArray.m = (int **) malloc(nr*sizeof(int*));  
	for (i = 0; i < nr; i++)  
   	theArray.m[i] = (int *) malloc(nc*sizeof(int));  
   	return theArray;  
}

//------------------------------------------
// float 2-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr,nc;
	float **m;
}float2d;

void freefloat2d(float2d myArray)
{
	for(i=0;i<myArray.nr;i++)
		free(myArray.m[i]);
	free(myArray.m);
}

float2d float2d_initialize(int nr, int nc) 
{  
	float2d theArray;  
	theArray.nr=nr;
	theArray.nc=nc;
	theArray.m = (float **) malloc(nr*sizeof(float*));  
	for (i = 0; i < nr; i++)  
   	theArray.m[i] = (float *) malloc(nc*sizeof(float));  
   	return theArray;  
}

//------------------------------------------
// unsigned char 3-dimensional dynamic memory allocation
//------------------------------------------
//nr,nc,depth
typedef struct
{
	int nr,nc,depth;
	unsigned char ***m;
}uchar3d;

uchar3d uchar3d_initialize(int nr,int nc,int depth)
{
	uchar3d theArray;
	theArray.depth=depth;
	theArray.nr=nr;
	theArray.nc=nc;
	theArray.m =(unsigned char ***) malloc(nr*sizeof(unsigned char**)); 
	for (i = 0; i < nr; i++)
	{
		theArray.m[i] = (unsigned char **)malloc( nc*sizeof(unsigned char*)); 
		for(j = 0; j < nc; j++)
		{
			theArray.m[i][j] = (unsigned char *)malloc(depth*sizeof(unsigned char)); 
		}
	}
	return theArray;
}

void freeuchar3d(uchar3d myArray)
{
	for (i = 0; i < myArray.nr; i++)
	{
		free(myArray.m[i]);
		for(j = 0; j < myArray.nc; j++)
		{
			free(myArray.m[i][j]); 
		}
	}
	free(myArray.m);
}

//------------------------------------------
// unsigned int 3-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr,nc,depth;
	unsigned int ***m;
}uint3d;

uint3d uint3d_initialize(int nr,int nc,int depth)
{
	uint3d theArray;
	theArray.depth=depth;
	theArray.nr=nr;
	theArray.nc=nc;
	theArray.m =(unsigned int ***) malloc(nr*sizeof(unsigned int**)); 
	for (i = 0; i < nr; i++)
	{
		theArray.m[i] = (unsigned int **)malloc( nc*sizeof(unsigned int*)); 
		for(j = 0; j < nc; j++)
		{
			theArray.m[i][j] = (unsigned int *)malloc(depth*sizeof(unsigned int)); 
		}
	}
	return theArray;
}

void freeuint3d(uint3d myArray)
{
	for (i = 0; i < myArray.nr; i++)
	{
		free(myArray.m[i]);
		for(j = 0; j < myArray.nc; j++)
		{
			free(myArray.m[i][j]); 
		}
	}
	free(myArray.m);
}

//------------------------------------------
// int 3-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr,nc,depth;
	int ***m;
}int3d;

int3d int3d_initialize(int nr,int nc,int depth)
{
	int3d theArray;
	theArray.depth=depth;
	theArray.nr=nr;
	theArray.nc=nc;
	theArray.m =(int ***) malloc(nr*sizeof(int**)); 
	for (i = 0; i < nr; i++)
	{
		theArray.m[i] = (int **)malloc( nc*sizeof(int*)); 
		for(j = 0; j < nc; j++)
		{
			theArray.m[i][j] = (int *)malloc(depth*sizeof(int)); 
		}
	}
	return theArray;
}

void freeint3d(int3d myArray)
{
	for (i = 0; i < myArray.nr; i++)
	{
		free(myArray.m[i]);
		for(j = 0; j < myArray.nc; j++)
		{
			free(myArray.m[i][j]); 
		}
	}
	free(myArray.m);
}

//------------------------------------------
// float 3-dimensional dynamic memory allocation
//------------------------------------------
typedef struct
{
	int nr,nc,depth;
	float ***m;
}float3d;

float3d float3d_initialize(int nr,int nc,int depth)
{
	float3d theArray;
	theArray.depth=depth;
	theArray.nr=nr;
	theArray.nc=nc;
	theArray.m =(float ***) malloc(nr*sizeof(float**)); 
	for (i = 0; i < nr; i++)
	{
		theArray.m[i] = (float **)malloc( nc*sizeof(float*)); 
		for(j = 0; j < nc; j++)
		{
			theArray.m[i][j] = (float *)malloc(depth*sizeof(float)); 
		}
	}
	return theArray;
}

void freefloat3d(float3d myArray)
{
	for (i = 0; i < myArray.nr; i++)
	{
		free(myArray.m[i]);
		for(j = 0; j < myArray.nc; j++)
		{
			free(myArray.m[i][j]); 
		}
	}
	free(myArray.m);
}
