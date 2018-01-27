/* threshold MIAT fast method */

#include "../threFast.h"

//size = 9
void threFast(uc1D *ima1, uc1D *ima2, int size)
{
	int i,j,k;
	int half=size/2;
	int max,min,ave;
	for(i=half;i<ima1->nr-half;i++)for(j=half;j<ima1->nc-half;j++)
	{
		max=min=ima1->m[i*ima1->nc+j];
		for(k=-half;k<=half;k++)
		{
			if(ima1->m[(i+k)*ima1->nc+j]>max)max=ima1->m[(i+k)*ima1->nc+j];
			if(ima1->m[(i+k)*ima1->nc+j]<min)min=ima1->m[i+k*ima1->nc+j];
			if(ima1->m[i*ima1->nc+(j+k)]>max)max=ima1->m[i*ima1->nc+(j+k)];
			if(ima1->m[i*ima1->nc+(j+k)]<min)min=ima1->m[i*ima1->nc+(j+k)];
		}
		ave=(max+min)/2;
		if(ima1->m[i*ima1->nc+j]<ave) ima2->m[i*ima1->nc+j]=0;
		else 	ima2->m[i*ima1->nc+j]=255;
	}
}
