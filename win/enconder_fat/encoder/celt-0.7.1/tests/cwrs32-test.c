//#ifdef HAVE_CONFIG_H
#include "config.h"
//#endif

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */


#include <stdio.h>
#include <string.h>

//#define FIXED_POINT 1

#undef CELT_C
#include "../libcelt/stack_alloc.h"
#define CELT_C 
//#include "celt_types.h"
//#include "arch.h"
#include "../libcelt/cwrs.c"
/*
#include "../libcelt/rangeenc.c"
#include "../libcelt/rangedec.c"
#include "../libcelt/entenc.c"
#include "../libcelt/entdec.c"
#include "../libcelt/entcode.c"
#include "../libcelt/cwrs.c"
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


static long Big_counter;
#define TPM3_PER_SEC 624994.039501416

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

#define NMAX (10)
#define KMAX 2172 //(32767/1024)

static const int kmax[11]={
 // 32767,32767,32767,32767, 1172,
    2172,238,   95,   53,   36,   27,
     22,   18,   16,   15,   13
};



  __interrupt void isrVadc1(void) {}
void MCU_init(void); /* Device initialization function declaration */

int main(int _argc,char **_argv){
  int n;
  
  MCU_init(); /* call Device Initialization */
  SOPT_COPE =0;        /*apago el watchdog*/
  
  //EnableInterrupts;
  
  ALLOC_STACK;
  for(n=2;n<=NMAX;n++){
    int dk;
    int k;
    dk=kmax[n]>7?kmax[n]/7:1;
    k=1-dk;
    do{
      celt_uint32 uu[KMAX+2U];
      celt_uint32 inc;
      celt_uint32 nc;
      celt_uint32 i;
      k=kmax[n]-dk<k?kmax[n]:k+dk;
      print("Testing CWRS with N=%i, K=%i...\n",n,k);
      nc=ncwrs_urow(n,k,uu);
      inc=nc/10000;
      if(inc<1)inc=1;
      for(i=0;i<nc;i+=inc){
        celt_uint32 u[KMAX+2U];
        int           y[NMAX];
        int           yy[5];
        celt_uint32 v;
        celt_uint32 ii;
        int           kk;
        int           j;
        memcpy(u,uu,(k+2U)*sizeof(*u));
        cwrsi(n,k,i,y,u);
        /*printf("%6u of %u:",i,nc);
        for(j=0;j<n;j++)printf(" %+3i",y[j]);
        printf(" ->");*/
        ii=icwrs(n,k,&v,y,u);
        if(ii!=i){
          print("Combination-index mismatch (%lu!=%lu).\n",
           (long)ii,(long)i);
          return 1;
        }
        if(v!=nc){
          print("Combination count mismatch (%lu!=%lu).\n",
           (long)v,(long)nc);
          return 2;
        }
#ifndef SMALL_FOOTPRINT
        if(n==2){
          cwrsi2(k,i,yy);
          for(j=0;j<2;j++)if(yy[j]!=y[j]){
            print("N=2 pulse vector mismatch ({%i,%i}!={%i,%i}).\n",
             yy[0],yy[1],y[0],y[1]);
            return 3;
          }
          ii=icwrs2(yy,&kk);
          if(ii!=i){
            print("N=2 combination-index mismatch (%lu!=%lu).\n",
             (long)ii,(long)i);
            return 4;
          }
          if(kk!=k){
            print("N=2 pulse count mismatch (%i,%i).\n",kk,k);
            return 5;
          }
          v=ncwrs2(k);
          if(v!=nc){
            print("N=2 combination count mismatch (%lu,%lu).\n",
             (long)v,(long)nc);
            return 6;
          }
        }
        else if(n==3){
          cwrsi3(k,i,yy);
          for(j=0;j<3;j++)if(yy[j]!=y[j]){
            print("N=3 pulse vector mismatch "
             "({%i,%i,%i}!={%i,%i,%i}).\n",yy[0],yy[1],yy[2],y[0],y[1],y[2]);
            return 7;
          }
          ii=icwrs3(yy,&kk);
          if(ii!=i){
            print("N=3 combination-index mismatch (%lu!=%lu).\n",
             (long)ii,(long)i);
            return 8;
          }
          if(kk!=k){
            print("N=3 pulse count mismatch (%i!=%i).\n",kk,k);
            return 9;
          }
          v=ncwrs3(k);
          if(v!=nc){
            print("N=3 combination count mismatch (%lu!=%lu).\n",
             (long)v,(long)nc);
            return 10;
          }
        }
        else if(n==4){
          cwrsi4(k,i,yy);
          for(j=0;j<4;j++)if(yy[j]!=y[j]){
            print("N=4 pulse vector mismatch "
             "({%i,%i,%i,%i}!={%i,%i,%i,%i}.\n",
             yy[0],yy[1],yy[2],yy[3],y[0],y[1],y[2],y[3]);
            return 11;
          }
          ii=icwrs4(yy,&kk);
          if(ii!=i){
            print("N=4 combination-index mismatch (%lu!=%lu).\n",
             (long)ii,(long)i);
            return 12;
          }
          if(kk!=k){
            print("N=4 pulse count mismatch (%i!=%i).\n",kk,k);
            return 13;
          }
          v=ncwrs4(k);
          if(v!=nc){
            print("N=4 combination count mismatch (%lu!=%lu).\n",
             (long)v,(long)nc);
            return 14;
          }
        }
        else if(n==5){
          cwrsi5(k,i,yy);
          for(j=0;j<5;j++)if(yy[j]!=y[j]){
            print("N=5 pulse vector mismatch "
             "({%i,%i,%i,%i,%i}!={%i,%i,%i,%i,%i}).\n",
             yy[0],yy[1],yy[2],yy[3],yy[4],y[0],y[1],y[2],y[3],y[4]);
            return 15;
          }
          ii=icwrs5(yy,&kk);
          if(ii!=i){
            print("N=5 combination-index mismatch (%lu!=%lu).\n",
             (long)ii,(long)i);
            return 16;
          }
          if(kk!=k){
            print("N=5 pulse count mismatch (%i!=%i).\n",kk,k);
            return 17;
          }
          v=ncwrs5(k);
          if(v!=nc){
            print("N=5 combination count mismatch (%lu!=%lu).\n",
             (long)v,(long)nc);
            return 18;
          }
        }
#endif /* SMALL_FOOTPRINT */

        /*printf(" %6u\n",i);*/
      }
      /*printf("\n");*/
    }
    while(k<kmax[n]);
  }
  return 0;
}
