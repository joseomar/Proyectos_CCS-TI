#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_hibernate.h"
#include "driverlib/ssi.h"
#include "driverlib/fpu.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/hibernate.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/fatFs_3/diskio.h"
#include "driverlib/fatFs_3/ff.h"
#include "driverlib/fatFs_3/ffconf.h"
#include "driverlib/fatFs_3/integer.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "drivers/rgb.h"
#include "drivers/buttons.h"
#include "drivers/qs-rgb.h"
#include <stdio.h>
#include <string.h>
#include <math.h>


FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
UINT bw;
BYTEC Buff[];
FRESULT rc;

volatile sAppState_t g_sAppState;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

char cThisChar;

int
main(void)
{
			/* Result code */

	unsigned long ulResetCause;

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

	    FPUEnable();
	    FPUStackingEnable();
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	    ulResetCause = SysCtlResetCauseGet();
	    SysCtlResetCauseClear(ulResetCause);
	    HibernateEnableExpClk(SysCtlClockGet());
	    ButtonsInit();
	    SysTickPeriodSet(SysCtlClockGet() / APP_SYSTICKS_PER_SEC);
	    SysTickEnable();
	    SysTickIntEnable();
	    IntMasterEnable();



	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);

	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						UART_CONFIG_PAR_NONE));

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);

	rc = f_mount(0, &Fatfs);					//registra un area de trabajo
	rc = f_open(&Fil, "Buff.TXT", FA_WRITE | FA_CREATE_ALWAYS);	//abre o crea un archivo

	//Configuracion de interrupcion de la UART1 para el GPS
		UARTFIFOLevelSet(UART1_BASE,UART_FIFO_TX7_8,UART_FIFO_RX7_8); //fijo los umbrales para la ocurrencia de la interrupcion
		UARTIntClear(UART1_BASE, UART_INT_TX | UART_INT_RX); //limpia los flags de interrupcion
		UARTIntEnable(UART1_BASE, UART_INT_RX);
		UARTFIFOEnable(UART1_BASE);
		IntMasterEnable();
		UARTEnable(UART1_BASE); //habilita transmision y recepcion
		IntEnable(INT_UART1);  //habilita las fuentes de interrupcion de la UART

	while(1);
}

void
AppButtonHandler(void)
{
    //
    // Switch statement to adjust the color wheel position based on buttons
    //
    switch(g_sAppState.ulButtons & ALL_BUTTONS)
    {

    case ALL_BUTTONS:{
    	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);

    	rc = f_close(&Fil);

    }
        }
}

void
SysTickIntHandler(void)
{


    g_sAppState.ulButtons = ButtonsPoll(0,0);
    AppButtonHandler();



}

void interrupcionUART1(void){

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

			    if(ulStatus && UART_INT_RX){

			    }
			     while(m_nTxBuffIn1>0 && UARTCharsAvail(UART1_BASE))
			     {
			               //
			               // Lee el proximo caracter de la FIFO de recepcion.
			               //
			     cThisChar = UARTCharGetNonBlocking(UART1_BASE);

			     Buff[n]=cThisChar;

			     m_nTxBuffIn1--;

			     n=n+1;
	    }

			    rc = f_write(&Fil, &Buff, n, &bw);

}

