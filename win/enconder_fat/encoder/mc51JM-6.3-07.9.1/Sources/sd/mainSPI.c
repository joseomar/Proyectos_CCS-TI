#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

void mainSPI(void) {
  CPU_Init();
  EnableInterrupts; /* enable interrupts */
  /* include your code here */
  
  SPI_Init();
    
  for(;;) {
    SPI_Send_byte(0x66);
    __RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}