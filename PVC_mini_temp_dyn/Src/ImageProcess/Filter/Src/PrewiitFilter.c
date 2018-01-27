#include "..\PrewiitFilter.h"

void PrewiitFilter(uc1D *ImaSrc, uc1D *ImaDst)
{
	unsigned char *pout = ImaDst->m;
	/*---------------prewiit mask---------------*/
	signed char gx[3][3]={{-1,-1,-1},
												{ 0, 0, 0},
												{ 1, 1, 1}};
	signed char gy[3][3]={{-1, 2, 1},
												{-1, 0, 0},
												{-1, 0, 1}};
	/*---------------reset---------------*/
	memset(pout,0,ImaSrc->nr * ImaSrc->nc);
												
	/*---------------¾B¸n¹Bºâ---------------*/
	int i,j,ii,jj;
	//int N=3/2;
	int gx_value=0,gy_value=0;
	pout = ImaDst->m;
	for(i=0;i<ImaSrc->nr;i++)for(j=0;j<ImaSrc->nc;j++)
	{
		if(i ==0 || i == ImaSrc->nr-1 || j == 0 || j == ImaSrc->nc-1)
		{
			*pout = ImaSrc->m[i*ImaSrc->nc+j];
			pout++;
		}
		else
		{
			gx_value = gy_value = 0;
			for(ii=-1;ii<=1;ii++)for(jj=-1;jj<=1;jj++)
			{
				gx_value = gx_value + ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)]*gx[ii+1][jj+1];
				gy_value = gy_value + ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)]*gy[ii+1][jj+1];
			}
			int mac=sqrt((gx_value*gx_value)+(gy_value*gy_value));

			if(mac>255) *pout = 255;	
			else *pout=mac;
			pout++;
		}
	}
}
