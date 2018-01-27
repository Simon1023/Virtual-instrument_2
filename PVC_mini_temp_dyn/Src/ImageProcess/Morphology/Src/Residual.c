#include "..\Residual.h"

const signed char seX[5]={0,-1,0,1,0}, seY[5]={-1,0,0,0,1};

//void Residual(uc1D *ImaSrc, uc1D *ImaDst)
//{	
//	uc1D ima_temp=uc1D_Initial(ImaSrc->nr,ImaSrc->nc);
//	for(int i=0;i<ImaSrc->nr;i++)
//	{
//		for(int j=0;j<ImaSrc->nc;j++)
//		{
//			ima_temp.m[i*ImaSrc->nc+j]=ImaSrc->m[i*ImaSrc->nc+j];
//		}
//	}
////	ima_temp.m=ImaSrc->m;
//	/*--binaryErosion--*/
//	
//	for(int i=0;i<ImaSrc->nr;i++)
//	{
//		for(int j=0;j<ImaSrc->nc;j++)
//		{
//			if(ImaSrc->m[i*ImaSrc->nc+j] >= 100)
//				ImaSrc->m[i*ImaSrc->nc+j] = 255;
//			else
//				ImaSrc->m[i*ImaSrc->nc+j] = 0;
//			ImaDst->m[i*ImaSrc->nc+j]=ImaSrc->m[i*ImaSrc->nc+j];
//		}
//	}
//		
//	for(int i=1;i<ImaSrc->nr-1;i++)
//	{
//		for(int j=1;j<ImaSrc->nc-1;j++)
//		{
//			if(ImaSrc->m[i*ImaSrc->nc+j]==0)
//			{
//				for(int k=0;k<5;k++)
//				{
//					if(ImaSrc->m[(i+seY[k])*ImaSrc->nc+(j+seX[k])]==255)
//					{
//						ImaDst->m[i*ImaSrc->nc+j]=255;
//						break;
//					}
//				}
//			}
//		}
//	}
//	
//	/*--binaryDilation--*/
//	for(int i=0;i<ImaSrc->nr;i++)
//	{
//		for(int j=0;j<ImaSrc->nc;j++)
//		{
//			ImaSrc->m[i*ImaSrc->nc+j]=ImaDst->m[i*ImaSrc->nc+j];
//		}
//	}
//	
//	for(int i=1;i<ImaSrc->nr-1;i++)
//	{
//		for(int j=1;j<ImaSrc->nc-1;j++)
//		{
//			if(ImaDst->m[i*ImaSrc->nc+j]==255)
//			{
//				for(int k=0;k<5;k++)
//				{
//					if(ImaDst->m[(i+seY[k])*ImaSrc->nc+(j+seX[k])]==0)
//					{
//						ImaSrc->m[i*ImaSrc->nc+j]=0;
//						break;
//					}
//				}
//			}
//		}
//	}
//	
//	/*--residual--*/
//	for(int i=0;i<ImaSrc->nr;i++)
//	{
//		for(int j=0;j<ImaSrc->nc;j++)
//		{
//			if(ImaSrc->m[i*ImaSrc->nc+j]!=ima_temp.m[i*ImaSrc->nc+j])
//				ImaDst->m[i*ImaSrc->nc+j]=255;
//			else
//				ImaDst->m[i*ImaSrc->nc+j]=0;
//		}
//	}		
//}

void T_binaryErosion(uc1D *ImaSrc, uc1D *ImaDst)
{
	unsigned char *pin = ImaSrc->m;
	unsigned char *pout = ImaDst->m;
	
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			if(*pin >= 100)
				*pin = 255;
			else
				*pin = 0;
			*pout = *pin;
			pin++;
			pout++;
		}
		
	pin = ImaSrc->m + 1 + ImaSrc->nc;
	pout = ImaDst->m + 1 + ImaDst->nc;

	for(int i=1;i<ImaSrc->nr-1;i++)
	{
		for(int j=1;j<ImaSrc->nc-1;j++)
		{
			if((*pin)==0)
			{
				for(int k=0;k<5;k++)
					if(ImaSrc->m[(i+seY[k])*ImaSrc->nc+(j+seX[k])]==255)
					{
						*pout=255;
						break;
					}
			}
			pin++;
			pout++;
		}
		pin += 2;
		pout += 2;
	}
}
void T_binaryDilation(uc1D *ImaSrc, uc1D *ImaDst)
{
	unsigned char *pin = ImaSrc->m;
	unsigned char *pout = ImaDst->m;
	
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			if(*pin >= 100)
				*pin = 255;
			else
				*pin = 0;
			*pout = *pin;
			pin++;
			pout++;
		}
	
	pin = ImaSrc->m + 1 + ImaSrc->nc;
	pout = ImaDst->m + 1 + ImaDst->nc;

	for(int i=1;i<ImaSrc->nr-1;i++)
	{
		for(int j=1;j<ImaSrc->nc-1;j++)
		{
			if(*pin==255)
			{
				for(int k=0;k<5;k++)
					if(ImaSrc->m[(i+seY[k])*ImaSrc->nc+(j+seX[k])]==0)
					{
						*pout=0;
						break;
					}
			}
			pin++;
			pout++;
		}
		pin += 2;
		pout += 2;
	}
}


void Residual(uc1D *ImaSrc, uc1D *ImaDst)
{
	uc1D ima_temp;
	ima_temp=uc1D_Initial(ImaSrc->nr,ImaSrc->nc);
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
			ima_temp.m[i*ImaSrc->nc+j]=ImaSrc->m[i*ImaSrc->nc+j] > 100 ? 255 : 0;
	T_binaryErosion(ImaSrc,ImaDst);
	T_binaryDilation(ImaDst,ImaSrc);
	for(int i=0;i<ImaSrc->nr;i++)
	{
		for(int j=0;j<ImaSrc->nc;j++)
		{
			if(ImaSrc->m[i*ImaSrc->nc+j]!=ima_temp.m[i*ImaSrc->nc+j])
				ImaDst->m[i*ImaSrc->nc+j]=255;
			else
				ImaDst->m[i*ImaSrc->nc+j]=0;
		}
	}	
	uc1D_Free(ima_temp);
}


