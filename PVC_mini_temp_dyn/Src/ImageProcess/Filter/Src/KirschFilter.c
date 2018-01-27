#include "..\KirschFilter.h"

void KirschFilter(uc1D *ImaSrc, uc1D *ImaDst)
{
	unsigned char *pout = ImaDst->m;
	/*---------------Kirsch mask------------*/
	signed char mask[8][3][3]={{{-3,-3, 5},{-3, 0, 5},{-3,-3, 5}},
														 {{-3, 5, 5},{-3, 0, 5},{-3,-3,-3}},
														 {{ 5, 5, 5},{-3, 0,-3},{-3,-3,-3}},
														 {{ 5, 5,-3},{ 5, 0,-3},{-3,-3,-3}},
														 {{ 5,-3,-3},{ 5, 0,-3},{ 5,-3,-3}},
														 {{-3,-3,-3},{ 5, 0,-3},{ 5, 5,-3}},
														 {{-3,-3,-3},{-3, 0,-3},{ 5, 5, 5}},
														 {{-3,-3,-3},{-3, 0, 5},{-3, 5, 5}}};
	/*---------------reset---------------*/
	memset(pout,0,ImaSrc->nr * ImaSrc->nc);
														 
	/*---------------¾B¸n¹Bºâ---------------*/
	int value=0,max=0;
	pout = ImaDst->m + ImaSrc->nc + 1;
	for(int i=1;i<ImaSrc->nr-1;i++)
	{
		for(int j=1;j<ImaSrc->nc-1;j++)
		{
			max=0;
			for(int z=0;z<8;z++)
			{
				value=0;
				for(int ii=-1;ii<=1;ii++)
				{
					for(int jj=-1;jj<=1;jj++)
					{
						value += ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)]*mask[z][ii+1][jj+1];
					}
				}
				if(value>max)
				{
					max=value;
				}
			}
			if(max>255)
				*pout=255;
			else
				*pout=max;
			pout++;
		}
		pout += 2;
	}
}
