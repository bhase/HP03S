#include "MockI2C.h"

void MockI2C_Create(int size)
{
}

void MockI2C_Destroy(void)
{
}

void MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(uint16_t device_address, uint8_t len, uint8_t *buffer)
{
}

void MockI2C_Expect_I2C_WriteTo_and_check_buffer(uint16_t device_address, uint8_t len, const uint8_t *buffer)
{
}

void MockI2C_Expect_I2C_Run_and_return(I2C_Result result)
{
}

/* I2C dummy implementation */

void I2C_ReadFrom(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
}

void I2C_WriteTo(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
}

I2C_Result I2C_Run()
{
	return I2C_Ok;
}

