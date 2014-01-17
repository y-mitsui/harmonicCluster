#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include <getopt.h>
#include "lpc.h"
#include "list.h"
#include "error.h"
#include "wave.h"
#include "harmonicCluster.h"

int *int2Number(int n){
	int *ret;

	ret=malloc(sizeof(int));
	*ret=n;
	return ret;
}
int number2Int(void *number){
	return *((int*)number);
}

double gauss(int f,int n,int k,double *centroid){
	double var=36.0;

	return (1/sqrt(2*M_PI*var))*exp(-pow(log(f)-log(n*centroid[k]),2)/(2*var));
}
list_t** harmonicClustering(const double *freq,const int numFreq,const int numKernel,const int numHarmonicSound,const int loop){
	double *centroid;
	int i,j,k,repeatCounter,f,n;
	double sum,sum2;
	int *kernel;
	int min,harmonic,nKernel,distance;
	

	centroid=malloc(sizeof(double)*numKernel);
	centroid[0]=100;				//セントロイドの初期値

	kernel=malloc(sizeof(double)*numKernel);
	double *weightDenomi=calloc(1,sizeof(double)*numFreq);

	for(repeatCounter=0;repeatCounter<loop;repeatCounter++){
		
		for(f=0;f<numFreq;f++){
			for(k=0;k<numKernel;k++){
				for(n=0;n<numHarmonicSound;n++){
					weightDenomi[f]+=gauss((f+1),(n+1),k,centroid);
				}
			}
		}
		for(k=0;k<numKernel;k++){
			sum=sum2=0;
			for(n=0;n<numHarmonicSound;n++){
				for(f=0;f<numFreq;f++){
					double d=log(f+1)-log(n+1);
					double w=gauss((f+1),(n+1),k,centroid)/weightDenomi[f];
					sum+=(d*w*freq[f]);
					sum2+=(w*freq[f]);
				}
			}
			centroid[k]=exp(sum/sum2);
			
		}
		


	}

	list_t **result=malloc(sizeof(list_t **)*numKernel);
	for(i=0;i<numKernel;i++){
		dcalloc(result[i],sizeof(list_t)*numHarmonicSound);
	}
	for(j=0;j<numFreq;j++){
		min=numFreq;
		for(i=0;i<numKernel;i++){
			n=round ((j+1)/centroid[i]);
			if(n < 1) n=1;
			if(n > numHarmonicSound) n=numHarmonicSound;
			distance=abs((int)j-centroid[i]*n);
			if(min > distance){
				min=distance;
				harmonic=n-1;
				nKernel=i;
			}
			
		}
		addList(&result[nKernel][harmonic],int2Number(j));
	}
	
	free(kernel);
	free(weightDenomi);
	free(centroid);
	return result;
}

static double *hamming(double *w, const int leng)
{
   int i;
   double arg;
   double *p;

   arg = 2*M_PI / (leng - 1);
   for (p = w, i = 0; i < leng; i++)
      *p++ = 0.54 - 0.46 * cos(i * arg);

   return (w);
}
void window( double *x, const int size){
	int i;
	double *w = NULL;

	w=malloc(sizeof(double)*size);

	hamming(w, size);

	for (i = 0; i < size; i++)
		x[i] = x[i] * w[i];
	free(w);
}

double *fir(double *x,int n,double* a,int na){
	double *y;
	int i,j;
	y=calloc(1,sizeof(double)*n);
	for(i=0;i<n;i++){
		for(j=0;j<na;j++){
			if(i-j>=0)
				y[i]+=a[j]*x[i-j];
		}
	}
	return y;
}


static void usage(void){
	fprintf(stderr,"Usage: harmonicCluster [option] [wav file]\n");
	fprintf(stderr," -l [frame length]\n");
	fprintf(stderr," -p [frame shift]\n");
	fprintf(stderr," -d [lpc order]\n");
	fprintf(stderr," -n [frame number]\n");
	fprintf(stderr," -h [number of harmonic ]\n");
	fprintf(stderr," -k [number of kernel ]\n");
  	exit(1);
}

#define FREQ_NUM 6000
int main(int argc,char **argv){
	cell_t *cur;
	double *frame;
	int i,j;
	int frameLength=320;
	int frameShift=320;
	int frameNumber=0;
	int lpcOrder = 16;
	int numHarmonicSound=2;
	int numKernel=1;
	char buf[1024];
	FILE *fp;

	int		ch;
	extern char	*optarg;
	extern int	optind, opterr;


	while ((ch = getopt(argc, argv, "l:p:d:n:")) != -1){
    		switch (ch){
    		case 'l':
			frameLength=atoi(optarg);
			break;
		case 'p':
			frameShift=atoi(optarg);
			break;
		case 'd':
			lpcOrder=atoi(optarg);
			break;
		case 'n':
			frameNumber=atoi(optarg);
			break;
		default:
		      usage();
		}
	}
	argc -= optind;
	argv += optind;

	if(argc!=1) usage();


	Sound *snd;
	if((snd = Read_Wave(argv[0])) == NULL){
		printf("error:%s\n",argv[0]);
		exit(1);
	}
	dmalloc(frame,sizeof(double)*frameLength);


	for(j=0;j<frameLength && (frameShift*frameNumber+j) < snd->datanum;j++){
		frame[j]=(double)snd->monaural16[frameShift*frameNumber+j];
	}

	//プリエンファシスフィルタをかける
	double b[2]={1.0,-0.99};
	frame=fir(frame,j,b,2);

	//ハミング窓
	window(frame,j);


	double *r = autocorr(frame,j, lpcOrder + 1);

		

	double *a;
	double e=LevinsonDurbin(r,&a,lpcOrder);

	double tmp[1];
	tmp[0]=sqrt(e);
	double *p=freqz(tmp,a,1,lpcOrder+1,FREQ_NUM);

	list_t **class=harmonicClustering(p,FREQ_NUM/2,numKernel,numHarmonicSound,10);
	
	
	for(i=0;i<numKernel;i++){
		for(j=0;j<numHarmonicSound;j++){
			snprintf(buf,sizeof(buf),"kernel%d_harmonic%d.txt",i,j);
			dfopen(fp,buf,"w",return EXIT_FAILURE);
			for(cur=class[i][j].first;cur;cur=cur->next){
				int freqNum=number2Int(cur->data);
				fprintf(fp,"%d %lf\n",freqNum,p[freqNum]);
			}
			fclose(fp);
		}
	}
	free(p);
	free(r);
	free(a);
	
	return EXIT_SUCCESS;
}
