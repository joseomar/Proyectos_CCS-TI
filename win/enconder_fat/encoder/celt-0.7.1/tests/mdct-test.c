#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#include <stdio.h>
#include "mdct.h"

#include "arch.h"

#include "../libcelt/stack_alloc.h"
#define CELT_C
/* 
#include "../libcelt/kiss_fft.c"
#include "../libcelt/mdct.c"
  */
void SCI_send_string(char *mensaje){
 int tam =strlen(mensaje);
 int i;
 for ( i=0;i<tam;i++){
    SCI1C2_TE=1; //habilito la transmicion
    SCI1D= mensaje[i];     //escribo el dato 
    while (SCI1S1_TDRE==0){//espero que se envie el dato
    }    
        
 }
}


#include <stdarg.h>

void print( char * fmt, ... )
{
  // create and start the va_list
  va_list listPointer ;
   va_start( listPointer, fmt ) ;

  // vsprintf is an example of
  // a function that works with
  // an "already started" va_list
   char buf[ 150 ] ;
  vsprintf( buf, fmt, listPointer ) ;
  SCI_send_string(buf);
  
}


#define TPM3_PER_SEC 624994.039501416

static long Big_counter;

void tic (void){
Big_counter = 0;
TPM3CNT = 0; 

}


__interrupt void isrVtpm3ovf(void)
{
//TPM3SC;
//lee registro de estado y control
TPM3SC_TOF=0;
//aclare bandera de sobreflujo

  Big_counter++;

}

void toc (void){
//Big_counter = 0;
//TPM3CNT = 0; 
double secs = 0;
char mensaje[80];
secs = (double) (Big_counter*0xFF+TPM3CNT) / (double)TPM3_PER_SEC;
    print("Elapsed seconds = %f \n", secs);
   // SCI_send_string(mensaje);

}

#ifndef M_PI
#define M_PI 3.141592653
#endif

#ifdef FIXED_DEBUG  
long long celt_mips=0;
#endif
int ret = 0;
void check(kiss_fft_scalar  * in,kiss_fft_scalar  * out,int nfft,int isinverse)
{
    int bin,k;
    double errpow=0,sigpow=0;
    double snr;
    for (bin=0;bin<nfft/2;++bin) {
        double ansr = 0;
        double difr;

        for (k=0;k<nfft;++k) {
           double phase = 2*M_PI*(k+.5+.25*nfft)*(bin+.5)/nfft;
           double re = cos(phase);
            
           re /= nfft/4;

           ansr += in[k] * re;
        }
        /*printf ("%f %f\n", ansr, out[bin]);*/
        difr = ansr - out[bin];
        errpow += difr*difr;
        sigpow += ansr*ansr;
    }
    snr = 10*log10(sigpow/errpow);
    print("nfft=%d inverse=%d,snr = %f\n",nfft,isinverse,snr );
    if (snr<60) {
       print( "** poor snr: %f **\n", snr);
       ret = 1;
    }
}

void check_inv(kiss_fft_scalar  * in,kiss_fft_scalar  * out,int nfft,int isinverse)
{
   int bin,k;
   double errpow=0,sigpow=0;
   double snr;
   for (bin=0;bin<nfft;++bin) {
      double ansr = 0;
      double difr;

      for (k=0;k<nfft/2;++k) {
         double phase = 2*M_PI*(bin+.5+.25*nfft)*(k+.5)/nfft;
         double re = cos(phase);

         //re *= 2;

         ansr += in[k] * re;
      }
      /*printf ("%f %f\n", ansr, out[bin]);*/
      difr = ansr - out[bin];
      errpow += difr*difr;
      sigpow += ansr*ansr;
   }
   snr = 10*log10(sigpow/errpow);
   print("nfft=%d inverse=%d,snr = %f\n",nfft,isinverse,snr );
   if (snr<60) {
      print( "** poor snr: %f **\n", snr);
      ret = 1;
   }
}


void test1d(int nfft,int isinverse)
{
    mdct_lookup cfg;
    size_t buflen = sizeof(kiss_fft_scalar)*nfft;

    kiss_fft_scalar  * in = (kiss_fft_scalar*)malloc(buflen);
    kiss_fft_scalar  * out= (kiss_fft_scalar*)malloc(buflen);
    celt_word16  * window= (celt_word16*)malloc(sizeof(celt_word16)*nfft/2);
    int k;

    clt_mdct_init(&cfg, nfft);
    for (k=0;k<nfft;++k) {
        in[k] = (rand() % 32768) - 16384;
    }

    for (k=0;k<nfft/2;++k) {
       window[k] = Q15ONE;
    }
#ifdef DOUBLE_PRECISION
    for (k=0;k<nfft;++k) {
       in[k] *= 32768;
    }
#endif
    
    if (isinverse)
    {
       for (k=0;k<nfft;++k) {
          in[k] /= nfft;
       }
    }
    
    /*for (k=0;k<nfft;++k) printf("%d %d ", in[k].r, in[k].i);printf("\n");*/
       
    if (isinverse)
    {
       for (k=0;k<nfft;++k)
          out[k] = 0;
          tic();
       clt_mdct_backward(&cfg,in,out, window, nfft/2);
       toc();
       check_inv(in,out,nfft,isinverse);
    } else {
        tic();
       clt_mdct_forward(&cfg,in,out,window, nfft/2);
       toc();
       check(in,out,nfft,isinverse);
    }
    /*for (k=0;k<nfft;++k) printf("%d %d ", out[k].r, out[k].i);printf("\n");*/


    free(in);
    free(out);
    clt_mdct_clear(&cfg);
}

 __interrupt void isrVadc1(void) {}
void MCU_init(void); /* Device initialization function declaration */

int main(int argc,char ** argv)
{
 MCU_init(); /* call Device Initialization */
  SOPT_COPE =0;        /*apago el watchdog*/
  
    ALLOC_STACK;
    if (argc>1) {
        int k;
        for (k=1;k<argc;++k) {
            test1d(atoi(argv[k]),0);
            test1d(atoi(argv[k]),1);
        }
    }else{
        test1d(32,0);
        test1d(32,1);
        test1d(64,0);
        test1d(64,1);
        test1d(128,0);
        test1d(128,1);
        test1d(256,0);
        test1d(256,1);
        test1d(512,0);
        test1d(512,1);
#ifndef RADIX_TWO_ONLY
        test1d(40,0);
        test1d(40,1);
        test1d(56,0);
        test1d(56,1);
        test1d(120,0);
        test1d(120,1);
        test1d(240,0);
        test1d(240,1);
        test1d(480,0);
        test1d(480,1);
#endif
    }
    return ret;
}
