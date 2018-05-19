#include <math.h>
#include "def_type.h"
#include "Transmission_mode.h"

#define MAX_ANGLE 180
#define FEATURE_POINT 255
#define BACKGROUND 0
#define R_MAX 400		
#define ANGLE_0 45 
#define ANGLE_100 315

#define HOUGH_DEBUG 0

typedef struct Array2D
{
	int nr;
	int nc;
	unsigned char **m;
}uc2D;

typedef struct {
		int nr;
		int nc;
		unsigned char *m;           /* Pixel values */
} uc1D;

typedef struct _MAX{
	int nr;
	int angle;
	float r;
	unsigned char count;
}MAX; 

static float rmax=0;
static MAX maxPoint={0};
static float sinTable[MAX_ANGLE],cosTable[MAX_ANGLE];
static float scale = (ANGLE_100-ANGLE_0)/(float)100;

//unsigned char buffer[2*R_MAX*MAX_ANGLE] = {0};
extern sd_uchar tempBuf[144000]; //2*R_MAX*MAX_ANGLE
unsigned char* pBufPoint[2*R_MAX] ={0};

static unsigned int featurePointLeft=0;
static unsigned int featurePointRight=0;

#if HOUGH_DEBUG
    static unsigned int featurePointCount=0;
#endif

static void tableInit()
{
	int angle;
    float theta;
    static char isInit = 0;
    
    if(isInit==1)
    	return;
    	
    for(angle=0;angle<MAX_ANGLE;angle++)
    {
        theta=angle*3.14159265358979/MAX_ANGLE;
        cosTable[angle]=cos(theta);
        sinTable[angle]=sin(theta);
   	}
	isInit =1;
	
}

static void transform(uc1D* im, uc2D* hough_ima)
{
	int i,j;
   	//float rmax=sqrt((float)im->nr*(float)im->nr+(float)im->nc*(float)im->nc);
   	
	//initialize(hough_ima,2*rmax,MAX_ANGLE);
		    
   	float r;
   	int nr,angle;
    int middle = im->nc/2;
    
    maxPoint.angle = maxPoint.count = maxPoint.nr = maxPoint.r = 0;
    
   	for(i=0;i<im->nr;i++)
	{
   		for(j=0;j<im->nc;j++)
   		{	
   	 		if(im->m[i*im->nc+j]==FEATURE_POINT)
     		{	
           		for(angle=0;angle<MAX_ANGLE;angle++)           // for each theta
           		{
            		r=i*sinTable[angle]+j*cosTable[angle];// compute r
            		nr=(int)(r+rmax);                     //shift r to positive value
            		hough_ima->m[nr][angle]++;           // accumulation
	            	
            		if(hough_ima->m[nr][angle] > maxPoint.count)
            		{
            			maxPoint.nr = nr;
            			maxPoint.angle = angle;
            			maxPoint.count = hough_ima->m[nr][angle];
            			maxPoint.r = r;
					}
           		}
                #if HOUGH_DEBUG
                    featurePointCount++;
                #endif
                
                if(j<middle)
                    featurePointLeft++;
                else
                    featurePointRight++;
                    
	  		}
   		}
   	}
}

int houghLineDetect(uc1D *pSrc)
{		
	int i,j,value;
	uc2D hough={0};
    
    #if HOUGH_DEBUG  
        featurePointCount=0;
    #endif
    
    featurePointLeft = featurePointRight=0;
    
    rmax=sqrt((float)pSrc->nr*(float)pSrc->nr+(float)pSrc->nc*(float)pSrc->nc);
    
    
   	hough.nr = 2*rmax;
   	hough.nc = MAX_ANGLE;
   	hough.m = pBufPoint;
   	for(i=0;i<2*rmax;i++)
   	{
   		pBufPoint[i] = &tempBuf[i*MAX_ANGLE];
	}
   	
   	for(i=0;i<hough.nr;i++)
   	{
		for(j=0;j<hough.nc;j++)
	 		hough.m[i][j]=0;
	}
	
    tableInit();
	transform(pSrc, &hough);	

    if(featurePointRight>featurePointLeft)
        maxPoint.angle+=MAX_ANGLE;
        
	value = (int)((maxPoint.angle-ANGLE_0)/scale+0.5);
    
    #if HOUGH_DEBUG    
        unsigned int count=0;
        for(i=0;i<hough.nr;i++)
        {
            for(j=0;j<hough.nc;j++)
                if(hough.m[i][j]==maxPoint.count)
                    count++;
        }
    #endif
    
    return value;
}

