#include "..\WeightedMedianFilter.h"

void WeightedMedianFilter(uc1D *ImaSrc, uc1D *ImaDst, int WinSize)
{
	int n,Area=0;
	unsigned char Buf;
	n=(WinSize-1)>>1;
	unsigned char *pout = ImaDst->m;

	/*----------count Aera size---------------*/
	unsigned char w[3][3]={	{1,2,3},
													{2,3,2},
													{3,2,1}};
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			Area+=w[i][j];
													
	uc1D Lst=uc1D_Initial(1,Area);
	memset(pout,0,ImaSrc->nr*ImaSrc->nc);
	pout = ImaDst->m + ImaSrc->nc + n;
	for(int r=n;r<ImaSrc->nr-n;r++)
	{
		for(int c=n;c<ImaSrc->nc-n;c++)
		{
			int i=0;
			for(int y=-n;y<=n;y++)
			{
				for(int x=-n;x<=n;x++)
				{
					/*----------根據權重次數---------- */
					for(int z=0;z<w[n+y][n+x];z++)
					{
						Lst.m[i]=ImaSrc->m[(r+y)*ImaSrc->nc+(c+x)];
						i++;
					}
				}
				/*---------- bubble sort -------------------*/
				for(int i=0;i<Area-1;i++)
				{
					for(int j=Area-1;i<j;j--)
					{
						if(Lst.m[j-1]>Lst.m[j])
						{
							Buf=Lst.m[j-1];
							Lst.m[j-1]=Lst.m[j];
							Lst.m[j]=Buf;
						}
						*pout=Lst.m[Area>>1];  // get madian value
					}
				}
			}
			pout++;
		}
		pout += 2*n;
	}
}
