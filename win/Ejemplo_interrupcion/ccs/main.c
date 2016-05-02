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

FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
UINT bw;
BYTEC Buff[];
int contador;


int main(void)
{
	//FRESULT rc;

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL |SYSCTL_XTAL_16MHZ| SYSCTL_OSC_MAIN );

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);

	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
	                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	                         UART_CONFIG_PAR_NONE));

	//rc = f_mount(0, &Fatfs);
	//rc = f_open(&Fil, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);

	UARTFIFOLevelSet(UART1_BASE,UART_FIFO_TX7_8,UART_FIFO_RX7_8); //fijo los umbrales para la ocurrencia de la interrupcion
	//UARTTxIntModeSet(UART1_BASE, UART_TXINT_MODE_FIFO);

	// Habilito la interrupcion
	//IntRegister(UART1_BASE, UARTinterrupcion);
	//UARTIntRegister(UART1_BASE, UARTinterrupcion); //registra un manejador de interrupciones para la interrupcion de la UART
	UARTIntClear(UART1_BASE, UART_INT_TX | UART_INT_RX); //limpia los flags de interrupcion
	UARTIntEnable(UART1_BASE, UART_INT_RX);
	UARTFIFOEnable(UART1_BASE);
	IntMasterEnable();
	UARTEnable(UART1_BASE); //habilita transmision y recepcion
	IntEnable(INT_UART1);  //habilita las fuentes de interrupcion de la UART

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

    //if(RIGHT_BUTTON){}

    if(ulStatus && UART_INT_RX)
    {
     while(m_nTxBuffIn1>0 && UARTCharsAvail(UART1_BASE))
     {
               //
               // Lee el proximo caracter de la FIFO de recepcion.
               //
     cThisChar = UARTCharGetNonBlocking(UART1_BASE);

     UARTCharPut(UART0_BASE, cThisChar);

     //rc = f_write(&Fil, "Hello world!\r\n", 14, &bw);

     m_nTxBuffIn1--;

     n=n+1;
     }

    }

}
