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
#include "driverlib/fatFs_3/ff.h"
#include "driverlib/fatFs_3/ffconf.h"
#include "driverlib/fatFs_3/integer.h"
#include <stdio.h>
#include "drivers/buttons.h"
#include <string.h>


unsigned int n,m;
//Sin DGPS
char buferA[]={'$','P','M','T','K','3','0','1',',','0','*','2','C','\r','\n'};
//Comando para programar el WAAS
char buferB[]={'$','P','M','T','K','3','0','1',',','2','*','2','E','\r','\n'};
char buferC[]={'$','P','M','T','K','4','0','1','*','3','7','\r','\n'};

int main(void)
{
	char cThisChar;

	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL |SYSCTL_XTAL_16MHZ| SYSCTL_OSC_MAIN );

	//Para el SW1
	/*SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIODirModeSet(GPIO_PORTF_BASE, 0xFF, GPIO_DIR_MODE_IN);
	GPIOPinIntEnable(GPIO_PORTB_BASE, 0xFF);
	IntEnable(INT_GPIOF);*/

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
	                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	                         UART_CONFIG_PAR_NONE));

	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	UARTFIFOLevelSet(UART1_BASE,UART_FIFO_TX7_8,UART_FIFO_RX7_8);
	UARTIntClear(UART1_BASE, UART_INT_TX | UART_INT_RX);
	UARTIntEnable(UART1_BASE, UART_INT_RX);
	UARTFIFOEnable(UART1_BASE);
	IntMasterEnable();
	UARTEnable(UART1_BASE);
	IntEnable(INT_UART1);

		for(n=0; n<sizeof(buferA); n++){
					UARTCharPut(UART1_BASE, buferA[n]);}

		for(m=0; m<sizeof(buferC); m++){
						UARTCharPut(UART1_BASE, buferC[m]);}

	while(1){;}
}

void UARTinterrupcion(void)
{
	char cThisChar;
			int n=0;
			int m_nTxBuffIn1 = 16;
		    unsigned long ulStatus;

		    //
		    // Obtengo el estado de la interrupcion
		    //
		    ulStatus = UARTIntStatus(UART1_BASE, true);
		    //
		    // Limpio los flags de interrupcion
		    //
		    UARTIntClear(UART1_BASE, ulStatus);

		    if(RIGHT_BUTTON){;}

		    if(ulStatus && UART_INT_RX){

		    }
		     while(m_nTxBuffIn1>0 && UARTCharsAvail(UART1_BASE))
		     {
		               //
		               // Lee el proximo caracter de la FIFO de recepcion.
		               //
		     cThisChar = UARTCharGetNonBlocking(UART1_BASE);

		     UARTCharPut(UART0_BASE, cThisChar);

		     m_nTxBuffIn1--;

		     n=n+1;
    }
}

void GPIOinterrupcion(void){
	while(1)
			     {

			     UARTCharPut(UART0_BASE, '0');

	    }
	     }

