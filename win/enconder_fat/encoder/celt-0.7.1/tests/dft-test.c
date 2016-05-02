#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
//#define _EWL_CLOCK_T_AVAILABLE 1
//#define  _EWL_OS_TIME_SUPPORT

#include <time.h>
#include <time_api.h>

#include <ctime>


#include <stdio.h>
#include "kiss_fft.h"





#include "../libcelt/stack_alloc.h"
//#include "../libcelt/kiss_fft.c"

//#include "../libcelt/entcode.c"

#define CELT_C
#define RADIX_TWO_ONLY 


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

#ifndef M_PI
#define M_PI 3.141592653
#endif

#define TPM3_PER_SEC 624994.039501416
                                  
#ifdef FIXED_DEBUG  
long long celt_mips=0;
#endif
int ret = 0;

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

void check(kiss_fft_cpx  * in,kiss_fft_cpx  * out,int nfft,int isinverse)
{
    int bin,k;
    double errpow=0,sigpow=0, snr;
      char mensaje[256];
    
    for (bin=0;bin<nfft;++bin) {
        double ansr = 0;
        double ansi = 0;
        double difr;
        double difi;

        for (k=0;k<nfft;++k) {
            double phase = -2*M_PI*bin*k/nfft;
            double re = cos(phase);
            double im = sin(phase);
            if (isinverse)
                im = -im;

            if (!isinverse)
            {
               re /= nfft;
               im /= nfft;
            }

            ansr += in[k].r * re - in[k].i * im;
            ansi += in[k].r * im + in[k].i * re;
        }
        /*printf ("%d %d ", (int)ansr, (int)ansi);*/
        difr = ansr - out[bin].r;
        difi = ansi - out[bin].i;
        errpow += difr*difr + difi*difi;
        sigpow += ansr*ansr+ansi*ansi;
    }
    snr = 10*log10(sigpow/errpow);
    print("nfft=%d inverse=%d,snr = %f\n",nfft,isinverse,snr );
    //SCI_send_string(mensaje);
    if (snr<60) {
       print("** poor snr: %f ** \n", snr);
       //SCI_send_string(mensaje);
       ret = 1;
    }
}

void test1d(int nfft,int isinverse)
{
    char mensaje [128];
    clock_t start;
    clock_t end;
    

    size_t buflen = sizeof(kiss_fft_cpx)*nfft;

    kiss_fft_cpx  * in = (kiss_fft_cpx*)malloc(buflen);
    kiss_fft_cpx  * out= (kiss_fft_cpx*)malloc(buflen);
    kiss_fft_cfg  cfg = kiss_fft_alloc(nfft,0,0);
    int k;

    for (k=0;k<nfft;++k) {
        in[k].r = (rand() % 32767) - 16384;
        in[k].i = (rand() % 32767) - 16384;
    }

#ifdef DOUBLE_PRECISION
    for (k=0;k<nfft;++k) {
       in[k].r *= 32768;
       in[k].i *= 32768;
    }
#endif
    
    if (isinverse)
    {
       for (k=0;k<nfft;++k) {
          in[k].r /= nfft;
          in[k].i /= nfft;
       }
    }
    
    /*for (k=0;k<nfft;++k) printf("%d %d ", in[k].r, in[k].i);printf("\n");*/
    //start=TPM3CNT;   //tic
    tic();
    
    if (isinverse)
       kiss_ifft(cfg,in,out);
    else
       kiss_fft(cfg,in,out);
    
    toc();
    //end =TPM3CNT;    //toc

    /*for (k=0;k<nfft;++k) printf("%d %d ", out[k].r, out[k].i);printf("\n");*/

    

    check(in,out,nfft,isinverse);

    free(in);
    free(out);
    free(cfg);
}



// end of isrVtpm3ovf 
  __interrupt void isrVadc1(void) {}
void MCU_init(void); /* Device initialization function declaration */

int main(int argc,char ** argv)
{
MCU_init(); /* call Device Initialization */
  SOPT_COPE =0;        /*apago el watchdog*/
  
  EnableInterrupts;

    //tic();
    ALLOC_STACK;
    while(1){
      
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
    #ifndef RADIX_TWO_ONLY
            test1d(36,0);
            test1d(36,1);
            test1d(50,0);
            test1d(50,1);
            test1d(120,0);
            test1d(120,1);
            test1d(105,0);
            test1d(105,1);
    #endif
      }
    }
    return ret;
}


