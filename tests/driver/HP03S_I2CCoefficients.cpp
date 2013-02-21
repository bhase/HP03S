
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
	uint8_t EEPROM_Address[1];
	uint16_t expected_coefficients[7];
	uint16_t returned_coefficients[7];

	static const I2C_Address EEPROM_DeviceAddress = 0xA0;
	static const uint8_t CoefficientStartAddress = 0x10;

	void setup(void)
	{
		memset(expected_coefficients, 0xAA, sizeof(expected_coefficients));
		memset(returned_coefficients, 0x01, sizeof(returned_coefficients));

		EEPROM_Address[0] = CoefficientStartAddress;

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
						    1, EEPROM_Address);
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(EEPROM_DeviceAddress,
						    14, (uint8_t *)expected_coefficients);
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	result = HP03S_ReadSensorCoefficient(returned_coefficients);
	CHECK(memcmp(expected_coefficients,
		     returned_coefficients,
		     sizeof(expected_coefficients)) == 0);
}
