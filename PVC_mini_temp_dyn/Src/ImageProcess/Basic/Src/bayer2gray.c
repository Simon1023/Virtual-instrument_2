
#include "..\bayer2gray.h"

void bayer2gray(uc1D *pSrc, uc1D *pDst)
{
	int i, j;
	unsigned char r, g, b;
	int index;
    unsigned nr = pSrc->nr, nc = pSrc->nc;
        
	for (i = 1; i<nr - 1; i++)
	{
		for (j = 1; j<nc - 1; j++)
		{
			index = (i * nc + j);
			if (i % 2 == 1 && j % 2 == 0)	//G
			{
				r = (pSrc->m[index - 1] + pSrc->m[index + 1]) / 2;
				g = pSrc->m[index];
				b = (pSrc->m[index - nc] + pSrc->m[index + nc]) / 2;

			}
			else if (i % 2 == 1 && j % 2 == 1)	//R
			{
				r = pSrc->m[index];
				g = (pSrc->m[index - nc] + pSrc->m[index + nc] + pSrc->m[index - 1] + pSrc->m[index + 1]) / 4;
				b = (pSrc->m[index - nc - 1] + pSrc->m[index - nc + 1] + pSrc->m[index + nc - 1] + 
                    pSrc->m[index + nc + 1]) / 4;
			}
			else if (i % 2 == 0 && j % 2 == 0)	//B
			{
				r = (pSrc->m[index - nc - 1] + pSrc->m[index - nc + 1] + pSrc->m[index + nc - 1] + pSrc->m[index + nc + 1]) / 4;
				g = (pSrc->m[index - nc] + pSrc->m[index + nc] + pSrc->m[index - 1] + pSrc->m[index + 1]) / 4;
				b = pSrc->m[index];
			}
			else if (i % 2 == 0 && j % 2 == 1)	//G
			{
				r = (pSrc->m[index - 1] + pSrc->m[index + 1]) / 2;
				g = pSrc->m[index];
				b = (pSrc->m[index - nc] + pSrc->m[index + nc]) / 2;

			}
      
            pDst->m[index] = (r*299 + g*587 + b*114 + 500)/1000;
            
        }
	}
}