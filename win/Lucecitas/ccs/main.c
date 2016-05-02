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
BYTEC BuffGPS[500];
BYTEC BuffYEI[500];
FRESULT rc;

#define aceleracion     ":39\n" //VECTOR DEL ACELEROMETRO CORREGIDO
#define orientacion     ":10\n" //ORIENTACION CORREGIDA COMO 2 VECTORES
int i,j,k,l,m,n;

#define verde     ":238,0,1,0\n"
#define rojo     ":238,1,0,0\n"


//Sin DGPS
char buferC[]={'$','P','M','T','K','3','0','1',',','0','*','2','C','\r','\n'};
//Comando para programar el WAAS
char buferB[]={'$','P','M','T','K','3','0','1',',','2','*','2','E','\r','\n'};
//Comando solo para dejar el PRMC activo
//$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C<CR><LF>
char buferA[]={'$','P','M','T','K','3','1','4',',','0',',','1',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0','*','2','9','\r','\n'};

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

int
main(void)
{
	char cThisChar;
			/* Result code */

	unsigned long ulResetCause;

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


	//Ordeno al GPS a que active el sistema WAAS
	for(n=0; n<sizeof(buferC); n++){
				UARTCharPut(UART1_BASE, buferC[n]);}


	//Ordeno al GPS a que me devuelva solo un mensaje..
	for(i=0; i<sizeof(buferA); i++){
			UARTCharPut(UART1_BASE, buferA[i]);}

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);

	for(k=0; k<sizeof(rojo); k++){
		UARTCharPut(UART4_BASE, rojo[k]);}


	rc = f_mount(0, &Fatfs);					//registra un area de trabajo


	do {


    int contador1=0;
    int contador2=0;
    cThisChar='0';

    	do{

    	cThisChar=UARTCharGet(UART1_BASE);
		BuffGPS[contador1]=cThisChar;
		contador1=contador1+1;

    	} while((cThisChar != '\n'));

    	cThisChar='0';

      	for(j=1; j<contador1; j++){

    		if(BuffGPS[j-1]==',' && BuffGPS[j]=='A'){

    			rc = f_open(&Fil, "BuffGPS.TXT", FA_WRITE | FA_OPEN_ALWAYS);	//abre o crea un archivo
    			rc = f_lseek(&Fil, Fil.fsize);
    			rc = f_write(&Fil, &BuffGPS, contador1, &bwGPS);
    			rc = f_sync(&Fil);
    			rc = f_close(&Fil);

    			contador1=0;

    			for(l=0; l<sizeof(verde); l++){
    					UARTCharPut(UART4_BASE, verde[l]);}

    			//Escribo el comando en el YEI
    			for(i=0; i<sizeof(aceleracion); i++){
    					UARTCharPut(UART4_BASE, aceleracion[i]);}


    			//Escribo el comando en el YEI
    			for(i=0; i<sizeof(orientacion); i++){
    					UARTCharPut(UART4_BASE, orientacion[i]);}


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
				rc = f_write(&Fil, &BuffYEI, contador2, &bwYEI);
				rc = f_sync(&Fil);
				rc = f_close(&Fil);
	}}}

	while(1);
}

void
AppButtonHandler(void)
{
    switch(g_sAppState.ulButtons & ALL_BUTTONS)
    {

    case ALL_BUTTONS:{

		rc = f_close(&Fil);

    	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);

		//IntMasterDisable();

    	for(m=0; m<sizeof(rojo); m++){

    		UARTCharPut(UART4_BASE, rojo[m]);}

    	while(1){;}

    }
        }
}

void
SysTickIntHandler(void)
{


    g_sAppState.ulButtons = ButtonsPoll(0,0);
    AppButtonHandler();



}

