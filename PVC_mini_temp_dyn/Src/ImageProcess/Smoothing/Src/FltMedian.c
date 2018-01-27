#include "../FltMedian.h"
#include <string.h>

void smMedian(uc1D *InIm, uc1D *OutIm, int WinSize)
{
  int   r,c, y,x, i,j, n, Area;
  unsigned char  Buf;
	uc1D Lst=uc1D_Initial(InIm->nr,InIm->nc);
//   unsigned char  *Lst;
//	 int nr=InIm->nr, nc=InIm->nc;
	
  n = (WinSize-1) >> 1;
  Area = (2*n+1) * (2*n+1);
//   Lst = (unsigned char*)malloc(Area*sizeof(unsigned char));
     
//  for (r=0; r<InIm->nr; r++)
//		for (c=0; c<InIm->nc; c++)
//			OutIm->m[r*InIm->nc+c] = 0;
	memset(OutIm->m,0,InIm->nr*InIm->nc);
  for (r=n; r<InIm->nr-n; r++)
  {
		for (c=n; c<InIm->nc-n; c++)
		{
			i=0;
			for (y=-n; y<=n; y++)for (x=-n; x<=n; x++)
			{
				Lst.m[i] = InIm->m[(r+y)*InIm->nc+(c+x)];
				i++;
			}
/*---------- bubble sort -------------------*/
			for (i=0; i<Area-1; i++)for (j=Area-1; i<j; j--)
				if (Lst.m[j-1] > Lst.m[j])
				{
					Buf      = Lst.m[j-1];
					Lst.m[j-1] = Lst.m[j];
					Lst.m[j]   = Buf;
				}
			OutIm->m[r*InIm->nc+c] = Lst.m[Area/2];  // get madian value
		}
	}
}
