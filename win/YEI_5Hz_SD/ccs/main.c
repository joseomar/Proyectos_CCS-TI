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

//Aca se implementa casi lo mismo que en guarda_GPSyYEI, pero se busca generar una interrupcion de 5Hz, a traves de un timer,
//para el refresco de los datos del YEI.


FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
UINT bw;
UINT bwGPS,bwYEI;
BYTEC BuffGPS[];
BYTEC BuffYEI[];
BYTEC Segundo[]="------------------------------";
FRESULT rc;

#define cuaternion     ":0\n" //orientacion como cuaternion

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
int contador_end_lines=0;
int contadorGPS=0;
int contadorYEI=0;
int contador_int_TIMER=0;
unsigned long valor_timer;
char cThisChar;
char distinto_YEI='z';
bool fin_mensajeNMEA = false;

int
main(void)
{


	unsigned long ulResetCause;

	//En vez de usar 16MHz, uso el PLL para conseguir 40MHz

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	//SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
				//SYSCTL_XTAL_16MHZ);
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


	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC4_U4RX);
	GPIOPinConfigure(GPIO_PC5_U4TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	UARTConfigSetExpClk(UART4_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));


	//Configuracion de timer y su interrupcion
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	valor_timer = (SysCtlClockGet() / 10) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, valor_timer -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);

	/*//Configuracion de interrupcion de la UART4 para el YEI
	UARTFIFOLevelSet(UART4_BASE,UART_FIFO_TX7_8,UART_FIFO_RX7_8); //fijo los umbrales para la ocurrencia de la interrupcion
	UARTIntClear(UART4_BASE, UART_INT_TX | UART_INT_RX); //limpia los flags de interrupcion
	UARTIntEnable(UART4_BASE, UART_INT_RX);
	UARTFIFOEnable(UART4_BASE);
	IntMasterEnable();
	UARTEnable(UART4_BASE); //habilita transmision y recepcion
	IntEnable(INT_UART4);  //habilita las fuentes de interrupcion de la UART*/

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);

	rc = f_mount(0, &Fatfs);					//registra un area de trabajo
	rc = f_open(&Fil, "BuffGPS.TXT", FA_WRITE | FA_CREATE_ALWAYS);	//abre o crea un archivo

	IntPrioritySet(FAULT_SYSTICK, 0x00);
	IntPrioritySet(INT_TIMER0A, 0x20);

	IntMasterEnable();

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

void Timer0IntHandler(void)
{
	// Limpia el flag de interrupcion
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	//Cuenta las veces que se ingresa en la interrupcion
	contador_int_TIMER=contador_int_TIMER+1;

	//Escribo el comando en el YEI
	for(i=0; i<sizeof(cuaternion); i++){
				UARTCharPut(UART4_BASE, cuaternion[i]);}

	unsigned int cont_coma=0;
	unsigned int cont_caracter_despues_coma=0;

	bool fin_linea=false;

		while(fin_linea != true && UARTCharsAvail(UART4_BASE)){

						distinto_YEI=UARTCharGetNonBlocking(UART4_BASE);
					    	BuffYEI[contadorYEI]=distinto_YEI;
					    	contadorYEI=contadorYEI+1;

					    	if(distinto_YEI==','){
					    	cont_coma=cont_coma+1;
					    		if(cont_coma==3){
					    		cont_caracter_despues_coma=cont_caracter_despues_coma+1;
					    			if(cont_caracter_despues_coma==4){
					    				fin_linea=true;
					    				cont_caracter_despues_coma=0;
					    				cont_coma=0;
					    			}}}
					    }


		rc = f_write(&Fil, &BuffYEI, contadorYEI, &bw);
		contadorYEI=0;
		distinto_YEI = 'z';

		//Si entro 10 veces en la interrupcion, es porque se cumplio 1[s]

		/*if(contador_int_TIMER == 10){
						rc = f_write(&Fil, &BuffYEI, sizeof(BuffYEI), &bw);
						contador_int_TIMER=0;
									}*/
}

