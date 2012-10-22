#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

typedef enum {
	I2C_Ok = 0,
} I2C_Result;

void I2C_ReadFrom(uint16_t device_address, uint8_t length, uint8_t *buffer);
void I2C_WriteTo(uint16_t device_address, uint8_t length, uint8_t *buffer);
I2C_Result I2C_Run();

#endif
