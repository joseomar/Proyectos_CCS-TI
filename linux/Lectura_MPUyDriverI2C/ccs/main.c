#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"

#include "utils/uartstdio.h"
#include "utils/ustdlib.h"

#include "sensorlib/hw_mpu9150.h"
#include "sensorlib/hw_ak8975.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/ak8975.h"
#include "sensorlib/mpu9150.h"
#include "sensorlib/comp_dcm.h"

#include "mi_driver_I2C.h"

#define MPU9150_I2C_ADDRESS     0x68

tI2CMInstance g_sI2CInst;

//Rutina de error que es invocada si la driverlib encuentra un error
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//Rutina para configurar la UART0 para mostrar los datos por consola
static void configurar_uart(void){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	                         UART_CONFIG_PAR_NONE));
	 UARTStdioInit(0);

}

int main(void){

    float pfData[16];
    float *pfAccel, *pfGyro, *pfMag, *pfEulers, *pfQuaternion;
    uint_fast32_t ui32SysClock;

    pfAccel = pfData;
    pfGyro = pfData + 3;
    pfMag = pfData + 6;
    pfEulers = pfData + 9;
    pfQuaternion = pfData + 12;

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ui32SysClock = SysCtlClockGet();

	configurar_uart();

    initI2C1();

	UARTprintf("\033[2JMPU9150 Raw Example\n");

    I2CMInit(&g_sI2CInst, I2C1_MASTER_BASE, INT_I2C1, 0xff, 0xff,
             ui32SysClock);

    I2CMasterInitExpClk(I2C1_MASTER_BASE, SysCtlClockGet(), true);

	return 0;

}
