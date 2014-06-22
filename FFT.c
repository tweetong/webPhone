#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "util/die.h"
#include "util/params.h"
#include "util/init.h"

int FFT_LPF( unsigned char *data , int N )
{
  long m, mh, i, j, k, irev;
  double wr, wi, xr, xi;
  double *ar,*ai;
  ar = (double*)calloc( sizeof(double), N );
  ai = (double*)calloc( sizeof(double), N );
  double theta;
  //FFT
  int flg=1;
  theta = flg * 2 * M_PI / N;
  if(flg==1){
    for(i=0;i<N;i++){
      ar[i]=(double)(data[i]-128)/128;
    }
  }
  for(i=0;i<N;i++){
      ai[i]=0;
  }
  i = 0;
  for (j = 1; j<N - 1; j++) {
    for (k = N>> 1; k > (i ^= k); k >>= 1);
    if (j < i) {
      xr = ar[j];
      xi = ai[j];
      ar[j] = ar[i];
      ai[j] = ai[i];
      ar[i] = xr;
      ai[i] = xi;
    }
  }
  for (mh = 1; (m = mh << 1) <= N; mh = m) {
    irev = 0;
    for (i = 0; i < N; i += m) {
      wr = cos(theta * irev);
      wi = sin(theta * irev);
      for (k = N >> 2; k > (irev ^= k); k >>= 1);
      for (j = i; j < mh + i; j++) {
    k = j + mh;
    xr = ar[j] - ar[k];
    xi = ai[j] - ai[k];
    ar[j] += ar[k];
    ai[j] += ai[k];
    ar[k] = wr * xr - wi * xi;
    ai[k] = wr * xi + wi * xr;
      }
    }
  }
  //LPF
 
  for(i=N/16;i<N;i++){
    ar[i]=ar[i]*(double)N/(16.0*(double)i);
    ai[i]=ai[i]*(double)N/(16.0*(double)i);
 
    /* ar[i]=ar[i]*(double)N/(64.0*(double)i*(double)i); */
    /* ai[i]=ai[i]*(double)N/(64.0*(double)i*(double)i); */
 
    //ar[i]=ai[i]=0;
    }
  //IFFT
  flg=-1;
  theta = flg * 2 * M_PI / N;
 
  i = 0;
  for (j = 1; j<N - 1; j++) {
    for (k = N>> 1; k > (i ^= k); k >>= 1);
    if (j < i) {
      xr = ar[j];
      xi = ai[j];
      ar[j] = ar[i];
      ai[j] = ai[i];
      ar[i] = xr;
      ai[i] = xi;
    }
  }
  for (mh = 1; (m = mh << 1) <= N; mh = m) {
    irev = 0;
    for (i = 0; i < N;i += m) {
      wr = cos(theta * irev);
      wi = sin(theta * irev);
      for (k = N >> 2; k > (irev ^= k); k >>= 1);
      for (j = i; j < mh + i; j++) {
    k = j + mh;
    xr = ar[j] - ar[k];
    xi = ai[j] - ai[k];
    ar[j] += ar[k];
    ai[j] += ai[k];
    ar[k] = wr * xr - wi * xi;
    ai[k] = wr * xi + wi * xr;
      }
    }
  }
    
  //逆変換
  for(i=0; i<N; i++){
    ar[i] /= N;
    data[i] = (unsigned char)(ar[i]*128 + 128);
 
    ai[i] /= N;
  }
   
  return 0;
}

/*
void fft(double ar[], double ai[], int n, int iter, int flag)
{
  int i, it, j, j1, j2, k, xp, xp2;
  double arg, dr1, dr2, di1, di2, tr, ti, w, wr, wi;

  if(n < 2)
    {
      fprintf(stderr, "Error : n < 2  in fft()\n");
      return;
    }
  if(iter <= 0)
    {
      iter = 0;
      i = n;
      while((i /= 2) != 0)	iter++;
    }
  j = 1;
  for(i = 0; i < iter; i++)	j *= 2;
  if(n != j)
    {
      fprintf(stderr, "Error : n != 2 ^ k  in fft1()\n");
      return;
    }
  w = (flag? M_PI: -M_PI) / (double)n;
  xp2 = n;
  for(it = 0; it < iter; it++)
    {
      xp = xp2;
      xp2 /= 2;
      w *= 2;
      for(k = 0, i = - xp; k < xp2; i++)
	{
	  wr = cos(arg = w * k++);
	  wi = sin(arg);
	  for(j = xp; j <= n; j += xp)
	    {
	      j2 = (j1 = j + i) + xp2;
	      tr = (dr1 = ar[j1]) - (dr2 = ar[j2]);
	      ti = (di1 = ai[j1]) - (di2 = ai[j2]);
	      ar[j1] = dr1 + dr2;
	      ai[j1] = di1 + di2;
	      ar[j2] = tr * wr - ti * wi;
	      ai[j2] = ti * wr + tr * wi;
	    }
	}
    }
  j = j1 = n / 2;
  j2 = n - 1;
  for(i = 1; i < j2; i++)
    {
      if(i < j)
	{
	  w = ar[i];
	  ar[i] = ar[j];
	  ar[j] = w;
	  w = ai[i];
	  ai[i] = ai[j];
	  ai[j] = w; 
	}
      k = j1;
      while(k <= j)
	{
	  j -= k;
	  k /= 2;
	}
      j += k;
    }
  if(flag == 0)	return;
  w = 1. / (double)n;
  for(i = 0; i < n; i++)
    {
      ar[i] *= w;
      ai[i] *= w;
    }
  return;
}
*/

int main( int argc, char *argv[] )
{
  int  i, nskip, framelen,flag,iter;
  double freq[100000];
  short  *sdata;
  double *xr, *xi, *ar, *ai;
  FILE *fpDAT;
  
  if( 4 != argc ) {
    fprintf( stderr, "Usage : %s DATfile skip[sample] frame_length[sample]\n", argv[0] );
    exit( 1 );
  }

  if( NULL == ( fpDAT = fopen( argv[1], "r" ) ) )  exit( 1 );
  if( 0 > ( nskip    = atoi( argv[2] ) ) )  exit( 1 );
  if( 0 > ( framelen = atoi( argv[3] ) ) )  exit( 1 );

  fprintf( stderr, "# DATfile = %s\n", argv[1] );
  fprintf( stderr, "# %d samples are skipped.\n", nskip );
  fprintf( stderr, "# 1 frame contains %d sampels.\n", framelen );

  sdata = (short*)calloc( sizeof(short), framelen );
  ar = (double*)calloc( sizeof(double), framelen );
  ai = (double*)calloc( sizeof(double), framelen );
  if( NULL == sdata || NULL == ar || NULL == ai )  exit( 1 );

  fseek( fpDAT, nskip*sizeof(short), SEEK_SET );
  fread( sdata, sizeof(short), framelen, fpDAT );
  fclose( fpDAT );

  FFT_LPF(sdata,framelen);

  /* for( i = 0 ; i < framelen ; i++ ) {
    ar[i] = sdata[i];
    ai[i] = 0.0;
  }

  flag = 0;
  iter = 0;

  fft( ar,ai,framelen,iter,flag);//フーリエ 
  */
  int fd;
  fd = open("/dev/dsp",O_RDWR,0644);
  /*
  FILE *fd = fopen("/dev/dsp","w");
  if(fd == NULL){perror("fopen");exit(1);}
  */
 /* for( i = 0 ; i < framelen ; i++ ) {
    freq[i] = ((double)i)/((double)framelen)*8000;
    if(freq[i] >= 3400){
      ar[i] = 0.0;
      ai[i] = 0.0;
    }
    }*/

 /* flag = 1;
  iter = 0;

  fft( ar,ai,framelen,iter,flag);//逆フーリエ

  for( i = 0 ; i < framelen ; i++ ) {
    sdata[i] = (short)(ar[i]);
  }
  */

  write(fd,sdata,framelen);

  close(fd);

  return( 0 );
}
