/*
 * mi_driver_I2C.h
 *
 *  Created on: 01/04/2015
 *      Author: José Chelotti
 */

#ifndef MI_DRIVER_I2C_H_
#define MI_DRIVER_I2C_H_

// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.

#ifdef __cplusplus
extern "C"
{
#endif

void initI2C1(void);

uint8_t readI2C1(uint16_t device_address, uint16_t device_register);

void writeI2C1(uint16_t device_address, uint16_t device_register, uint8_t device_data);

#ifdef __cplusplus
}
#endif


#endif /* MI_DRIVER_I2C_H_ */
