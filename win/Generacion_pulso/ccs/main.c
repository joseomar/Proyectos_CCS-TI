#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int PinData=2;

int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_1|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);

	while(1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, PinData);
		SysCtlDelay(SysCtlClockGet()/600000);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
		SysCtlDelay(SysCtlClockGet()/600000);
		//if(PinData==8) {PinData=2;} else {PinData=PinData*2;}
	}
}
