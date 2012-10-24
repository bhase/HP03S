#include "MockI2C.h"
#include "CppUTest/TestHarness_c.h"

static uint8_t write_expected = 0;
static uint8_t read_expected = 0;

void MockI2C_Create(int size)
{
	write_expected = 0;
	read_expected = 0;
}

void MockI2C_Destroy(void)
{
}

void MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(uint16_t device_address, uint8_t len, uint8_t *buffer)
{
	read_expected++;
}

void MockI2C_Expect_I2C_WriteTo_and_check_buffer(uint16_t device_address, uint8_t len, const uint8_t *buffer)
{
	write_expected++;
}

void MockI2C_Expect_I2C_Run_and_return(I2C_Result result)
{
}

/* I2C dummy implementation */

void I2C_ReadFrom(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
	if (!read_expected) {
		FAIL_TEXT_C("unexpected read");
	}
	else {
		read_expected--;
	}
}

void I2C_WriteTo(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
	if (!write_expected) {
		FAIL_TEXT_C("unexpected write");
	}
	else {
		write_expected--;
	}
}

I2C_Result I2C_Run()
{
	return I2C_Ok;
}

