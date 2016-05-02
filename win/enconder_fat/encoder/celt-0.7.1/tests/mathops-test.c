#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */


#include "mathops.h"
#include <stdio.h>
#include <math.h>

#ifdef FIXED_POINT
#define WORD "%d"
#else
#define WORD "%f"
#endif

#ifdef FIXED_DEBUG  
long long celt_mips=0;
#endif
int ret = 0;


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


void testdiv(void)
{
   celt_int32 i;
   for (i=1;i<=327670;i++)
   {
      double prod;
      celt_word32 val;
      val = celt_rcp(i);
#ifdef FIXED_POINT
      prod = (1./32768./65526.)*val*i;
#else
      prod = val*i;
#endif
      if (fabs(prod-1) > .00025)
      {
         print ( "div failed: 1/%d="WORD" (product = %f)\n", i, val, prod);
         ret = 1;
      }
   }
}

void testsqrt(void)
{
   celt_int32 i;
   for (i=1;i<=1000000000;i++)
   {
      double ratio;
      celt_word16 val;
      val = celt_sqrt(i);
      ratio = val/sqrt(i);
      if (fabs(ratio - 1) > .0005 && fabs(val-sqrt(i)) > 2)
      {
         print ("sqrt failed: sqrt(%d)="WORD" (ratio = %f)\n", i, val, ratio);
         ret = 1;
      }
      i+= i>>10;
   }
}

void testrsqrt(void)
{
   celt_int32 i;
   for (i=1;i<=2000000;i++)
   {
      double ratio;
      celt_word16 val;
      val = celt_rsqrt(i);
      ratio = val*sqrt(i)/Q15ONE;
      if (fabs(ratio - 1) > .05)
      {
         print ("rsqrt failed: rsqrt(%d)="WORD" (ratio = %f)\n", i, val, ratio);
         ret = 1;
      }
      i+= i>>10;
   }
}

#ifndef FIXED_POINT
void testlog2(void)
{
   float x;
   for (x=0.001;x<1677700.0;x+=(x/8.0))
   {
      float error = fabs((1.442695040888963387*log(x))-celt_log2(x));
      if (error>0.0009)
      {
         print ("celt_log2 failed: fabs((1.442695040888963387*log(x))-celt_log2(x))>0.001 (x = %f, error = %f)\n", x,error);
         ret = 1;    
      }
   }
}

void testexp2(void)
{
   float x;
   for (x=-11.0;x<24.0;x+=0.0007)
   {
      float error = fabs(x-(1.442695040888963387*log(celt_exp2(x))));
      if (error>0.0002)
      {
         print ("celt_exp2 failed: fabs(x-(1.442695040888963387*log(celt_exp2(x))))>0.0005 (x = %f, error = %f)\n", x,error);
         ret = 1;    
      }
   }
}

void testexp2log2(void)
{
   float x;
   for (x=-11.0;x<24.0;x+=0.0007)
   {
      float error = fabs(x-(celt_log2(celt_exp2(x))));
      if (error>0.001)
      {
         print ( "celt_log2/celt_exp2 failed: fabs(x-(celt_log2(celt_exp2(x))))>0.001 (x = %f, error = %f)\n", x,error);
         ret = 1;    
      }
   }
}
#else
void testlog2(void)
{
   celt_word32 x;
   for (x=8;x<1073741824;x+=(x>>3))
   {
      float error = fabs((1.442695040888963387*log(x/16384.0))-celt_log2(x)/256.0);
      if (error>0.003)
      {
         print ("celt_log2 failed: x = %ld, error = %f\n", (long)x,error);
         ret = 1;
      }
   }
}

void testexp2(void)
{
   celt_word16 x;
   for (x=-32768;x<-30720;x++)
   {
      float error1 = fabs(x/2048.0-(1.442695040888963387*log(celt_exp2(x)/65536.0)));
      float error2 = fabs(exp(0.6931471805599453094*x/2048.0)-celt_exp2(x)/65536.0);
      if (error1>0.0002&&error2>0.00004)
      {
         print ("celt_exp2 failed: x = "WORD", error1 = %f, error2 = %f\n", x,error1,error2);
         ret = 1;
      }
   }
}

void testexp2log2(void)
{
   celt_word32 x;
   for (x=8;x<65536;x+=(x>>3))
   {
      float error = fabs(x-0.25*celt_exp2(celt_log2(x)<<3))/16384;
      if (error>0.004)
      {
         print( "celt_log2/celt_exp2 failed: fabs(x-(celt_log2(celt_exp2(x))))>0.001 (x = %ld, error = %f)\n", (long)x,error);
         ret = 1;
      }
   }
}

void testilog2(void)
{
   celt_word32 x;
   for (x=1;x<=268435455;x+=127)
   {
      celt_word32 error = abs(celt_ilog2(x)-(int)floor(log2(x)));
      if (error!=0)
      {
         print("celt_ilog2 failed: celt_ilog2(x)!=floor(log2(x)) (x = %d, error = %d)\n",x,error);
         ret = 1;
      }
   }
}
#endif



  __interrupt void isrVadc1(void) {}
void MCU_init(void); /* Device initialization function declaration */

int main(void)
{
MCU_init(); /* call Device Initialization */
  SOPT_COPE =0;        /*apago el watchdog*/
  
   testdiv();
   testsqrt();
   testrsqrt();
   testlog2();
   testexp2();
   testexp2log2();
#ifdef FIXED_POINT
   testilog2();
#endif
   return ret;
}
