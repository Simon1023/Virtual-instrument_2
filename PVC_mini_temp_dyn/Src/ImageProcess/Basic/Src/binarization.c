#include "..\binarization.h"

void binarization(uc1D *ImaSrc, uc1D *ImaDst, int threshold)
{

	for(int i = 0; i < ImaSrc->nr * ImaSrc->nc; i++)
	{
		if(ImaSrc->m[i]<threshold)
			ImaDst->m[i]=(unsigned char)0;
		else
			ImaDst->m[i]=(unsigned char)255;
	}
	
}
