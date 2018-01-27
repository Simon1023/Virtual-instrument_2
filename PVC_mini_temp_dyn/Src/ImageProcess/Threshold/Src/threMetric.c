/* threshold Inter Variance method */

#include "../threMetric.h"

int threMetric(uc1D *image)  
{
	int threshold=0, i, j, width=image->nc, height=image->nr;
	int total=width*height, m_total=0;
	int *histo, *histoIncre, *m_histoIncre;
	double u1, u2, metric, min=0;
	
	histo = (int *)malloc(sizeof(int)*256);
	histoIncre = (int *)malloc(sizeof(int)*256);
	m_histoIncre = (int *)malloc(sizeof(int)*256);	
	
	//統計直方圖
	for (i=0; i<256; i++) histo[i] = 0;
	for(i=0;i<height;i++)for(j=0;j<width;j++)
		histo[ image->m[i*image->nc+j] ]++;
	
	//histoIncre[]: 累增灰階直方圖
	//m_histoIncre[]: 累增灰階期望值直方圖
	histoIncre[0] = histo[0];
	m_histoIncre[0] = 0;
	for(i=1;i<256;i++){
		histoIncre[i] = histoIncre[i-1]+histo[i];
		m_histoIncre[i] = m_histoIncre[i-1]+ i*histo[i]; 
	}
	//m_total: 整張image的灰階值之期望值總和
	m_total = m_histoIncre[255];
	//初始化min, 假設histoIncre[0]為0的情況下的metric設為初始值
	for(i=0;i<256;i++)
		min += histo[i]*abs((double)( i-(m_total/total) ));
	
	//u1: 灰階值0~i中所有pixel value的平均
	//u2: 灰階值i+1~255中所有pixel value的平均
	//metric: 灰階值0~k的hist[i]*|i-u1|的總和加上灰階值k+1~255的hist[i]*|i-u2|的總和
	//求metric最小時的灰階值i
	for(i=0;i<256;i++){
		if(histoIncre[i] == 0)continue;	//表示灰階值0~i沒有任何pixel
		if(total==histoIncre[i])break;
		u1 = (double)(m_histoIncre[i]/histoIncre[i]);
		u2 = (double)( (m_total-m_histoIncre[i])/(total-histoIncre[i]) );
		
		metric = 0;
		for(j=0;j<256;j++){
			if(j<=i){
				metric += histo[i]*abs(i-u1);
			}else{
				metric += histo[i]*abs(i-u2);
			}
		}
		if(metric < min){
			min = metric;
			threshold = i;
		}		
	}	
	return threshold;
}
