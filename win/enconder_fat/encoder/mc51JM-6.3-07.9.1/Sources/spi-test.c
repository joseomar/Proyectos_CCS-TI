

  #include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */


#ifdef __cplusplus
 extern "C"
#endif



void MCU_init(void); /* Device initialization function declaration */

 void SCI_send_string(char * mensaje); /* Envia texto por SCI2 */


 void isrVadc1(void)
{
}


__interrupt void isrVftm1ovf(void)
{
  /* Write your interrupt code here ... */
  
}

char KBI=0;

__interrupt void isrVirq(void)
{
  /* Write your interrupt code here ... */
        IRQSC_IRQACK=1;
          KBI=1;
}
/* end of isrVirq */



#include <stdarg.h>
 void print_c( char * fmt, ... )
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

 void SCI_send_string(char *mensaje){
 unsigned long tam =strlen(mensaje);
 int i;
 for ( i=0;i<tam;i++){
    SCI2C2_TE=1; //habilito la transmicion
    SCI2D= mensaje[i];     //escribo el dato 
    while (SCI2S1_TDRE==0){//espero que se envie el dato
    }    
    //SCI2C2_TE=0;
        
 }
}




int main(void) {
  
  MCU_init(); /* call Device Initialization */
  //SOPT_COPE =0;        /*apago el watchdog*/
  /* include your code here */
   for(;;){};
  
}



