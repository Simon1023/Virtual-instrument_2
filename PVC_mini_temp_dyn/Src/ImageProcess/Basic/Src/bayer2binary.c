
#include "..\bayer2binary.h"

#define BINARY_THRESHOLD    128


void bayer2binary(uc1D *pSrc, uc1D *pDst, unsigned char thrR, unsigned char thrG, unsigned char thrB)
{
	int i, j;
	unsigned char r, g, b;
	int index;
    unsigned nr = pSrc->nr, nc = pSrc->nc;

    //test
    /*
    for(index=0;index<nc*nr;index++)
     pDst->m[index] = pSrc->m[index];
    */
        
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

            if(abs(r-thrR) > BINARY_THRESHOLD)
                pDst->m[index] = 0;
            else if(abs(g-thrG) > BINARY_THRESHOLD)
                pDst->m[index] = 0;
            else if(abs(b-thrB) > BINARY_THRESHOLD)
                pDst->m[index] = 0;
            else
                pDst->m[index] = 255;                
        }
	}
}