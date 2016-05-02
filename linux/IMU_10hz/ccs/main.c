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
#include "driverlib/timer.h"
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
#include <stdbool.h>



FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
UINT bw;
UINT bwYEI;
BYTEC BuffYEI[500];
FRESULT rc;

#define giroscopo     ":38\n" //VECTOR DEL GIROSCOPO CORREGIDO
#define aceleracion     ":39\n" //VECTOR DEL ACELEROMETRO CORREGIDO
#define magnetometro     ":40\n" //VECTOR DEL MAGNETOMETRO CORREGIDO
#define todos_normalizados     ":37\n" //GIROSCOPO, ACELEROMETRO Y COMPAS CORREGIDOS
#define orientacion     ":10\n" //ORIENTACION CORREGIDA COMO 2 VECTORES

BYTEC nuevo_registro[]="NUEVO REGISTRO\n";


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
int valor_timer;

int
main(void)
{

	//En vez de usar 16MHz, uso el PLL para conseguir 40MHz

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	unsigned long g=SysCtlClockGet();

	FPUEnable();
	FPUStackingEnable();
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC4_U4RX);
	GPIOPinConfigure(GPIO_PC5_U4TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	UARTConfigSetExpClk(UART4_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);

	rc = f_mount(0, &Fatfs);					//registra un area de trabajo
	rc = f_open(&Fil, "BuffGPS.TXT", FA_WRITE | FA_OPEN_ALWAYS);	//abre o crea un archivo
	rc = f_lseek(&Fil, Fil.fsize);
	rc = f_write(&Fil, &nuevo_registro, 15, &bw);
	rc = f_sync(&Fil);
	rc = f_close(&Fil);

	//Configuracion de timer y su interrupcion
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	valor_timer = (SysCtlClockGet()/20);
	TimerLoadSet(TIMER0_BASE, TIMER_A, valor_timer -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);

	//IntPrioritySet(FAULT_SYSTICK, 0x00);
	IntPrioritySet(INT_TIMER0A, 0x00);

	IntMasterEnable();

	while(1);
}



void Timer0IntHandler(void)
{
	int i;
	// Limpia el flag de interrupcion
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

/*	//Escribo el comando en el YEI
	for(i=0; i<sizeof(todos_normalizados); i++){
				UARTCharPut(UART4_BASE, todos_normalizados[i]);}*/

/*	//Escribo el comando en el YEI
	for(i=0; i<sizeof(giroscopo); i++){
				UARTCharPut(UART4_BASE, giroscopo[i]);}*/

	//Escribo el comando en el YEI
	for(i=0; i<sizeof(aceleracion); i++){
				UARTCharPut(UART4_BASE, aceleracion[i]);}

/*	//Escribo el comando en el YEI
	for(i=0; i<sizeof(magnetometro); i++){
				UARTCharPut(UART4_BASE, magnetometro[i]);}*/

	//Escribo el comando en el YEI
		for(i=0; i<sizeof(orientacion); i++){
					UARTCharPut(UART4_BASE, orientacion[i]);}

	cThisChar='0';
	int contador2=0;
	int contador_end_lines=0;

					do{

			    	cThisChar=UARTCharGet(UART4_BASE);
			    	BuffYEI[contador2]=cThisChar;
			    	contador2=contador2+1;
			    		if((cThisChar == '\n'))
			    			contador_end_lines=contador_end_lines+1;

						} while(contador_end_lines != 2);

		rc = f_open(&Fil, "BuffGPS.TXT", FA_WRITE | FA_OPEN_ALWAYS);	//abre o crea un archivo
		rc = f_lseek(&Fil, Fil.fsize);
		rc = f_write(&Fil, &BuffYEI, contador2, &bw);
		rc = f_sync(&Fil);
		rc = f_close(&Fil);

		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2))
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		}
		else
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
		}
}

