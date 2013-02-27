
#include "CppUTest/TestHarness.h"

extern "C"
{
#include <string.h>

#include "HP03S.h"
#include "HP03S_internal.h"
#include "MockI2C.h"
}

TEST_GROUP(HP03S_I2CCoefficients)
{
	HP03S_Result result;
	HP03S_Result expected_result;
	uint8_t EEPROM_CellAddress[1];
	uint8_t coefficients_i2c[14];
	uint16_t returned_coefficients[7];

	static const I2C_Address EEPROM_DeviceAddress = 0xA0;
	static const uint8_t CoefficientStartAddress = 0x10;

	void setup(void)
	{
		memset(returned_coefficients, 0x01, sizeof(returned_coefficients));
		memset(coefficients_i2c, 0, sizeof(coefficients_i2c));

		coefficients_i2c[0] = 0x3;
		coefficients_i2c[1] = 0xA7;
		coefficients_i2c[4] = 0x7;
		coefficients_i2c[5] = 0x03;

		EEPROM_CellAddress[0] = CoefficientStartAddress;

		result = HP03S_ERROR;
		expected_result = HP03S_OK;

		MockI2C_Create(3);
	}

	void teardown(void)
	{
		MockI2C_CheckExpectations();
		MockI2C_Destroy();

		LONGS_EQUAL(expected_result, result);
	}
};

TEST(HP03S_I2CCoefficients, ReadOk)
{
	/* expect: I2C start
	 * I2C write (EEP address)
	 * => I2C_WriteTo(...)
	 * I2C start
	 * read eeprom address 16 (C1, MSB) to 30 (C7, LSB)
	 * => I2C_readFrom(...)
	 * I2C stop
	 * => I2C_Run()
	 */
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(EEPROM_DeviceAddress,
						    1, EEPROM_CellAddress);
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(EEPROM_DeviceAddress,
						    14,
						    coefficients_i2c);
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	result = HP03S_ReadSensorCoefficient(returned_coefficients);
	/* check byte order */
	LONGS_EQUAL(0x3A7, returned_coefficients[0]);
	LONGS_EQUAL(0x703, returned_coefficients[2]);
}

TEST(HP03S_I2CCoefficients, NoResponse)
{
	expected_result = HP03S_NoDevice;
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(EEPROM_DeviceAddress,
						    1, EEPROM_CellAddress);
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(EEPROM_DeviceAddress,
						    14,
						    coefficients_i2c);
	MockI2C_Expect_I2C_Run_and_return(I2C_Timeout);

	result = HP03S_ReadSensorCoefficient(returned_coefficients);
}
