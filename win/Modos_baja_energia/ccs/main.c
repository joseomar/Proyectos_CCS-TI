#include "utils/ustdlib.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/hibernate.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"

//Realiza el chequeo de errores sobrela funcion llamada. Es euna buena practica de programacion
#ifdef DEBUG
void__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

int main(void)
{
	//Setea el clock del sistema para que trabaje a 40 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	//Leds encendidos para indicar WAKE y apagados para indicar HIBERNATE
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x08);

	//Habilito el modulo de hibernacion
	SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
	//Clock para el modulo de hibernacion (en este caso igual al del sistema)
	HibernateEnableExpClk(SysCtlClockGet());
	//Habilita el estado del Pin durante la hibernacion, y lo mantiene incluso cuando esta "despertando"
	HibernateGPIORetentionEnable();
	//Retardo de 4seg para observar el LED
	SysCtlDelay(64000000);
	//Setea lacondicion WAKEpara el Pin WAKE
	HibernateWakeSet(HIBERNATE_WAKE_PIN);
	//Apaga el LED verde, antes de que el dispositivo vaya a dormir (SLEEP MODE)
	//GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0x00);

	//LLama al modulo de hibernacion para que desactive el regulador externo, y tanto el procesador
	//como los perisfericos queden sin alimentacion. El modulo de hibernacion, permanece alimentado por la bateria o
	//lo que este alimentando al circuito.
	HibernateRequest();
	while(1)
	{
	}
}
