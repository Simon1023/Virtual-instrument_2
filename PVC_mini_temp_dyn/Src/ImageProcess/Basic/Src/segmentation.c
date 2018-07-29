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
	unsigned char *p;
	unsigned char *curRow;
    
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
		
		//if(count>(imageSrc->nc/THRESHOLD))
        if(count>0)
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
		for(y=0;y<projNr;y++)
		{
            curRow = imageSrc->m+(y+hMin-1)*imageSrc->nc;
			//if(*(imageSrc->m+x+y*imageSrc->nc) == 255)
            if(*(curRow+x) == 255)
            { 
                /*
                //if((y==hMin-1) && (*(imageSrc->m+x+(y+1)*imageSrc->nc) == 255))
                if((y==0) && (*(curRow+x+imageSrc->nc) == 255))
                    count++;
                //else if((y==hMax-1) && (*(imageSrc->m+x+(y-1)*imageSrc->nc) == 255))
                else if((y==projNr-1) && (*(curRow+x-imageSrc->nc) == 255))
                    count++;                
                //else if((*(imageSrc->m+x+(y-1)*imageSrc->nc) == 255) || (*(imageSrc->m+x+(y+1)*imageSrc->nc) == 255))
                else if((*(curRow+x-imageSrc->nc) == 255) || (*(curRow+x+imageSrc->nc) == 255))
                    count++;
                */
                count++;
            }
		}
		
		//if( count>(imageSrc->nr/THRESHOLD))
        if( count>0)
		{
			if(startX == NO_ASSIGN)
				startX = x;
			else
				endX = x;
		}
		else
		{
            if(x-startX+1 < projNr/2)
                continue;
            
            //if(*(imageSrc->m+x+(hMin-1)*imageSrc->nc) == *(imageSrc->m+x+(hMax-1)*imageSrc->nc) == 255)
            //    continue;
            
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
				
                //Two digits are connected
                if(gpCharInfo[index].nc>gpCharInfo[index].nr)
                {
                    unsigned char reSegmentCount=0;
                    unsigned char reSegmentNc=0;
                    int i;
                    
                    reSegmentCount = gpCharInfo[index].nc/gpCharInfo[index].nr+1;
                    reSegmentNc = gpCharInfo[index].nc/reSegmentCount;
                    
                    for(i=0;i<reSegmentCount;i++)
                    {                        
                        gpCharInfo[index+i].x = startX+i*reSegmentNc;
                        gpCharInfo[index+i].y = hMin;
                        gpCharInfo[index+i].nc = reSegmentNc;
                        gpCharInfo[index+i].nr = projNr;
                    }
                    
                    index=index+reSegmentCount-1;
                }
                
				index++;
				startX = NO_ASSIGN;					
			}
		}
	}

	gCharCount = index;
	//pChar = gpProjection ;
	
	//Draw the boundry for testing
 #if 0
	for(x=0;x<imageSrc->nc;x++)
    {
        //*(imageSrc->m+hMin*imageSrc->nc+x) = 128;
        p=imageSrc->m+hMin*imageSrc->nc+x;
        if(*p!=255)
            *p=120;
    }
    
	for(x=0;x<imageSrc->nc;x++)
    {
		//*(imageSrc->m+hMax*imageSrc->nc+x) = 128;	
        p=imageSrc->m+hMax*imageSrc->nc+x;
        if(*p!=255)
            *p=120;       
	}
    
	for(index=0;index<gCharCount;index++)
	{
		for(y=0;y<imageSrc->nr;y++)
		{
			//*(gpProjection[index].m + y*imageSrc->nc) = 128;
			//*(gpProjection[index].m + gpProjection[index].nc + y*imageSrc->nc) = 128;
			
            //*(imageSrc->m + gpCharInfo[index].x + y*imageSrc->nc)=128; 
            p= imageSrc->m + gpCharInfo[index].x + y*imageSrc->nc;
            if(*p!=255)
                *p=100;
            
			//*(imageSrc->m + gpCharInfo[index].x + gpCharInfo[index].nc + y*imageSrc->nc)=128; 
            //p= imageSrc->m + gpCharInfo[index].x + gpCharInfo[index].nc + y*imageSrc->nc;
            p+=gpCharInfo[index].nc-1;
            if(*p!=255)
                *p=100;            
		}
	}
 #endif
    
    //20180729 Simon :Write the information of segmentation in image for debugging
    //test -->
    p= imageSrc->m +imageSrc->nc;
    *(p++) = gpCharInfo[0].x;
    *(p++) = gpCharInfo[0].y;
    *(p++) = gpCharInfo[0].nr;
    *(p++) = gpCharInfo[0].nc;

    if(gCharCount>1)
    {
        *(p++) = gpCharInfo[1].x;
        *(p++) = gpCharInfo[1].y;
        *(p++) = gpCharInfo[1].nr;
        *(p++) = gpCharInfo[1].nc;
    }

    if(gCharCount>2)
    {
        *(p++) = gpCharInfo[2].x;
        *(p++) = gpCharInfo[2].y;
        *(p++) = gpCharInfo[2].nr;
        *(p++) = gpCharInfo[2].nc;
    }
    //test <--
    
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