#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/fatFs_3/diskio.h"
#include "driverlib/fatFs_3/ff.h"
#include "driverlib/fatFs_3/ffconf.h"
#include "driverlib/fatFs_3/integer.h"
#include <stdio.h>



int
main(void)
{
		//Clock del sistema
		SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

		//Configuro la UART1 para el GPS

		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

		GPIOPinConfigure(GPIO_PB0_U1RX);
		GPIOPinConfigure(GPIO_PB1_U1TX);

		GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

		UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
				(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						UART_CONFIG_PAR_NONE));


		//Uso el modulo SPI3 para el YEI3
		SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);

		//Habilito el puerto correspondiente D
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

		//Configuro c/u de los pines para la funcion especificada
		GPIOPinConfigure(GPIO_PD0_SSI3CLK);
		GPIOPinConfigure(GPIO_PD1_SSI3FSS);
		GPIOPinConfigure(GPIO_PD2_SSI3RX);
		GPIOPinConfigure(GPIO_PD3_SSI3TX);

		GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 |
			GPIO_PIN_0);

		//Configuro el SPI para trabajar a 115200bps
		SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 115200, 8);

		SSIEnable(SSI3_BASE);

		//Orden enviada al Yei

		return(0);
}
