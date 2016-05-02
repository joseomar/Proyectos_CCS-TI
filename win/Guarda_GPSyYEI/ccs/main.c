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
UINT bwGPS,bwYEI;
BYTEC BuffGPS[];
BYTEC BuffYEI[];
FRESULT rc;

#define cuaternion     ":0\n" //orientacion como cuaternion

int cont_gen;
int cont_genYEI;

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

int i;

int
main(void)
{
	char cThisChar;
			/* Result code */

	unsigned long ulResetCause;

	//SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
				SYSCTL_XTAL_16MHZ);
	unsigned long g=SysCtlClockGet();

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

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC4_U4RX);
	GPIOPinConfigure(GPIO_PC5_U4TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	UARTConfigSetExpClk(UART4_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);

	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						UART_CONFIG_PAR_NONE));

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);

	rc = f_mount(0, &Fatfs);					//registra un area de trabajo
	rc = f_open(&Fil, "BuffGPS.TXT", FA_WRITE | FA_CREATE_ALWAYS);	//abre o crea un archivo

	do {


    int contador1=0;
    //int contador2=0;

    while(UARTCharsAvail(UART1_BASE)){

    	cThisChar=UARTCharGetNonBlocking(UART1_BASE);
		BuffGPS[contador1]=cThisChar;
		contador1=contador1+1;

    }
		//rc = f_write(&Fil, &BuffGPS, contador1, &bwGPS);
		//contador1=0;

		//SysCtlDelay(15000000);

		for(i=0; i<sizeof(cuaternion); i++){
			UARTCharPut(UART4_BASE, cuaternion[i]);}

		while(UARTCharsAvail(UART4_BASE)){

		    	cThisChar=UARTCharGetNonBlocking(UART4_BASE);
		    	BuffGPS[contador1]=cThisChar;
				contador1=contador1+1;

		    }

		rc = f_write(&Fil, &BuffGPS, contador1, &bwGPS);
		contador1=0;

		while(UARTCharsAvail(UART1_BASE)==false){;}
	}

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

