/* Threshold, gray image to binary image */
#include "../threshold.h"

void cvThreshold(uc1D *img1, uc1D *outimg, int threshold)
{
	int t, i, j;
	for(i=0;i<img1->nr;i++){
		for(j=0;j<img1->nc;j++){
			t = img1->m[i*img1->nc+j];
			if(t > threshold)
				outimg->m[i*img1->nc+j] = 255;
			else
				outimg->m[i*img1->nc+j] = 0;
		}
	}
}
