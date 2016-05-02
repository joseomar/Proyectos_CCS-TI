#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include <string.h>


unsigned int frecuencia= 1000000;
unsigned int duracion= 100000000;
unsigned int inicio= 1000;

//Sesion de streaming
//#define buferA     ":80,34,255,255,255,255,255,255,255,255\n" //orientacion como 2 vectores
//#define buferB     ":82,100000,100000,100\n" //Aceleracion lineal corregida
//#define buferC     ":85\n" //Inicio de streaming


//#define buferA     ":238,0,1,0\n" //cambio de color del LED (blanco)
//#define buferA     ":1\n" //orientacion como angulo de Euler
//#define buferA     ":41\n" //Lee aceleracion lineal corregida------> No devuelve
#define buferA     ":39\n" //Vector del acelerometro corregido---->  OK
//#define buferA     ":34\n" //Vector del acelerometro normalizado
//#define buferA     ":2\n" //orientacion como matriz de rotacion
//#define buferB     ":10\n" //orientacion como angulo de eje
//#define buferB     ":82,1000000,100000000,1000\n" //Comando de timing (frecuencia, duracion de la sesion, delay de inicio)
//#define buferA     ":4\n" //orientacion como 2 vectores
//#define buferA     ":0\n" //Orientacion como cuaternion
//#define buferC     ":85\n" //Inicio de streaming
//#define buferA     ":106,2\n" //orientacion como cuaternion
//char buferA[]={'$','P','M','T','K','2','5','1',',','3','8','4','0','0','*','2','7','\r','\n'}; //este funcaba
//char buferA[]=":0\n";
//char buferB[]="$PMTK300,200,0,0,0,0*2F";

int i,j,k,contador2;

int main(void)
{
	char cThisChar;

	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL |SYSCTL_XTAL_16MHZ| SYSCTL_OSC_MAIN );

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART4);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinConfigure(GPIO_PC4_U4RX);
	GPIOPinConfigure(GPIO_PC5_U4TX);

	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART4_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	                         UART_CONFIG_PAR_NONE));

	//unsigned int reloj = SysCtlClockGet();

for(i=0; i<sizeof(buferA); i++){

	UARTCharPut(UART4_BASE, buferA[i]);

	}

//k=sizeof(buferA);

/*for(j=0; j<sizeof(buferB); j++){

	UARTCharPut(UART4_BASE, buferB[j]);

	}*/
/*
for(k=0; k<sizeof(buferC); k++){

	UARTCharPut(UART4_BASE, buferC[k]);

	}
*/
contador2=0;

do{
while(UARTCharsAvail(UART4_BASE)){

		    	cThisChar=UARTCharGetNonBlocking(UART4_BASE);
		    	UARTCharPut(UART0_BASE, cThisChar);
				contador2=contador2+1;}
}
while(1);

	/* do
	    {
	        cThisChar = UARTCharGet(UART4_BASE);

	        UARTCharPut(UART0_BASE, cThisChar);

	    } while(cThisChar != 'z');*/


	//return(0);
}

