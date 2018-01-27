/* threshold Moments method */

#include "../threMoments.h"

int threMoments (uc1D *im_ori) {
	//  W. Tsai, "Moment-preserving thresholding: a new approach," Computer Vision,
	// Graphics, and Image Processing, vol. 29, pp. 377-393, 1985.
	// Ported to ImageJ plugin by G.Landini from the the open source project FOURIER 0.8
	// by  M. Emre Celebi , Department of Computer Science,  Louisiana State University in Shreveport
	// Shreveport, LA 71115, USA
	//  http://sourceforge.net/projects/fourier-ipal
	//  http://www.lsus.edu/faculty/~ecelebi/fourier.htm
	double total =0;
	double m0=1.0, m1=0.0, m2 =0.0, m3 =0.0, sum =0.0, p0=0.0;
	double cd, c0, c1, z0, z1;	/* auxiliary variables */
	int threshold = -1, i, j;

	double *histo, *data;
	
	histo = (double *)malloc(sizeof(double)*256);
	data = (double *)malloc(sizeof(double)*256);
	for (i=0; i<256; i++) data[i] = 0.0;
	for(i=0; i<im_ori->nr; i++)for(j=0; j<im_ori->nc; j++)
		data[im_ori->m[i*im_ori->nc+j]]++;
		
	total = im_ori->nr*im_ori->nc;
	for (i=0; i<256; i++)
		histo[i]=(double)(data[i]/total); //normalised histogram

	/* Calculate the first, second, and third order moments */
	for (i = 0; i < 256; i++){
		m1 += i * histo[i];
		m2 += i * i * histo[i];
		m3 += i * i * i * histo[i];
	}
	/* 
	First 4 moments of the gray-level image should match the first 4 moments
	of the target binary image. This leads to 4 equalities whose solutions 
	are given in the Appendix of Ref. 1 
	*/
	cd = m0 * m2 - m1 * m1;
	c0 = ( -m2 * m2 + m1 * m3 ) / cd;
	c1 = ( m0 * -m3 + m2 * m1 ) / cd;
	z0 = 0.5 * ( -c1 - sqrt( c1 * c1 - 4.0 * c0 ) );
	z1 = 0.5 * ( -c1 + sqrt( c1 * c1 - 4.0 * c0 ) );
	p0 = ( z1 - m1 ) / ( z1 - z0 );  /* Fraction of the object pixels in the target binary image */

	// The threshold is the gray-level closest  
	// to the p0-tile of the normalized histogram 
	sum=0;
	for (i=0; i<256; i++){
		sum+=histo[i];
		if (sum>p0) {
			threshold = i;
			break;
		}
	}
	return threshold;
}
