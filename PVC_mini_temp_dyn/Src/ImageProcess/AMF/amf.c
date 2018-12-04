#include "amf.h"
#include "def_type.h"
#include "Transmission_mode.h"

#define THRESHOLD_DIGIT 150
#define THRESHOLD_WAVE 90
#define FEATURE_POINT 255

#define TRUE 1
#define FALSE 0

static uc1D gBasic = {0};
static int gPixelCount = 0;

extern sd_uchar amfBuf[NUM_OF_PIXELS];
extern ROI gRoi;

static void updateBg(uc1D *pSrc)
{
	for(int i=0;i<gPixelCount;i++)
	{
		if(pSrc->m[i] > gBasic.m[i])
			gBasic.m[i]++;
		
		if(pSrc->m[i] < gBasic.m[i])
            gBasic.m[i]--;
	}
}

static void markFg(uc1D *pSrc)
{
    unsigned char threshold;
    
    if(gRoi.type == ROI_TYPE_WAVE)
        threshold = THRESHOLD_WAVE;
    else
        threshold = THRESHOLD_DIGIT;
    
	for(int i=0;i <gPixelCount;i++)
	{
		if(abs(pSrc->m[i] - gBasic.m[i]) > threshold)
            pSrc->m[i] = 255;
		else
            pSrc->m[i] = 0;
	}
}

int doAmf(uc1D *pSrc)
{    
    if(gBasic.nr==0)
    {
        /*
        gBasic = uc1D_Initial(pSrc->nr, pSrc->nc);
        
        if(gBasic.nr!=0)
        {
            gPixelCount = gBasic.nc*gBasic.nr;
            
            for(int i=0;i<gPixelCount;i++)
                gBasic.m[i] = pSrc->m[i];
                    
            return AMF_RESULT_INIT;
        }
        else
            return AMF_RESULT_ERROR;
        */
        
        gBasic.nc=pSrc->nc;
        gBasic.nr=pSrc->nr;
        gBasic.m=amfBuf;
        gPixelCount = gBasic.nc*gBasic.nr;
        
        for(int i=0;i<gPixelCount;i++)
            gBasic.m[i] = pSrc->m[i];
            
        return AMF_RESULT_INIT;
    }
    else
    {
        updateBg(pSrc);
        markFg(pSrc);
        
        return AMF_RESULT_OK;
    }
}

void resetAmf()
{
    //uc1D_Free(gBasic);
    gBasic.nr=gBasic.nc=0;
    gPixelCount=0;
}

int getWaveValue(uc1D *pImg)
{
    int i,j;
    int x=0,y=0;
    int value;
    char get=0;
    
    for (j = pImg->nc-1;get==0; j--)
    {
        for (i = 0; i<pImg->nr; i++)
        {
            if(pImg->m[i*pImg->nc+j] == FEATURE_POINT && pImg->m[(i+1)*pImg->nc+j] == FEATURE_POINT)
            {
                x=pImg->nc-1-j;
                y=pImg->nr-i+1;
                
                get =1;
                break;
            }
        }
        
        if(j==0)
            break;
    }
    
/*    
    for (i = 0; i<pImg->nr; i++)
	{
		for (j = 0; j<pImg->nc; j++)
		{
            if(pImg->m[i*pImg->nc+j]==FEATURE_POINT)
            {
                //test
                if((i<pImg->nr-1)&&(pImg->m[(i+1)*pImg->nc+j]!=FEATURE_POINT))
                    continue;
                
                if((j>x) || (j==x && (pImg->nr-i)>y))
                {
                    x=j;
                    y=pImg->nr-i;
                }
            }
        }
    }
*/

    value = (int)(((float)y/(float)(pImg->nr))*100)+0.5;
    
    return value;
}