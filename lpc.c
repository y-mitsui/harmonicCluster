#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include "harmonicCluster.h"
#include "error.h"



double* vectorScalarMult(double *x,int num,double y){
	double *result;
	
	dmalloc(result,sizeof(double)*num);
	int i;
	for(i=0;i<num;i++){
		result[i]=x[i]*y;
	}

	return result;
}
double* vectorVectorAdd(double* x,int num,double* y){
	double *result;

	dmalloc(result,sizeof(double)*num);
	int i;
	for(i=0;i<num;i++){
		result[i]=x[i]+y[i];
	}

	return result;
}


//autocorr ���ȑ��֊֐�
// x:�f�[�^�z��
// len:�z��̗v�f��
// lagSize:�ő�̃��O(1�`lagSize�܂ł̑��ւ����߂�)
// �߂�l�F���ȑ��֊֐�
double *autocorr(const double *x,const int len,const int lagSize){
	double *r;
	int lag,j;

	dcalloc(r,lagSize*sizeof(double));
	
	for(lag=0;lag<lagSize;lag++){
		for(j=0;j<len-lag;j++){
			r[lag]+=(double)x[j] * x[j + lag];
		}
	}
	return r;
}


// LevinsonDurbin AR���f�����\�z
// r:���ȑ��֊֐�
// A:���߂�ꂽAR�W��(�߂�l)
// lpcOrder:����
// �߂�l�F�m�C�Y
double LevinsonDurbin(const double *r,double **A,const int lpcOrder){
	double *Ak;

	dcalloc(Ak,(lpcOrder+1)*sizeof(double));
	int k,j;
	double *U,*V;

	Ak[0] = 1.0;
	Ak[1] = - r[1] / r[0];
	double Ek = r[0] + r[1] * Ak[1];
	double lambda = - r[1] / r[0];

	dcalloc(U,sizeof(double)*(lpcOrder+2));
	dcalloc(V,sizeof(double)*(lpcOrder+2));

	for(k=1;k<lpcOrder;k++){
		lambda = 0.0;
		for(j=0;j<k+1;j++){
			lambda -= Ak[ j ] * r[ k + 1 - j ];
		}
		lambda /= Ek;

		U[0]=1.0;
		for(j=1;j<k+1;j++)
			U[j]=Ak[j];
		U[j]=0.0;

		V[0]=0.0;
		for(j=k;j>0;j--)
			V[k-j+1]=Ak[j];
		V[k-j+1]=1.0;
		double *v=vectorScalarMult(V,k+2,lambda);
		free(Ak);
		Ak=vectorVectorAdd(U,k+2,v);
		free(v);

		Ek *= 1.0 - lambda * lambda;
	}

	*A=&Ak[0];

	free(U);
	free(V);
	return Ek;
}

//freqz FIR�t�B���^�̎��g�����������߂�
// b a :�t�B���^�W��
// nB: b�z��̗v�f��
// nA: a�z��̗v�f��
// N: ���g���̕������i�傫���l�قǍׂ������ׂ�j
// �߂�l�F���g�������֐�
double *freqz(const double *b,const double *a,const int nB,const int nA,const int N){
	double *ret;
	dmalloc(ret,sizeof(double)*(N+1));
	int n,i;
	double N2pi=2*M_PI/N;
	for (n = 0; n < N + 1; ++n) {
		double complex z,x,numerator,denominator,t;
		x=-I*N2pi * n;
		z=cexp(x);

		denominator=numerator=0.0+I*0.0;
		for (i = 0; i < nB; ++i) {
			t=b[nB - 1 - i] * cpow(z, i);
			
			numerator +=t;
		}
		for (i = 0; i < nA; ++i) {
			t=a[nA - 1 - i] * cpow(z, i);
			denominator += t;
		
		}
		

		ret[n]=cabs(numerator / denominator);
	}
	return ret;
	
}
