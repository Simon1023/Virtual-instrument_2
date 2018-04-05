#include <stdio.h>
#include <math.h>
#include "pnn_data.h"
#include "pnn_array.h"


typedef unsigned char bool;
#define false 0
#define true 1

//#define TEST_NUM 18

//enum FileForm{Data,Weight};
//----------------------------------------------------------

static int nb_in,nb_out,nb_hidden;
static float2d W_xh,W_hy;
static float sigma;
static bool  IsSet=false,IsLoad=false;
static float1d input,output;

static float* array1[PNN_FEATURE_NUM]={0};
static float  array2[PNN_FEATURE_NUM][PNN_DATA_NUM]={0};
static float* array3[PNN_DATA_NUM]={0};
static float  array4[PNN_DATA_NUM][PNN_CLASSFICAT_NUM]={0};

extern void segmentGetRatio(unsigned char* img,int nc,unsigned char charIndex, float* array);

static void PNN_SetSmoothParameter(float p)
{
	sigma=p;
	IsSet=true;
}

static int PNN_Using(float1d *input,float1d *output)
{
	int i,j;
	int winner=0;
	float neth,netj;
	float1d Hidden;
	Hidden=float1d_initialize(nb_hidden);
	if(IsSet && IsLoad && (input->nr==nb_in))
	{

		for(i=0;i<nb_hidden;i++)
		{
			neth=0;
			for(j=0;j<nb_in;j++)
			{
				neth+=(input->m[j]-W_xh.m[j][i])*(input->m[j]-W_xh.m[j][i]);
			}
			Hidden.m[i]=exp(-neth/(2*sigma*sigma));
		}
		float N;
		float temp;
		winner=0;
		for(i=0;i<nb_out;i++)
		{
			N=0;
			for(j=0;j<nb_hidden;j++)
			{
				N+=W_hy.m[j][i];
			}
			netj=0;
			for(j=0;j<nb_hidden;j++)
			{
				netj+=W_hy.m[j][i]*Hidden.m[j];
			}
			if(N!=0)
			{
				netj/=N;
			}
			if(i==0)
			{
				temp=netj;
				output->m[0]=1;
			}
			else
			{
				if(netj>=temp)
				{
					temp=netj;
					output->m[winner]=0;
					output->m[i]=1;
					winner=i;
				}
				else
				{
					output->m[i]=0;
				}
			}
		}
	}
	freefloat1d(Hidden);
	
	return winner;
}

void PNN_Initialize()
{
	int i,j;

	nb_in = PNN_FEATURE_NUM; 
	nb_out = PNN_CLASSFICAT_NUM; 
	nb_hidden = PNN_DATA_NUM;
	
	//W_xh=float2d_initialize(nb_in,nb_hidden);
	W_xh.nr=nb_in;
	W_xh.nc=nb_hidden;
	W_xh.m = (float **) array1;  
	for (i = 0; i < W_xh.nr; i++)  
   		W_xh.m[i] = (float *) &array2[i];  
	
	//W_hy=float2d_initialize(nb_hidden,nb_out);
	W_hy.nr=nb_hidden;
	W_hy.nc=nb_out;
	W_hy.m = (float **) array3;
	for (i = 0; i < W_hy.nr; i++)  
   		W_hy.m[i] = (float *) &array4[i];
    

	for(i=0;i<nb_hidden;i++)
	{
		for(j=0;j<nb_in;j++)
		{
			W_xh.m[j][i]=pnnDataArray[i][j];
		}
		for(j=0;j<nb_out;j++)
		{			
			W_hy.m[i][j]=(pnnDataArray[i][PNN_PARAMETER_NUM-1]==j)?1:0;			
		}
	}
	
	IsLoad=true;
	
	input=float1d_initialize(PNN_FEATURE_NUM);
   	output=float1d_initialize(PNN_CLASSFICAT_NUM);
   	
   	PNN_SetSmoothParameter(5);
}

int PNN_Calculate(unsigned char* img,int nc,unsigned char index)
{   
    int result;
    
    
    segmentGetRatio(img,nc,index,input.m);
    //test
    /*
    input.m[0]=0.4;
    input.m[1]=0.2;
    input.m[2]=0.266667;
    input.m[3]=0;
    input.m[4]=0.666667;
    input.m[5]=0;
    input.m[6]=0.2;
    input.m[7]=0.266667;
    input.m[8]=0.666667;
    */
    
    result = PNN_Using(&input,&output);

    return result;
}

#if 0
int main()
{
   int nb_in,nb_out,nb;
   int i,j;
   int result;
   float1d input,output;

	float testArray[TEST_NUM][PNN_PARAMETER_NUM-1] =
	{
		{0.27,0.13,0.07,0,0.33,0.13,0.47,0.13,0.07},//0
		{0.2,0,0,0,0,0,0.2,0,0},//1
		{0.27,0.13,0.2,1,0.33,0.27,0.27,0,0.2},//2
		{0.33,0.13,0.4,0.87,0.13,0.33,0.4,0.2,0.47},//3
		{0.92,0.08,0.33,0.08,0,0.25,0.67,0.17,0.17},//4
		{0.33,0.17,0.75,0.58,0.08,0.25,0.75,0.58,0.08},//5
		{0.75,0.08,0.75,0.08,0.17,0.17,0.17,0.5,0.08},//6
		{0.125,0.25,0.125,1,0.875,0,1,0.25,0},//7
		{0.4,0.27,0.2,0.4,0,0.13,0.2,0.27,0.13},//8
		{0.25,0.08,0.33,0,0.5,0,0.58,0.17,0},//9
		{0.4,0.27,0.2,0.4,0,0.13,0.2,0.27,0.13},//8
		{0.125,0.25,0.125,1,0.875,0,1,0.25,0},//7
		{0.4,0.27,0.2,0.4,0,0.13,0.2,0.27,0.13},//8
		{0.125,0.25,0.125,1,0.875,0,1,0.25,0},//7
		{0.27,0.13,0.07,0,0.33,0.13,0.47,0.13,0.07},//0
		{0.4,0.27,0.2,0.4,0,0.13,0.2,0.27,0.13},//8
		{0.27,0.23,0.2,1,0.33,0.57,0,0,0.2},//2
		{0.33,0.17,0.75,0.58,0.08,0.25,0.75,0.58,0.08},//5
	};


   PNN_Initialize(&input,&output);
   //PNN_SetSmoothParameter(5);

   nb = TEST_NUM;
   //nb = 1;
   	
   //input=float1d_initialize(PNN_FEATURE_NUM);
   //output=float1d_initialize(PNN_CLASSFICAT_NUM);
     
   for(i=0;i<nb;i++)
   	{
   		for(j=0;j<PNN_FEATURE_NUM;j++){
   			input.m[j] = testArray[i][j];
   			
   			//printf("input.m[%d]= %f\n",j,input.m[j]); 
		}
	
       	result = PNN_Using(&input,&output);
       	
       	printf("[%d] result =%d\n",i,result);
       	
       	/*
       	printf("[%d]\n",i);
       	for(j=0;j<nb_out;j++){
   	
       		//printf("output.m[%d]= %f\n",j,output.m[j]);
			   
			if(output.m[j]==1)
				printf("output is %d\n",j); 
		}
		printf("\n");
		*/
   	}
    return 0;

}
#endif