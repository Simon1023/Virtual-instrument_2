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

		//��l��
		for(i=0;i<256;i++)
		{
			pixelCount[i]=0;
			pixelPro[i]=0;
		}

		//�έp�Ϲ����C�ӦǶ��Ȫ��Ӽ�
		for(i=0;i<ImaSrc->nr;i++)
			for(j=0;j<ImaSrc->nc;j++)
			{
				k=ImaSrc->m[i*ImaSrc->nc+j];
				if(k>=ipixelu0&&k<=ipixelu1)
					pixelCount[k]++;
			}

		//�p��C�ӹ����b�Ϲ��������
		for(i=0;i<256;i++)
			pixelPro[i]=(float)(pixelCount[i])/(float)(pixelSum);

		//Otsu�t��k�A�o��e���M�I������
		//���Ƕ���[0,255]�A�p��X��t�̤j���Ƕ��Ȭ��̨λ֭�
		for(i=0;i<256;i++)
		{
			w0=w1=u0tmp=u1tmp=u0=u1=u=deltaTmp=0;

			for(j=0;j<256;j++)
			{
				if(j<=i)  //�I������
				{
					//�Hi��threshold�����A�Ĥ@���`�����v
					w0+=pixelPro[j];
					u0tmp+=j*pixelPro[j];
				}
				else  //�e������
				{
					//�Hi��threshold�����A�ĤG���`�����v
					w1+=pixelPro[j];
					u1tmp+=j*pixelPro[j];
				}
			}

			u0=u0tmp/w0;  //�Ĥ@���������ǫ�
			u1=u1tmp/w1;  //�ĤG���������ǫ�
			u=u0tmp+u1tmp;  //���Ϫ������ǫ�

			//�p���t
			deltaTmp=w0*(u0-u)*(u0-u)+w1*(u1-u)*(u1-u);
			//��X�̤j��t�H�ι������֭�
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
