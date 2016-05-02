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

//Aca trato de no usar FIFOs de recepcion, sino solo interrupciones simples, para ello entre otras funciones stellaris uso: UARTFIFODisable()


FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
UINT bw;
UINT bwGPS,bwYEI;
BYTEC BuffGPS[];
BYTEC BuffYEI[];
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
int entradas_int_YEI=0;
int contadorGPS=0;
int contadorYEI=0;
unsigned long valor_timer;
char cThisChar='a';
char distinto_YEI='a';
bool fin_mensajeNMEA = false;

int
main(void)
{


	unsigned long ulResetCause;

	//En vez de usar 16MHz, uso el PLL para conseguir 40MHz

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	unsigned long g=SysCtlClockGet();

	FPUEnable();
	FPULazyStackingEnable();
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	ulResetCause = SysCtlResetCauseGet();
	SysCtlResetCauseClear(ulResetCause);
	HibernateEnableExpClk(SysCtlClockGet());
	ButtonsInit();

	//Configurando el Systick
	SysTickPeriodSet(SysCtlClockGet() / APP_SYSTICKS_PER_SEC);
	SysTickEnable();
	SysTickIntEnable();

	//Configurando la UART4 para el YEI
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC4_U4RX);
	GPIOPinConfigure(GPIO_PC5_U4TX);
	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	UARTConfigSetExpClk(UART4_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	//Configurando la UART1 para el GPS
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						UART_CONFIG_PAR_NONE));

	//Configuracion de timer y su interrupcion
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	valor_timer = (SysCtlClockGet() / 10) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, valor_timer -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER0_BASE, TIMER_A);


	//Configuracion de interrupcion de la UART4 para el YEI
	UARTIntClear(UART4_BASE, UART_INT_TX | UART_INT_RX); //limpia los flags de interrupcion
	UARTIntEnable(UART4_BASE, UART_INT_RX);
	UARTFIFODisable(UART4_BASE); //Desabilito la FIFO para evitar posibles problemas de overflow y grandes latencias generadas por el tamaño de la FIFO
	UARTEnable(UART4_BASE); //habilita transmision y recepcion
	IntEnable(INT_UART4);  //habilita las fuentes de interrupcion de la UART

	//Apago el LED de cierre de SD...
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);

	//Se monta la SD y se crea un archivo
	rc = f_mount(0, &Fatfs);					//registra un area de trabajo
	rc = f_open(&Fil, "BuffGPS.TXT", FA_WRITE | FA_CREATE_ALWAYS);	//abre o crea un archivo

	//Configuracion de interrupcion de la UART1 para el GPS
	UARTIntClear(UART1_BASE, UART_INT_TX | UART_INT_RX); //limpia los flags de interrupcion
	UARTIntEnable(UART1_BASE, UART_INT_RX);
	UARTFIFODisable(UART1_BASE); //Desabilito la FIFO para evitar posibles problemas de overflow y grandes latencias generadas por el tamaño de la FIFO
	UARTEnable(UART1_BASE); //habilita transmision y recepcion
	IntEnable(INT_UART1);  //habilita las fuentes de interrupcion de la UART


	//Configuracion de prioridades. La interrupcion mas importante es la de mas bajo valor(INT_UART1).
	IntPrioritySet(FAULT_SYSTICK, 0x00);
	IntPrioritySet(INT_TIMER0A, 0x20);
    IntPrioritySet(INT_UART4, 0x40);
    IntPrioritySet(INT_UART1, 0x80);

    IntMasterEnable();

    while(1);
}

void
AppButtonHandler(void)
{
    //
    // Si ambos botones son presionados, se cierra la memoria
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
	entradas_int_YEI=entradas_int_YEI+1;

	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	for(i=0; i<sizeof(cuaternion); i++){

				UARTCharPut(UART4_BASE, cuaternion[i]);}

}

void UARTinterrupcionGPS(void)
{
    unsigned long ulStatusGPS;

    ulStatusGPS = UARTIntStatus(UART1_BASE, true);

    UARTIntClear(UART1_BASE, ulStatusGPS);

    cThisChar = UARTCharGet(UART1_BASE);

    BuffGPS[contadorGPS] = cThisChar;

    contadorGPS=contadorGPS+1;

    	 if(cThisChar=='*'){
    		 fin_mensajeNMEA=true;
    		 rc = f_write(&Fil, &BuffGPS, contadorGPS, &bw);
    		//La funcion f_sync() realiza algo parecido a f_close(), pero sin cerrar del todo la SD, permitiendo acciones de read/write
    		//Esto se debe a que se intenta minimizar el tiempo de riesgo de perdida de datos, que existe al mantener abierta la memoria
    		 rc = f_sync(&Fil);
    		 contadorGPS=0; }
}


void UARTinterrupcionYEI(void){

	unsigned long ulStatusYEI;

	ulStatusYEI = UARTIntStatus(UART4_BASE, true);

	UARTIntClear(UART4_BASE, ulStatusYEI);

	distinto_YEI=UARTCharGet(UART4_BASE);

	BuffYEI[contadorYEI]=distinto_YEI;

	contadorYEI=contadorYEI+1;

	if(entradas_int_YEI == 5){
		rc = f_write(&Fil, &BuffYEI, contadorYEI, &bw);
		rc = f_sync(&Fil);
		contadorYEI=0;
		entradas_int_YEI=0;}

}
