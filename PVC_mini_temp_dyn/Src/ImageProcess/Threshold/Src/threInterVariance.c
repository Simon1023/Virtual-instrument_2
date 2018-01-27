/* threshold Inter Variance method */

#include "../threInterVariance.h"

int threInterVar(uc1D *image)  
{
	int threshold, i, j, width=image->nc, height=image->nr;
	int total=width*height;
	double *histoPro, m_total=0, w, m, tmp, max=0, variance;
	
	histoPro = (double *)malloc(sizeof(double)*256);
	for(i=0;i<256;i++)histoPro[i]=0;
	//統計直方圖
	for(i=0;i<height;i++)for(j=0;j<width;j++)
		histoPro[ image->m[i*image->nc+j] ]++;
		
	//histoPro[]: 每個gray level的pixel數佔整體pixel數的比例
	//m_total: 整張image的灰階值之期望百分比總和
	for(i=0;i<256;i++){
		histoPro[i] = histoPro[i]/(double)total;
		m_total += i*histoPro[i];
	}
	//w: 灰階值0~i佔了全部pixel數的比例
	//m: 灰階值0~i的期望百分比總和
	//variance: 灰階值i的內方差
	//求variance最大時的灰階值i
	for(i=0;i<256;i++)
	{
		w += histoPro[i];
		if(w == 0)continue;	//避免分母為0的情況			
		m += i*histoPro[i];
		tmp = m_total*w - m;
		variance = (tmp*tmp) / w*(1-w);
		if(variance > max)
		{
			max = variance;
			threshold = i;
		}
	}
	
	return threshold;
}
