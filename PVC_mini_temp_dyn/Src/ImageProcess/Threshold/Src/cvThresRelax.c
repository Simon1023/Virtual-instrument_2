
#include "../cvThresRelax.h"

static void minmax (uc1D *im, float *gmin, float *gmax);
static float update_relax (uc1D *im, f1D *p, f1D *q, int method);
static float R(uc1D *im, int r, int c, int rj, int cj, int l1, int l2, int method);
static void assign_class (uc1D *im, f1D *p, f1D *q, int method);
static float mean (uc1D *im, int method);
static void meanminmax (uc1D *im, int r, int c, float *mean, float *xmin, float *xmax);
static void thresh (uc1D *im, f1D *p, f1D *q);
static float Q(uc1D *im, f1D *p, f1D *q, int r, int c, int cla, int method);

static f1D pp, qq;

//method : 1~3
void cvThresRelax (uc1D *im_ori, uc1D *im, int method)
{
	float res = 0.0, minres = 10000.0;
	f1D p, q;
	int iter = 0, count = 0;

	uc1D_Copy(im, im_ori);
/* Space allocation */
	p = f1D_Initial (im->nr, im->nc);
	q = f1D_Initial (im->nr, im->nc);
	pp = f1D_Initial (im->nr, im->nc);
	qq = f1D_Initial (im->nr, im->nc);

/* Initial assignment of pixel classes */
	assign_class (im, &p, &q, method);
	thresh (im, &p, &q);

/* Relaxation */
	do
	{
	  res = update_relax (im, &p, &q, method);
	  iter += 1;
	  //printf ("Iteration %d residual is %f\n", iter, res);
	  if (res < minres)
	  {
		minres = res;
		count = 1;
	  } else if (fabs(res-minres) < 0.0001)
	  {
		if (count > 2) break;
		else count++;
	  }
	  thresh (im, &p, &q);
	} while (iter < 100 && res > 1.0);

	thresh (im, &p, &q);
}

/* Threshold */
void thresh (uc1D *im, f1D *p, f1D *q)
{
	int i,j;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    if (p->m[i*im->nc+j] > q->m[i*im->nc+j]) im->m[i*im->nc+j] = 0;
	     else im->m[i*im->nc+j] = 255;
}

/* Mean grey level */
float mean (uc1D *im, int method)
{
	int i, j;
	long N, sum=0;
	float result;

	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    sum += im->m[i*im->nc+j];
	if(method == 1){
		N = (long)(im->nr) * (long)(im->nc);
		result = (float)sum/(float)(N);
	}else{
		result = (float)sum/(float)(im->nr)*(float)(im->nc);
	}
	return result;
}

/* Min and max levels */
void minmax (uc1D *im, float *gmin, float *gmax)
{
	int i,j;

	*gmin = *gmax = im->m[0*im->nc+0];
	for (i=0; i<im->nr; i++)
	  for (j=0; j<im->nc; j++)
	    if (*gmin > im->m[i*im->nc+j]) *gmin = im->m[i*im->nc+j];
	     else if (*gmax < im->m[i*im->nc+j]) *gmax = im->m[i*im->nc+j];
}

void assign_class (uc1D *im, f1D *p, f1D *q, int method)
{
	int i,j;
	float ud2, y, z, u, lm2;

/* Rosenfeld & Smith class assignment */
	if(method == 1){
		/* Rosenfeld & Smith class assignment */
		u = mean (im, method);
		minmax (im, &z, &y);
		ud2 = 2.0*(u-z);
		lm2 = 2.0*(y-u);
		/* P is the probability that a pixel is black */
		for (i=0; i<im->nr; i++)
		  for (j=0; j<im->nc; j++)
			if (im->m[i*im->nc+j] <= u)
			{
				p->m[i*im->nc+j] = 0.5 + (u-im->m[i*im->nc+j])/ud2;
				q->m[i*im->nc+j] = 1.0-p->m[i*im->nc+j];
			}
			/* q is the probability that a pixel is white */
			else
			{
				q->m[i*im->nc+j] = (0.5 + (im->m[i*im->nc+j]-u)/lm2);
				p->m[i*im->nc+j] = 1.0 - q->m[i*im->nc+j];
			}
	}else{
		for (i=0; i<im->nr; i++)		/* Mean of local area */
			for (j=0; j<im->nc; j++)
			{
				meanminmax (im, i, j, &u, &z, &y);
				ud2 = 2.0*(u-z);
				lm2 = 2.0*(y-u);
				if (im->m[i*im->nc+j] <= u)
				{
					p->m[i*im->nc+j] = 0.5 + (u - im->m[i*im->nc+j])/ud2;
					q->m[i*im->nc+j] = 1.0-p->m[i*im->nc+j];
				} else {
					q->m[i*im->nc+j] = (0.5 + (im->m[i*im->nc+j]-u)/lm2);
					p->m[i*im->nc+j] = 1.0-q->m[i*im->nc+j];
				}
			}
	}
	return;
}

void meanminmax (uc1D *im, int r, int c, float *mean, float *xmin, float *xmax)
{
	int i,j;
	long sum = 0, k=0;

	*xmin = *xmax = im->m[r*im->nc+c];
	for (i=r-10; i<=r+10; i++)
	  for (j=c-10; j<=c+10; j++)
	  {
	    if (uc1D_Range(im, i, j) != 1) continue;
	    if (*xmin > im->m[i*im->nc+j]) *xmin = im->m[i*im->nc+j];
	    else if (*xmax < im->m[i*im->nc+j]) *xmax = im->m[i*im->nc+j];
	    sum += im->m[i*im->nc+j];
	    k++;
	  }
	*mean = (float)sum/(float)(k);
}

float Q(uc1D *im, f1D *p, f1D *q, int r, int c, int cla, int method)
{
	int i,j;
	float sum = 0.0;

	for (i=r-1; i<=r+1; i++)
	  for (j=c-1; j<=c+1; j++)
	    if (i!=r || j!=c)
	      sum += R(im, r, c, i, j, cla, 0, method)*p->m[i*im->nc+j] +
		  R(im, r, c, i, j, cla, 1, method)*q->m[i*im->nc+j];
	return sum/8.0;
}

float update_relax (uc1D *im, f1D *p, f1D *q, int method)
{
	float z, qw, pk, qb;
	int i,j;

	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	  {
	    qb = (1.0 + Q(im, p, q, i, j, 0, method));
	    qw = (1.0 + Q(im, p, q, i, j, 1, method));
	    pk = p->m[i*im->nc+j]*qb + q->m[i*im->nc+j]*qw;

	    if (pk == 0.0) 
	      continue;

	    pp.m[i*im->nc+j] = p->m[i*im->nc+j]*qb/pk;
	    qq.m[i*im->nc+j] = q->m[i*im->nc+j]*qw/pk;
	  }

	z = 0.0;
	for (i=1; i<im->nr-1; i++)
	  for (j=1; j<im->nc-1; j++)
	  {
		z += fabs(p->m[i*im->nc+j]- pp.m[i*im->nc+j]) + fabs(q->m[i*im->nc+j]- qq.m[i*im->nc+j]);
		p->m[i*im->nc+j] = pp.m[i*im->nc+j];
		q->m[i*im->nc+j] = qq.m[i*im->nc+j];
		qq.m[i*im->nc+j] = pp.m[i*im->nc+j] = 0.0;
	  }
	return z;
}

float R(uc1D *im, int r, int c, int rj, int cj, int l1, int l2, int method)
{
	float xd = 0.0;
	if(method == 3){
		xd = 1.0 - (im->m[r*im->nc+c] - im->m[rj*im->nc+cj])/256.0;
		if (l1 == l2) return xd*0.9;
			return -(1.0-xd)*0.9;
	}else{
		if (l1 == l2) return 0.9;
		return -0.9;
	}
}
