#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_types.h"
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "driverlib/fatFs_3/diskio.h"
//#include "driverlib/fatFs_3/fatfs.h"
#include "driverlib/fatFs_3/ff.h"
#include "driverlib/fatFs_3/ffconf.h"
#include "driverlib/fatFs_3/integer.h"
#include <stdio.h>


FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
UINT bw;
BYTEC Buff[128];		/* File read buffer */

//char buf[]={'A','V','I','L','A'};

int
main(void)
{
	FRESULT rc;				/* Result code */

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_16MHZ);

	/*SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);

	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
			GPIO_PIN_2);

	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);

	SSIEnable(SSI0_BASE);*/
	rc = f_mount(0, &Fatfs);					//registra un area de trabajo

	rc = f_open(&Fil, "Hola.TXT", FA_WRITE | FA_CREATE_ALWAYS);	//abre o crea un archivo


	rc = f_write(&Fil, "Hello world!\r\n", 14, &bw);

	rc = f_write(&Fil, "Hello world!\r\n", 14, &bw);

	rc = f_close(&Fil);

	unsigned long i= SysCtlClockGet();

	unsigned long j= SSIClockSourceGet(SSI0_BASE);

while(1){}

}
