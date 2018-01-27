#include "..\ArcelliBaja.h"
#include <string.h>
unsigned char data[240][320];
void ArcelliBaja(uc1D *ImaSrc, uc1D *ImaDst)
{
	/*-----------------four connectivity----------------*/
	unsigned char wn[2]={0};
	unsigned char es[3]={0};
	unsigned char *pin = ImaSrc->m;
	unsigned char *pout = ImaDst->m;
	memset(data,0,ImaSrc->nr*ImaSrc->nc);
	//Initialize the image pixels
	for(int i=0; i<ImaSrc->nr*ImaSrc->nc;i++)
	{
		if(*pin > 100)
			*pin = 0;
		else
			*pin = 1;
		
		pin++;
	}
	/*------------Forward pass------------*/
	for(int i=1;i<ImaSrc->nr-1;i++)
	{
		for(int j=1;j<ImaSrc->nc-1;j++)
		{
			if(ImaSrc->m[i*ImaSrc->nc+j]==1)
			{
				wn[0]=ImaSrc->m[i*ImaSrc->nc+(j-1)];
				wn[1]=ImaSrc->m[(i-1)*ImaSrc->nc+(j)];
				if(wn[0]<wn[1])
					ImaSrc->m[i*ImaSrc->nc+j]=wn[0]+1;
				else
					ImaSrc->m[i*ImaSrc->nc+j]=wn[1]+1;
			}
		}
	}
	/*------------Backward  pass------------*/
	for(int i=ImaSrc->nr-1;i>1;i--)
	{
		for(int j=ImaSrc->nc-1;j>1;j--)
		{
			if(ImaSrc->m[i*ImaSrc->nc+j]>=1)
			{
				es[0]=ImaSrc->m[i*ImaSrc->nc+j];
				es[1]=ImaSrc->m[i*ImaSrc->nc+(j+1)]+1;
				es[2]=ImaSrc->m[(i+1)*ImaSrc->nc+j]+1;
				unsigned char min=ImaSrc->m[i*ImaSrc->nc+j];
				for(int z=0;z<3;z++)
				{
					if(es[z]<min)
						min=es[z];
				}
				ImaSrc->m[i*ImaSrc->nc+j]=min;
			}
		}
	}
	/*--------------------------------------------------*/
	
	pin = ImaSrc->m;
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++){
			data[i][j]=*pin;
			pin++;
		}

	int stop=1;
	unsigned char d=1;
	while(stop)
	{
		stop=0;
		for(int i=0;i<ImaSrc->nr;i++)
		{
			for(int j=0;j<ImaSrc->nc;j++)
			{
				if(data[i][j]==d)
					data[i][j]='C';	//c
				else if(data[i][j]==0)
					data[i][j]='A'; //A'
				else if(data[i][j]=='M')
					data[i][j]='M';
				else{
					data[i][j]='I';
					stop=1;
				}
			}
		}

		d++;
		for(int i=1;i<ImaSrc->nr-1;i++)
		{
			for(int j=1;j<ImaSrc->nc-1;j++)
			{
				if(data[i][j]=='C')
				{
					/*------------multiple C 條件-----------*/
					if(!((data[i-1][j]=='I' && data[i+1][j]=='A' ) || (data[i-1][j]=='A' && data[i+1][j]=='I') || (data[i][j-1]=='I' && data[i][j+1]=='A') || (data[i][j-1]=='A' && data[i][j+1]=='I')))
						data[i][j]='M';
					else if( data[i-1][j+1]=='C' && data[i-1][j]=='A' && data[i][j+1]=='A' )
						data[i][j]='M';
					else if( data[i+1][j+1]=='C' && data[i][j+1]=='A' && data[i+1][j]=='A')
						data[i][j]='M';
					else if( data[i+1][j-1]=='C' && data[i+1][j]=='A' && data[i][j-1]=='A')
						data[i][j]='M';
					else if( data[i-1][j-1]=='C' && data[i][j-1]=='A' && data[i-1][j]=='A')
						data[i][j]='M';
				}

			}
		}
		pin = ImaSrc->m;
		for(int i=0;i<ImaSrc->nr;i++){
			for(int j=0;j<ImaSrc->nc;j++){
				if(data[i][j]=='C') data[i][j]=0;
				if(data[i][j]=='A') data[i][j]=0;
				if(data[i][j]=='I') data[i][j]=*pin;
				
				pin++;
			}
		}
	}
	/*------------輸出影像------------*/
	pout = ImaDst->m;
	for(int i=0;i<ImaSrc->nr;i++)
	{
		for(int j=0;j<ImaSrc->nc;j++)
		{
			if(data[i][j]=='M')
				*pout=0;
			else 
				*pout=255;
			
			pout++;
		}
	}
}
