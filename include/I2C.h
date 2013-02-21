#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

typedef enum {
	I2C_Ok = 0,
	I2C_AckFailure,
	I2C_Timeout,
} I2C_Result;

typedef uint16_t I2C_Address;

void I2C_ReadFrom(I2C_Address device_address, uint8_t length, uint8_t *buffer);
void I2C_WriteTo(I2C_Address device_address, uint8_t length, uint8_t *buffer);
I2C_Result I2C_Run();

#endif
