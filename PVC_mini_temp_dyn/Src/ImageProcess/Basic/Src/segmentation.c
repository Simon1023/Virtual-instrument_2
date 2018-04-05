#include "..\segmentation.h"

#define MAX_CHAR 8
#define THRESHOLD 10

CHAR_INFO *gpCharInfo = NULL;
unsigned char gCharCount =0 ;

int segment(uc1D *imageSrc)
{
	int x=0;
	int y=0;			
	int count=0;
	int startX=0;
	int endX=0;
	unsigned char hMin = 0;
	unsigned char hMax = 0;
	int projNr = 0;
	unsigned char index=0;
	
	
	if(gpCharInfo == NULL)
		gpCharInfo = (CHAR_INFO *)calloc(MAX_CHAR,sizeof(CHAR_INFO));
	
	if(gpCharInfo == NULL)
		return 1;
	
	//horizontal
	for(y=0,count=0;y<imageSrc->nr;y++,count=0)
	{
		for(x=0;x<imageSrc->nc;x++)
		{
			if(*(imageSrc->m+x+y*imageSrc->nc) == 255)
				count++;
		}
		
		if(count>(imageSrc->nc/THRESHOLD))
		{
			if(hMin==0)
				hMin = y;
			else
				hMax = y;
		}			
	}	
	projNr = hMax - hMin +1;

	//vertical
	for(x=0,count=0,startX=NO_ASSIGN;x<imageSrc->nc;x++,count=0)
	{
		for(y=0;y<imageSrc->nr;y++)
		{
			if(*(imageSrc->m+x+y*imageSrc->nc) == 255)
				count++;			
		}
		
		if( count>(imageSrc->nr/THRESHOLD))
		{
			if(startX == NO_ASSIGN)
				startX = x;
			else
				endX = x;
		}
		else
		{
			if(startX != NO_ASSIGN)
			{
				/*
				gpProjection[index].nc = endX-startX;
				gpProjection[index].nr = projNr;
				gpProjection[index].m = imageSrc->m+startX+hMin*imageSrc->nc;
				*/
				
				gpCharInfo[index].x = startX;
				gpCharInfo[index].y = hMin;
				gpCharInfo[index].nc = endX-startX+1;
				gpCharInfo[index].nr = projNr;
				
				index++;
				startX = NO_ASSIGN;					
			}
		}
	}

	gCharCount = index;
	//pChar = gpProjection ;
	
	//Draw the boundry for testing
    /*
	for(x=0;x<imageSrc->nc;x++)
		*(imageSrc->m+hMin*imageSrc->nc+x) = 128;

	for(x=0;x<imageSrc->nc;x++)
		*(imageSrc->m+hMax*imageSrc->nc+x) = 128;	
	
	for(index=0;index<gCharCount;index++)
	{
		for(y=0;y<imageSrc->nr;y++)
		{
			//*(gpProjection[index].m + y*imageSrc->nc) = 128;
			//*(gpProjection[index].m + gpProjection[index].nc + y*imageSrc->nc) = 128;
			*(imageSrc->m + gpCharInfo[index].x + y*imageSrc->nc)=128; 
			*(imageSrc->m + gpCharInfo[index].x + gpCharInfo[index].nc + y*imageSrc->nc)=128; 
		}
	}
	*/
    
	return 0;
}

unsigned char segmentGetCount()
{
    return gCharCount;
}

CHAR_INFO * segmentGetInfo()
{
    return gpCharInfo;
}

void segmentGetRatio(unsigned char* srcImg,int nc,unsigned char charIndex, float* array)
{
    char blockCount =9;
    char blockNr, blockNc;
    CHAR_INFO *pCharInfo = gpCharInfo+charIndex;
    int blockIndex;
    
    blockNr = pCharInfo->nr/3;
    blockNc = pCharInfo->nc/3;
    
    for (blockIndex = 0; blockIndex < blockCount; blockIndex++)
    {
        int blockStart = pCharInfo->y*nc + pCharInfo->x;
        int blackCount = 0;
        float blackRatio;

        if (blockIndex < 3)
        {
            blockStart += blockIndex*blockNc;
        }
        else if (blockIndex < 6)
        {
            blockStart += (blockIndex-3)*blockNc + blockNr*nc;
        }
        else if (blockIndex < 9)
        {
            blockStart += (blockIndex-6)*blockNc + blockNr*nc*2;
        }

        for (int j = 0; j < blockNr; j++)
        {
            for (int k = 0; k < blockNc; k++)
            {
                if (srcImg[blockStart+j*nc+k] == 0)
                    blackCount++;
            }
        }

        blackRatio = (float)blackCount/ (float)(blockNr*blockNc);
        
        array[blockIndex] = blackRatio;
    }
      
}