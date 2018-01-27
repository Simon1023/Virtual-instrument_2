#include "..\Optimal.h"

void Optimal(uc1D *ImaSrc, uc1D *ImaDst)
{
	double u1, u2;
	int Told, Tnew;
	int t1, t2;
	u1=u2=0;
	t1=t2=0;
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			if((i==0)||(i==ImaSrc->nr-1)||(j==0)||(j==ImaSrc->nc-1))
			{
				t1++;
				u1=u1+ImaSrc->m[i*ImaSrc->nc+j];
			}
			else
			{
				t2++;
				u2=u2+ImaSrc->m[i*ImaSrc->nc+j];
			}
		}
	Told=0;
	u1=u1/t1;
	u2=u2/t2;
	Tnew=(u1+u2)/2;
	while(Tnew!=Told)
	{
		u1=0;
		u2=0;
		t1=0;
		t2=0;
		for(int i=0;i<ImaSrc->nr;i++)
			for(int j=0;j<ImaSrc->nc;j++)
			{
				if(ImaSrc->m[i*ImaSrc->nc+j]<Tnew)
				{
					t1++;
					u1=u1+ImaSrc->m[i*ImaSrc->nc+j];
				}
				else
				{
					t2++;
					u2=u2+ImaSrc->m[i*ImaSrc->nc+j];
				}
			}

		Told=Tnew;
		u1=u1/t1;
		u2=u2/t2;
		Tnew=(u1+u2)/2;
	}
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			if(ImaSrc->m[i*ImaSrc->nc+j]>Tnew)
				ImaDst->m[i*ImaSrc->nc+j]=255;
			else
				ImaDst->m[i*ImaSrc->nc+j]=0;
		}
}
