#include "amf.h"
#include "def_type.h"
#include "Transmission_mode.h"

#define THRESHOLD 100

#define TRUE 1
#define FALSE 0

static uc1D gBasic = {0};
static int gPixelCount = 0;

extern sd_uchar amfBuf[NUM_OF_PIXELS];

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
	for(int i=0;i <gPixelCount;i++)
	{
		if(abs(pSrc->m[i] - gBasic.m[i]) > THRESHOLD)
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
