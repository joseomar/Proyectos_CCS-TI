#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
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
//char mensaje[80];
secs = (double) (Big_counter*0xFF+TPM3CNT) / (double)TPM3_PER_SEC;
    print("Elapsed seconds = %f \n", secs);
   // SCI_send_string(mensaje);

}

#include "celt_types.h"
#include <stdio.h>



  __interrupt void isrVadc1(void) {}
void MCU_init(void); /* Device initialization function declaration */



int main(void)
{
MCU_init(); /* call Device Initialization */
  SOPT_COPE =0;        /*apago el watchdog*/
   celt_int16 i = 1;
   i <<= 14;
   if (i>>14 != 1)
   {
      print("celt_int16 isn't 16 bits\n");
      return 1;
   }
   if (sizeof(celt_int16)*2 != sizeof(celt_int32))
   {
      print( "16*2 != 32\n");
      return 1;
   }
   return 0;
}
