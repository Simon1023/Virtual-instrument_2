#include "..\FreiChennFilter.h"

void FreiChennFilter(uc1D *ImaSrc, uc1D *ImaDst)
{
	unsigned char *pout = ImaDst->m;
	/*---------------frei chenn mask---------------*/
	float gx[3][3]={{-1,  -1.414,    -1},
									{ 0,       0,     0},
									{ 1,   1.414,     1}};
	float gy[3][3]={{-1,       0,     1},
									{-1.414,   0, 1.414},
									{-1,       0,     1}};
	/*---------------reset---------------*/
	memset(pout,0,ImaSrc->nr * ImaSrc->nc);
	
	/*---------------¾B¸n¹Bºâ---------------*/

	int gx_value,gy_value;
	pout = ImaDst->m + ImaSrc->nc + 1;
	for(int i=1;i<ImaSrc->nr-1;i++)
	{
		for(int j=1;j<ImaSrc->nc-1;j++)
		{
			gx_value = gy_value = 0;
			for(int ii=-1;ii<=1;ii++)
				for(int jj=-1;jj<=1;jj++)
				{
					gx_value = gx_value + ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)]*gx[ii+1][jj+1];
					gy_value = gy_value + ImaSrc->m[(i+ii)*ImaSrc->nc+(j+jj)]*gy[ii+1][jj+1];
				}
			int mac=sqrt((gx_value*gx_value)+(gy_value*gy_value));

			if(mac>255) *pout = 255;
			else *pout=mac;
			pout++;
		}
		pout += 2;
	}
}
