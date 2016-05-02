#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include <string.h>


//Restablece el sistema
//$PMTK314,-1*04<CR><LF>
//char buferA[]={'$','P','M','T','K','3','1','4',',','-','1','*','0','4','\r','\n'};

//Comando solo para dejar el GGA activo
//$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C<CR><LF>
char buferA[]={'$','P','M','T','K','3','1','4',',','0',',','1',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0',',','0','*','2','9','\r','\n'};

//Cambio del baud-rate a 38400
//$PMTK251,38400*27
//char buferA[]={'$','P','M','T','K','2','5','1',',','3','8','4','0','0','*','2','7','\r','\n'}; //este funcaba

//Baud rate por defecto
//$PMTK251,0*28
//char buferA[]={'$','P','M','T','K','2','5','1',',','0','*','2','8','\r','\n'}; //este funcaba

//Comando para testear la comunicacion entre receptor y host
//char buferA[]={'$','P','M','T','K','0','0','0','*','3','2','\r','\n'};
//Deberia devolver $PMTK001,0,3*30; todo esta OK

//Comando para cambiar la frecuencia a 5Hz
//$PMTK300,200,0,0,0,0*2F
//char buferB[]={'$','P','M','T','K','3','0','0',',','2','0','0',',','0',',','0',',','0','*','2','F','\r','\n'};




int i,j,k;

int main(void)
{
	char cThisChar;

	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL |SYSCTL_XTAL_16MHZ| SYSCTL_OSC_MAIN );

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

	//int reloj = SysCtlClockGet();

for(i=0; i<sizeof(buferA); i++){

	UARTCharPut(UART1_BASE, buferA[i]);

	}



/*for(j=0; j<sizeof(buferB); j++){

	UARTCharPut(UART1_BASE, buferB[j]);

	}*/


	 do
	    {
	        cThisChar = UARTCharGet(UART1_BASE);

	        UARTCharPut(UART0_BASE, cThisChar);

	    } while(cThisChar != 'z');


	return(0);
}

