#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/fatFs_3/diskio.h"
//#include "driverlib/fatFs_3/fatfs.h"
#include "driverlib/fatFs_3/ff.h"
#include "driverlib/fatFs_3/ffconf.h"
#include "driverlib/fatFs_3/integer.h"
#include <stdio.h>
#include <string.h>


FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
UINT bw;
BYTEC Buff[];
int contador;


int
main(void)
{
	char cThisChar;
	FRESULT rc;				/* Result code */

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);

	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						UART_CONFIG_PAR_NONE));

	UARTFIFOLevelSet(UART1_BASE,UART_FIFO_TX7_8,UART_FIFO_RX7_8);
	UARTTxIntModeSet(UART1_BASE, UART_TXINT_MODE_FIFO) ;

	// Habilito la interrupcion
	UARTIntRegister(UART1_BASE,UART1IntHandler);
	UARTIntClear(UART1_BASE, UART_INT_TX | UART_INT_RX);
	UARTEnable(UART1_BASE);
	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_TX);


	rc = f_mount(0, &Fatfs);					//registra un area de trabajo
	rc = f_open(&Fil, "Buff.TXT", FA_WRITE | FA_CREATE_ALWAYS);	//abre o crea un archivo


	cThisChar = UARTCharGet(UART1_BASE);

	rc = f_write(&Fil, Buff, contador, &bw);

	rc = f_close(&Fil);

	return(0);
}



void UART1IntHandler()
{
    unsigned long ulStatus;

    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(UART1_BASE, true);
    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART1_BASE, ulStatus);

    if(ulStatus & UART_INT_TX)
    {
     // TX int
     // Push next char to transmitter
     bRc = true;
     while((m_nTxBuffIn1 > 0) && (bRc == true) && UARTSpaceAvail(UART1_BASE))
     {
               //
               // Write the next character into the transmit FIFO.
               //
      bRc = UARTCharPutNonBlocking(UART1_BASE,m_tTxBuff1[m_nTxNextNdx1]);
       if(bRc == true)
      {
       m_nTxNextNdx1++;
       m_nTxBuffIn1--;
      }
     }

    }
    else if(ulStatus & UART_INT_RX || ulStatus & UART_INT_RT)
    {
     // RX int
     // Read all RX fifo data
     while(UARTCharsAvail(UART1_BASE))
     {
      // Read the next character from the UART
      // (and write it back to the UART) ?
      OmapMessageLoop(UART1_BASE,1);
     }
    }

}
