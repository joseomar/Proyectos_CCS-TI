#include "inc/lm4f120h5qr.h"
#include <stdio.h>
#include <string.h>

#define UART_PRINTF

#ifdef UART_PRINTF
int fputc(int _c, register FILE *_fp);
int fputs(const char *_ptr, register FILE *_fp);
#endif

void main(void)
{
  unsigned int counter=0;
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  // initialize clock module
  P1DIR |= 0x01;                            // P1.0 output
  UCSCTL3 |= SELREF__REFOCLK;
  UCSCTL4 |= SELA__REFOCLK;

  // initialize Timer_A module
  TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
  TA1CCR0 = 32768;
  TA1CTL = TASSEL__ACLK + MC__UP + TACLR;   // ACLK, up mode, clear TAR

#ifdef UART_PRINTF
  // initialize USCI module
  P5SEL |= BIT6 + BIT7;                     // P5.6,7 = USCI_A1 TXD/RXD
  UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA1CTL1 |= UCSSEL__ACLK;                 // AMCLK
  UCA1BR0 = 27;                             // 32,768kHz 1200 (see User's Guide)
  UCA1BR1 = 0;                              // 32,768kHz 1200
  UCA1MCTL = UCBRS_2;                       // 32,768kHz 1200
  UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
#endif

  while(1)
  {
    __bis_SR_register(LPM3_bits+GIE);       // Enter LPM3, enable interrupts
    printf("Hello world %d!\r\n", counter++);
  }
}

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  P1OUT ^= 0x01;                            // Toggle P1.0
  __bic_SR_register_on_exit(LPM3_bits);
}

#ifdef UART_PRINTF
int fputc(int _c, register FILE *_fp)
{
  while(!(UCA1IFG&UCTXIFG));
  UCA1TXBUF = (unsigned char) _c;

  return((unsigned char)_c);
}

int fputs(const char *_ptr, register FILE *_fp)
{
  unsigned int i, len;

  len = strlen(_ptr);

  for(i=0 ; i<len ; i++)
  {
    while(!(UCA1IFG&UCTXIFG));
    UCA1TXBUF = (unsigned char) _ptr[i];
  }

  return len;
}
#endif
