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

#define SLAVE_ADDRESS 0x3C

//*****************************************************************************
//
// Global instance structure for the ISL29023 sensor driver.
//
//*****************************************************************************
tMPU9150 g_sMPU9150Inst;

//Rutina de error que es invocada si la driverlib encuentra un error
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

// Configuro UART0 para ver los datos de la IMU
static void configurar_uart(void);
// Configuro la interfaz con la MPU9150
static void configurar_i2c(void);


void main(void) {

	//En vez de usar 16MHz, uso el PLL para conseguir 40MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	unsigned long g=SysCtlClockGet();

	configurar_uart();

	UARTprintf("\033[2JMPU9150 Raw Example\n");

	configurar_i2c();

    // Initialize the MPU9150 Driver.
    //MPU9150Init(&g_sMPU9150Inst, &g_sI2CInst, MPU9150_I2C_ADDRESS,
               // MPU9150AppCallback, &g_sMPU9150Inst);

}

static void configurar_uart(void){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
	                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	                         UART_CONFIG_PAR_NONE));

}

static void configurar_i2c(void){

	// Habilito el perisferico I2C1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
	// Habilito los GPIOs del puerto A (ya que voy a usar A6 y A7)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	// Configuro la multiplexacion de pines para las funciones del I2C
	GPIOPinConfigure(GPIO_PA6_I2C1SCL);
	GPIOPinConfigure(GPIO_PA7_I2C1SDA);
	// Selecciona las funciones de I2C para estos pines
	GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	// Habilita el loopback mode (permite enviar datos como master y recibir como esclavo)
	HWREG(I2C1_MASTER_BASE + I2C_O_MCR) |= 0x01;

	// Habilito e inicializo el modulo I2C1 para operacion master.
	// Usamos el mismo clock del micro para el I2C1 y el ultimo parametro es
	// la tasa de transferencia (false:100bkps, true:400kbps) => elijo true porque el sensor se comunica a esta tasa
	I2CMasterInitExpClk(I2C1_MASTER_BASE, SysCtlClockGet(), true);

	// Habilito tambien el modulo esclavo para testeo.
	I2CSlaveEnable(I2C1_SLAVE_BASE);

	// Seteo la direccion del esclavo a SLAVE_ADDRESS
	I2CSlaveInit(I2C1_SLAVE_BASE, SLAVE_ADDRESS);

	// Le digo al modulo master en que dirección colocar el BUS para comunicarse con el esclavo.
	// Inicializa el modo escritura en el esclavo (false)
	I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, SLAVE_ADDRESS, false);

}

/*
 * (Pone un byte o caracter en un buffer hasta esperar la orden de envio)
 * I2CMasterDataPut(I2C0_MASTER_BASE, ulDataTx[ulindex]);
 *
 * (Inicializa el envio de datos desde el master al slave)
 * I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);
 *
 * Espera hasta que el esclavo haya recibido y hecho un ack de los datos
 * while(!(I2CSlaveStatus(I2C0_SLAVE_BASE) & I2C_SCSR_RREQ))
        {
        }
 *
 * Lee un dato desde el esclavo
 * I2CSlaveDataGet(I2C0_SLAVE_BASE);
 *
 * Espera a que el master haga la transferencia
 *         while(I2CMasterBusy(I2C0_MASTER_BASE))
        {
        }
 *
 * Inicializacion de master para lectura de los datos del esclavo (true).
 * I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, SLAVE_ADDRESS, true);
 *
 * recepcion tonta para asegurarse de no estar recibiendo basura
 * I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
 *
 * Se espera hasta el requerimiento de recepcion del master
 *  while(!(I2CSlaveStatus(I2C0_SLAVE_BASE) & I2C_SLAVE_ACT_TREQ))
    {
    }
 *
 * El esclavo pone los datos a ser enviados en el registro correspondiente
 * I2CSlaveDataPut(I2C0_SLAVE_BASE, ulDataTx[ulindex]);
 *
 * Le dice al master que lea datos!
 * I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
 *
 * Espera hasta que el esclavo este enviando datos
 *  while(!(I2CSlaveStatus(I2C0_SLAVE_BASE) & I2C_SLAVE_ACT_TREQ))
        {º
        }
 *
 *
 *
 *
 */


