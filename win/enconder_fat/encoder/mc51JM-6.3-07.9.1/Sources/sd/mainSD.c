#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "CPU.h"
#include "SPI.h"

void mainSD(void) {
  char buffer[512];
  int i;
      
  CPU_Init();
  EnableInterrupts; /* enable interrupts */
  /* include your code here */
 
  
  SD_Init();
  
  
  for(i=0;i<512;i++)
    buffer[i]=0;
  
  
  SD_Read_Block(0,buffer);


  for(;;) {
    __RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}