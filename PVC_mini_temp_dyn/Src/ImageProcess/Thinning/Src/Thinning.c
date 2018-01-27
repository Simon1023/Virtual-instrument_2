#include "..\Thinning.h"
#include <string.h>
unsigned char temp_array[240*320]={0};
unsigned char temp[240*320]={0};
int thinning(uc1D *ImaSrc)
{
	int success = 0 ;
  int success2 = 0;
  int count ;
  int count2;
  int num_transitions ;
  int transitions ;
	memset(temp_array,0,ImaSrc->nr*ImaSrc->nc);
	memset(temp,0,ImaSrc->nr*ImaSrc->nc);
	
//	unsigned char temp_array[240*320]={0};
//	unsigned char temp[240*320]={0};

//	uc1D temp_array=uc1D_Initial(ImaSrc->nr,ImaSrc->nc);
//	uc1D temp=uc1D_Initial(ImaSrc->nr,ImaSrc->nc);

//   uc2D temp_array;
//	 temp_array.nc = ImaSrc->nc;
//	 temp_array.nr = ImaSrc->nr;
//	 temp_array.m = (unsigned char*) malloc(sizeof(unsigned char)*ImaSrc->nr*ImaSrc->nc);
//   uc2D temp;
//	 temp.nc = ImaSrc->nc;
//	 temp.nr = ImaSrc->nr;
//	 temp.m = (unsigned char*) malloc(sizeof(unsigned char)*ImaSrc->nr*ImaSrc->nc);
   
	
  for (int i = 1; i < ImaSrc->nr-1; i++)
	{
		for (int j = 1; j < ImaSrc->nc-1; j++)
		{
			count = 0;
      num_transitions = 0;
			// check  for N(p)
			if (ImaSrc->m[i*ImaSrc->nc+j] == 255)
			{
				if (ImaSrc->m[(i-1)*ImaSrc->nc+(j-1)] != 0)
					count++;
				if (ImaSrc->m[i*ImaSrc->nc+(j-1)] != 0)
					count++;
		    if (ImaSrc->m[(i+1)*ImaSrc->nc+(j-1)] != 0)
					count++;
				if (ImaSrc->m[(i+1)*ImaSrc->nc+j] != 0)
					count++;
				if (ImaSrc->m[(i-1)*ImaSrc->nc+j] != 0)
					count++;
				if (ImaSrc->m[(i+1)*ImaSrc->nc+(j+1)] != 0)
					count++;
				if (ImaSrc->m[i*ImaSrc->nc+(j+1)] != 0)
					count++;
				if (ImaSrc->m[(i-1)*ImaSrc->nc+(j+1)] != 0)
					count++;
				
				if (count != 8)
				{
					// 2 <= N(p) <= 6
					if (count >= 2 && count <= 6)
					{
						if(ImaSrc->m[(i-1)*ImaSrc->nc+j] == 0 && ImaSrc->m[(i-1)*ImaSrc->nc+(j+1)] == 255)
							num_transitions++ ;
						if(ImaSrc->m[(i-1)*ImaSrc->nc+(j+1)] == 0 && ImaSrc->m[i*ImaSrc->nc+(j+1)] == 255)
							num_transitions++ ;
						if(ImaSrc->m[i*ImaSrc->nc+(j+1)] == 0 && ImaSrc->m[(i+1)*ImaSrc->nc+(j+1)] == 255)
							num_transitions++ ;
						if(ImaSrc->m[(i+1)*ImaSrc->nc+(j+1)] == 0 && ImaSrc->m[(i+1)*ImaSrc->nc+j] == 255)
							num_transitions++ ;
						if(ImaSrc->m[(i+1)*ImaSrc->nc+j] == 0 && ImaSrc->m[(i+1)*ImaSrc->nc+(j-1)] == 255)
							num_transitions++ ;
						if(ImaSrc->m[(i+1)*ImaSrc->nc+(j-1)] == 0 && ImaSrc->m[i*ImaSrc->nc+(j-1)] == 255)
							num_transitions++ ;
						if(ImaSrc->m[i*ImaSrc->nc+(j-1)] == 0 && ImaSrc->m[(i-1)*ImaSrc->nc+(j-1)] == 255)
							num_transitions++ ;
						if(ImaSrc->m[(i-1)*ImaSrc->nc+(j-1)] == 0 && ImaSrc->m[(i-1)*ImaSrc->nc+j] == 255)
							num_transitions++ ;
						//S(p) = 1
						if (num_transitions == 1)
						{
							// if p2 * p4 * p6 = 0
							if (ImaSrc->m[(i-1)*ImaSrc->nc+j] == 0 || ImaSrc->m[i*ImaSrc->nc+(j+1)] == 0 || ImaSrc->m[(i+1)*ImaSrc->nc+j] == 0)
							{
								// if p4 * p6 * p8 = 0
								if(ImaSrc->m[i*ImaSrc->nc+(j+1)] == 0 || ImaSrc->m[(i+1)*ImaSrc->nc+j] == 0 || ImaSrc->m[i*ImaSrc->nc+(j-1)] == 0)
								{
									temp_array[i*ImaSrc->nc+j] = 0;
									success = 1;
								}
								else
									temp_array[i*ImaSrc->nc+j] = 255;
							}
							else
								temp_array[i*ImaSrc->nc+j] = 255;
						}
						else
							temp_array[i*ImaSrc->nc+j] = 255;
					}
					else
						temp_array[i*ImaSrc->nc+j] = 255;
				}
				else
					temp_array[i*ImaSrc->nc+j] = 255;
				
			}
			else
				temp_array[i*ImaSrc->nc+j] = 0;
		}
	}
	//copy thinned image back to original

	for (int a = 1; a < ImaSrc->nr-1; a++)
	{
		for (int b = 1; b < ImaSrc->nc-1; b++)
			ImaSrc->m[a*ImaSrc->nc+b] = temp_array[a*ImaSrc->nc+b];
  }
	
	
	// step 2 of the thinning algorithm
	for (int k = 0; k < ImaSrc->nr; k++)
	{
		for (int l = 0; l < ImaSrc->nc; l++)
		{
			count2 = 0;
			transitions = 0;
      if (ImaSrc->m[k*ImaSrc->nc+l] == 255)
			{
				if (ImaSrc->m[(k-1)*ImaSrc->nc+(l-1)] != 0)
					count2++;
				if (ImaSrc->m[k*ImaSrc->nc+(l-1)] != 0)
					count2++;
				if (ImaSrc->m[(k+1)*ImaSrc->nc+(l-1)] != 0)
					count2++;
				if (ImaSrc->m[(k+1)*ImaSrc->nc+l] != 0)
					count2++;
				if (ImaSrc->m[(k-1)*ImaSrc->nc+l] != 0)
					count2++;
				if (ImaSrc->m[(k+1)*ImaSrc->nc+(l+1)] != 0)
					count2++;
				if (ImaSrc->m[k*ImaSrc->nc+(l+1)] != 0)
					count2++;
				if (ImaSrc->m[(k-1)*ImaSrc->nc+(l+1)] != 0)
					count2++;
				
				if (count2 != 8)
				{
					if (count2 >= 2 && count2 <= 6)
					{
						if(ImaSrc->m[(k-1)*ImaSrc->nc+l] == 0 && ImaSrc->m[(k-1)*ImaSrc->nc+(l+1)] == 255)
							transitions++ ;
						if(ImaSrc->m[(k-1)*ImaSrc->nc+(l+1)] == 0 && ImaSrc->m[k*ImaSrc->nc+(l+1)] == 255)
							transitions++ ;
						if(ImaSrc->m[k*ImaSrc->nc+(l+1)] == 0 && ImaSrc->m[(k+1)*ImaSrc->nc+(l+1)] == 255)
							transitions++ ;
						if(ImaSrc->m[(k+1)*ImaSrc->nc+(l+1)] == 0 && ImaSrc->m[(k+1)*ImaSrc->nc+l] == 255)
							transitions++ ;
						if(ImaSrc->m[(k+1)*ImaSrc->nc+l] == 0 && ImaSrc->m[(k+1)*ImaSrc->nc+(l-1)] == 255)
							transitions++ ;
						if(ImaSrc->m[(k+1)*ImaSrc->nc+(l-1)] == 0 && ImaSrc->m[k*ImaSrc->nc+(l-1)] == 255)
							transitions++ ;
						if(ImaSrc->m[k*ImaSrc->nc+(l-1)] == 0 && ImaSrc->m[(k-1)*ImaSrc->nc+(l-1)] == 255)
							transitions++ ;
						if(ImaSrc->m[(k-1)*ImaSrc->nc+(l-1)] == 0 && ImaSrc->m[(k-1)*ImaSrc->nc+l] == 255)
							transitions++ ;
						if (transitions == 1)
						{
							// if p2 * p4 * p8 =0
							if(ImaSrc->m[(k-1)*ImaSrc->nc+l] == 0 || ImaSrc->m[k*ImaSrc->nc+(l+1)] == 0 || ImaSrc->m[k*ImaSrc->nc+(l-1)] == 0)
							{
								// if p2 * p6 * p8
								if(ImaSrc->m[(k-1)*ImaSrc->nc+l] == 0 || ImaSrc->m[(k+1)*ImaSrc->nc+l] == 0 || ImaSrc->m[k*ImaSrc->nc+(l-1)] == 0)
								{
									temp[k*ImaSrc->nc+l] = 0;
									success2 = 1;
                }
                else
									temp[k*ImaSrc->nc+l] = 255;
							}
              else
								temp[k*ImaSrc->nc+l] = 255;
						}
            else
							temp[k*ImaSrc->nc+l] = 255;
					}
					else
						temp[k*ImaSrc->nc+l] = 255;
				}
				else
					temp[k*ImaSrc->nc+l] = 255;
				
			}
			else
				temp[k*ImaSrc->nc+l] = 0;
		}
	}
	
	for (int a = 0; a < ImaSrc->nr; a++)
	{
		for (int b = 0; b < ImaSrc->nc; b++)
			ImaSrc->m[a*ImaSrc->nc+b] = temp[a*ImaSrc->nc+b];
	}
	
	return success || success2;
}

void Thinning(uc1D *ImaSrc, uc1D *ImaDst)
{
//	for(int i = 0; i < ImaSrc->nr; i++)
//	{
//		for(int j = 0; j < ImaSrc->nc; j++)
//		{
//			if(ImaSrc->m[i*ImaSrc->nc+j] > 100)
//				ImaSrc->m[i*ImaSrc->nc+j] = 0;
//			else
//				ImaSrc->m[i*ImaSrc->nc+j] = 255;
//		}
//	}
	/*------改變前後景------*/
//   for(int i=0;i<ImaSrc->nr;i++){
//   	for(int j=0;j<ImaSrc->nc;j++){
//   		if(ImaSrc->m[i*ImaSrc->nc+j]==255) ImaSrc->m[i*ImaSrc->nc+j]=0;
//   		else ImaSrc->m[i*ImaSrc->nc+j]=255;
//	   }
//   }
   /*-------迭代細線化-------*/
	int continue_do;
	while(1)
	{
		continue_do=thinning(ImaSrc);
		if(continue_do==0)break;
	}

	for(int i = 0; i < ImaSrc->nr; i++)
		for(int j = 0; j < ImaSrc->nc; j++)
			ImaDst->m[i*ImaSrc->nc+j] = ImaSrc->m[i*ImaSrc->nc+j];
}
