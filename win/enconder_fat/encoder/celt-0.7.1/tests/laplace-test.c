#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#include <stdio.h>
#include <stdlib.h>
#include "laplace.h"

#include "arch.h"
#include "../libcelt/stack_alloc.h"
#define CELT_C 
/*
#include "../libcelt/rangeenc.c"
#include "../libcelt/rangedec.c"
#include "../libcelt/entenc.c"
#include "../libcelt/entdec.c"
#include "../libcelt/entcode.c"
#include "../libcelt/laplace.c"
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

#define DATA_SIZE 1000
#define VAR_SIZE 500


  __interrupt void isrVadc1(void) {}
void MCU_init(void); /* Device initialization function declaration */



int main(void)
{
MCU_init(); /* call Device Initialization */
  SOPT_COPE =0;        /*apago el watchdog*/
  
   int i;
   int ret = 0;
   ec_enc enc;
   ec_dec dec;
   ec_byte_buffer buf;
   unsigned char *ptr;
   int val[VAR_SIZE], decay[VAR_SIZE];
   
   ALLOC_STACK;
   ptr = malloc(DATA_SIZE);
   ec_byte_writeinit_buffer(&buf, ptr, DATA_SIZE);
   //ec_byte_writeinit(&buf);
   ec_enc_init(&enc,&buf);
   
   val[0] = 3; decay[0] = 6000;
   val[1] = 0; decay[1] = 5800;
   val[2] = -1; decay[2] = 5600;
   for (i=3;i<VAR_SIZE;i++)
   {
      val[i] = rand()%15-7;
      decay[i] = rand()%11000+5000;
   }
   for (i=0;i<VAR_SIZE;i++)
      ec_laplace_encode(&enc, &val[i], decay[i]);      
      
   ec_enc_done(&enc);

   ec_byte_readinit(&buf,ec_byte_get_buffer(&buf),ec_byte_bytes(&buf));
   ec_dec_init(&dec,&buf);

   for (i=0;i<VAR_SIZE;i++)
   {
      int d = ec_laplace_decode(&dec, decay[i]);
      if (d != val[i])
      {
         print ("Got %d instead of %d\n", d, val[i]);
         ret = 1;
      }
   }
   
   return ret;
}
