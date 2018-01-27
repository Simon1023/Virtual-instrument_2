#include "..\LocalHE.h"

void LocalHE(uc1D *ImaSrc, uc1D *ImaDst, unsigned char blocksize)
{
	int rank;
	if(blocksize%2==0)
		blocksize+=1;
	unsigned char hsize=blocksize>>1;
	unsigned char area=blocksize*blocksize;
	for(int i=hsize;i<(ImaSrc->nr-hsize);i++)
		for(int j=hsize;j<(ImaSrc->nc-hsize);j++)
		{
			rank=0;
			for(int k=(i-hsize);k<=(i+hsize);k++)
				for(int l=(j-hsize);l<=(j+hsize);l++)
					if(ImaSrc->m[i*ImaSrc->nc+j]>ImaSrc->m[k*ImaSrc->nc+l])
						rank++;
			ImaDst->m[i*ImaSrc->nc+j]=rank*255/area;
		}
}
