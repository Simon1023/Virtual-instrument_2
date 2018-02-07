#include "..\projection.h"

uc1D *gpProjextion = NULL;
unsigned char gProjCount =0 ;

int getProjection(uc1D *imageSrc)
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
	unsigned char charCount = 8;
	
	if(gpProjextion == NULL)
		gpProjextion = (uc1D *)calloc(charCount,sizeof(uc1D));
	
	if(gpProjextion == NULL)
		return 1;
	
	//horizontal
	for(y=0,count=0;y<imageSrc->nr;y++,count=0)
	{
		for(x=0;x<imageSrc->nc;x++)
		{
			if(*(imageSrc->m+x+y*imageSrc->nc) == 255)
				count++;
		}
		
		if(count>(imageSrc->nc/20))
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
		
		if( count>(imageSrc->nr/20))
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
				gpProjextion[index].nc = endX-startX;
				gpProjextion[index].nr = projNr;
				gpProjextion[index].m = imageSrc->m+startX+hMin*imageSrc->nc;

				index++;
				startX = NO_ASSIGN;					
			}
		}
	}

	gProjCount = index;
	
	
	//test
	for(x=0;x<imageSrc->nc;x++)
		*(imageSrc->m+hMin*imageSrc->nc+x) = 128;

	for(x=0;x<imageSrc->nc;x++)
		*(imageSrc->m+hMax*imageSrc->nc+x) = 128;	
	
	for(index=0;index<gProjCount;index++)
	{
		for(y=0;y<imageSrc->nr;y++)
		{
			*(gpProjextion[index].m + y*imageSrc->nc) = 128;
			*(gpProjextion[index].m + gpProjextion[index].nc + y*imageSrc->nc) = 128;
		}
	}
	
	return 0;
}