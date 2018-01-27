/* threshold Niblack method */

#include "../threNiblack.h"

//row,col : row and column of window
//deviation : (0~10)/10, default:2/10
void threNiblack(uc1D *image, uc1D *outImg, int row, int col, int deviation)
{
	int i, j, ii, jj, hf_row, hf_col, temp;
	float devi = deviation/10, t;
	uc1D meanImg;
	i1D temp1, temp2, count1, count2;
	
	meanImg = uc1D_Initial(image->nr, image->nc);
	temp1 = i1D_Initial(image->nr, image->nc);
	temp2 = i1D_Initial(image->nr, image->nc);
	count1 = i1D_Initial(image->nr, image->nc);
	count2 = i1D_Initial(image->nr, image->nc);
	
	hf_row = row/2;
	hf_col = col/2;
	
	/*
	//creat local mean image
	for(i=0;i<image->nr;i++){
		for(j=0;j<image->nc;j++){
			n=0;
			sum=0;
			for(ii=-hf_row;ii<=hf_row;ii++){
				if(i+ii<0 || i+ii>=image->nr)continue;	//out of range
				for(jj=-hf_col;jj<=hf_col;jj++){
					if(j+jj<0 || j+jj>=image->nc)continue;	//out of range
					sum = sum + image->m[i+ii][j+jj];
					n++;
				}
			}
			meanImg->m[i][j] = sum/n;
		}
	}*/
	
	//creat local mean image	
	//calculate row's sum of first column 
	// count[][]: calculate how many points is valid
	for(j=0;j<image->nc;j++){
		temp1.m[0*image->nc+j] = 0;
		count1.m[0*image->nc+j] = 0;
		for(ii=0;ii<=hf_row;ii++){
			temp1.m[0*image->nc+j] += image->m[ii*image->nc+j];
			count1.m[0*image->nc+j]++;
		}
	}
	//calculate sum of the previous row results
	for (i=1; i<image->nr; i++){
		for (j = 0; j <image->nc; j++){
			temp1.m[i*image->nc+j] = temp1.m[(i-1)*image->nc+j];
			count1.m[i*image->nc+j] = count1.m[(i-1)*image->nc+j];
			//add new row's value
			if(i+hf_row < image->nr){
				temp1.m[i*image->nc+j] += image->m[(i+hf_row)*image->nc+j];
				count1.m[i*image->nc+j]++;
			}
			//sub row-1-hf_row's value;
			if(i-1-hf_row >= 0){
				temp1.m[i*image->nc+j] -= image->m[(i-1-hf_row)*image->nc+j];
				count1.m[i*image->nc+j]--;
			}
		}
	}
	//calculate column's sum of first row   
	for (i=0; i<image->nr; i++)
	{
		temp2.m[i*image->nc+0] = temp1.m[i*image->nc+0];
		count2.m[i*image->nc+0] = count1.m[i*image->nc+0];
		for (jj=1;jj<=hf_col;jj++)
		{
			temp2.m[i*image->nc+0] += temp1.m[i*image->nc+jj];
			count2.m[i*image->nc+0] += count1.m[i*image->nc+jj];
		}
		//(temp2.m[i][0]/count2.m[i][0]) is local mean value
		meanImg.m[i*image->nc+0] = temp2.m[i*image->nc+0]/count2.m[i*image->nc+0];
	}
	
	//calculate true sum 
	for (i=0; i<image->nr; i++){
		for (j=1; j <image->nc; j++){
			temp2.m[i*image->nc+j] = temp2.m[i*image->nc+(j-1)];
			count2.m[i*image->nc+j] = count2.m[i*image->nc+(j-1)];
			//add new column's value
			if(j+hf_col < image->nc){
				temp2.m[i*image->nc+j] += temp1.m[i*image->nc+(j+hf_col)];
				count2.m[i*image->nc+j] += count1.m[i*image->nc+(j+hf_col)];
			}
			//sub column-1-hf_col's value;
			if(j-1-hf_col >= 0){
				temp2.m[i*image->nc+j] -= temp1.m[i*image->nc+(j-1-hf_col)];
				count2.m[i*image->nc+j] -= count1.m[i*image->nc+(j-1-hf_col)];;
			}
			meanImg.m[i*image->nc+j] = temp2.m[i*image->nc+j]/count2.m[i*image->nc+j];
		}
	}
	
	/*
	for(i=0;i<image->nr;i++){
		for(j=0;j<image->nc;j++){
			n=0;
			sum=0;
			//calculate variance
			for(ii=-hf_row;ii<=hf_row;ii++){
				if(i+ii<0 || i+ii>=image->nr)continue;	//out of range
				for(jj=-hf_col;jj<=hf_col;jj++){
					if(j+jj<0 || j+jj>=image->nc)continue;	//out of range
					temp = image->m[i+ii][j+jj]- meanImg->m[i+ii][j+jj];
					sum = sum + temp*temp;
					n++;
				}
			}			
			sum = sum/(n-1);
			//local threshold
			t = meanImg->m[i][j]+ deviation*sum;
			if(image->m[i][j] >= t)
				outImg->m[i][j] = 255;
			else
				outImg->m[i][j] = 0;
		}
	}*/
	
	//calculate local variance
	//calculate row's sum of first column 
	// count[][]: use mean's count[][], because two are same
	for(j=0;j<image->nc;j++){
		temp1.m[0*image->nc+j] = 0;
		for(ii=0;ii<=hf_row;ii++){
			temp = image->m[ii*image->nc+j]- meanImg.m[ii*image->nc+j];
			temp1.m[0*image->nc+j] += temp*temp;
		}
	}
	//calculate sum of the previous row results
	for (i=1; i<image->nr; i++){
		for (j = 0; j <image->nc; j++){
			temp1.m[i*image->nc+j] = temp1.m[(i-1)*image->nc+j];
			//add new row's value
			if(i+hf_row < image->nr){
				temp = image->m[(i+hf_row)*image->nc+j]- meanImg.m[(i+hf_row)*image->nc+j];
				temp1.m[i*image->nc+j] += temp*temp;
			}
			//sub row-1-hf_row's value;
			if(i-1-hf_row >= 0){
				temp = image->m[(i-1-hf_row)*image->nc+j]- meanImg.m[(i-1-hf_row)*image->nc+j];
				temp1.m[i*image->nc+j] -= temp*temp;
			}
		}
	}
	//calculate column's sum of first row   
	for (i=0; i<image->nr; i++)
	{
		temp2.m[i*image->nc+0] = temp1.m[i*image->nc+0];
		for (jj=1;jj<=hf_col;jj++)
		{
			temp2.m[i*image->nc+0] += temp1.m[i*image->nc+jj];
		}
		temp2.m[i*image->nc+0] = temp2.m[i*image->nc+0]/count2.m[i*image->nc+0];
		//local threshold
		t = meanImg.m[i*image->nc+0]+ devi*temp2.m[i*image->nc+0];
		if(image->m[i*image->nc+0] >= t)
			outImg->m[i*image->nc+0] = 255;
		else
			outImg->m[i*image->nc+0] = 0;
	}
	
	//calculate true sum 
	for (i=0; i<image->nr; i++){
		for (j=1; j <image->nc; j++){
			temp2.m[i*image->nc+j] = temp2.m[i*image->nc+(j-1)];
			//add new column's value
			if(j+hf_col < image->nc){
				temp2.m[i*image->nc+j] += temp1.m[i*image->nc+(j+hf_col)];
			}
			//sub column-1-hf_col's value;
			if(j-1-hf_col >= 0){
				temp2.m[i*image->nc+j] -= temp1.m[i*image->nc+(j-1-hf_col)];
			}
			temp2.m[i*image->nc+j] = temp2.m[i*image->nc+j]/count2.m[i*image->nc+j];
			//local threshold
			t = meanImg.m[i*image->nc+j]+ devi*temp2.m[i*image->nc+j];
			if(image->m[i*image->nc+j] >= t)
				outImg->m[i*image->nc+j] = 255;
			else
				outImg->m[i*image->nc+j] = 0;
		}
	}
	
	uc1D_Free(meanImg);
	i1D_Free(temp1);
	i1D_Free(temp2);
	i1D_Free(count1);
	i1D_Free(count2);
		
}	
