#include "..\Otsu.h"
#include <string.h>

void Otsu(uc1D *ImaSrc, uc1D *ImaDst)
{
	int iu0, iu1, ipixelu0, ipixelu1, threshold=0;
	int iNum;
//	int width=ImaSrc->nc;
//	int height=ImaSrc->nr;
//	int *pixelCount;
//	float *pixelPro;
	int i,j,pixelSum=ImaSrc->nc*ImaSrc->nr,k;
	float w0,w1,u0tmp,u1tmp,u0,u1,u,deltaTmp,deltaMax=0;

//	pixelCount=(int *)malloc(256*sizeof(int));
//	pixelPro=(float *)malloc(256*sizeof(float));
	
	int pixelCount[256]={0};
	float pixelPro[256]={0.0f};

	for(iNum=0;iNum<=255;iNum++)
	{
		if(iNum==0)
		{
			ipixelu0=0;
			ipixelu1=255;
		}
		else
		{
			ipixelu0=iu0;
			ipixelu1=iu1;
		}

		//初始化
		for(i=0;i<256;i++)
		{
			pixelCount[i]=0;
			pixelPro[i]=0;
		}

		//統計圖像中每個灰階值的個數
		for(i=0;i<ImaSrc->nr;i++)
			for(j=0;j<ImaSrc->nc;j++)
			{
				k=ImaSrc->m[i*ImaSrc->nc+j];
				if(k>=ipixelu0&&k<=ipixelu1)
					pixelCount[k]++;
			}

		//計算每個像素在圖像中的比例
		for(i=0;i<256;i++)
			pixelPro[i]=(float)(pixelCount[i])/(float)(pixelSum);

		//Otsu演算法，得到前景和背景分割
		//取灰階度[0,255]，計算出方差最大的灰階值為最佳閥值
		for(i=0;i<256;i++)
		{
			w0=w1=u0tmp=u1tmp=u0=u1=u=deltaTmp=0;

			for(j=0;j<256;j++)
			{
				if(j<=i)  //背景部分
				{
					//以i為threshold分類，第一類總的機率
					w0+=pixelPro[j];
					u0tmp+=j*pixelPro[j];
				}
				else  //前景部分
				{
					//以i為threshold分類，第二類總的機率
					w1+=pixelPro[j];
					u1tmp+=j*pixelPro[j];
				}
			}

			u0=u0tmp/w0;  //第一類的平均灰度
			u1=u1tmp/w1;  //第二類的平均灰度
			u=u0tmp+u1tmp;  //全圖的平均灰度

			//計算方差
			deltaTmp=w0*(u0-u)*(u0-u)+w1*(u1-u)*(u1-u);
			//找出最大方差以及對應的閥值
			if(deltaTmp>deltaMax)
			{
				deltaMax=deltaTmp;
				threshold=i;
				iu0=u0;
				iu1=u1;
			}
		}
	}

	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			if(ImaSrc->m[i*ImaSrc->nc+j]<threshold)
				ImaDst->m[i*ImaDst->nc+j]=0;
			else
				ImaDst->m[i*ImaDst->nc+j]=255;
		}
}
