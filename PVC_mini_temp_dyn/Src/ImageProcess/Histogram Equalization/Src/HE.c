#include "..\HE.h"

void HE(uc1D *ImaSrc, uc1D *ImaDst)
{
	unsigned char *pin = ImaSrc->m;
	unsigned char *pout = ImaDst->m;
	unsigned int ImaSize = ImaSrc->nr*ImaSrc->nc;
	unsigned int histo[256]={0};  //histogram
	float accpbhisto[256]={0.0}; // cumulative histogram
	int table[256]={0};// Look-up table for mapping fuction of histogram equalization

	for(int i=0;i<ImaSrc->nr;i++)
	{
		for(int j=0;j<ImaSrc->nc;j++)
		{
			histo[ImaSrc->m[i*ImaSrc->nc+j]]++;
		}
	}
// Compute cumulative histogram
	accpbhisto[0]=((float)histo[0]/ImaSize);
	for(int i=1;i<256;i++)
		accpbhisto[i]=accpbhisto[i-1]+((float)histo[i]/ImaSize);
	
// compute mapping function
	for(int i=0;i<256;i++)
		table[i]=(unsigned char)(accpbhisto[i]*255.0f);   //Program is over here¡I¡I¡I

// Enhancement
	pin = ImaSrc->m;
	pout = ImaDst->m;
	for(int i=0;i<ImaSrc->nr;i++)
		for(int j=0;j<ImaSrc->nc;j++)
		{
			ImaDst->m[i*ImaSrc->nc+j]=table[ImaSrc->m[i*ImaSrc->nc+j]];
//			*pout = (unsigned char)table[*pin];
//			pin++;
//			pout++;
		}
}
