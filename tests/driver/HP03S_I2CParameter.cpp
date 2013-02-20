
#include "CppUTest/TestHarness.h"

extern "C"
{
#include <string.h>

#include "HP03S.h"
#include "HP03S_internal.h"
#include "MockI2C.h"
}

TEST_GROUP(HP03S_I2CParameter)
{
	HP03S_Result result;
	HP03S_Result expected_result;
	uint8_t output_buffer[1];
	uint8_t expected_parameter[4];
	uint8_t returned_parameter[4];

	static const uint16_t EEPROM_DeviceAddress = 0xA0;
	static const uint8_t ParameterStartAddress = 0x1E;

	void setup(void)
	{
		memset(expected_parameter, 0xAA, sizeof(expected_parameter));
		memset(returned_parameter, 0x01, sizeof(returned_parameter));

		output_buffer[0] = ParameterStartAddress;

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

TEST(HP03S_I2CParameter, ReadOk)
{
	/* expect: I2C start
	 * I2C write (EEP address)
	 * => I2C_WriteTo(...)
	 * I2C start
	 * read eeprom address 30 (param A) to 33 (param D)
	 * => I2C_readFrom(...)
	 * I2C stop
	 * => I2C_Run()
	 */
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(EEPROM_DeviceAddress,
						    1, output_buffer);
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(EEPROM_DeviceAddress,
						    4, expected_parameter);
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	result = HP03S_ReadSensorParameter(returned_parameter);
	CHECK(memcmp(expected_parameter,
		     returned_parameter,
		     sizeof(expected_parameter)) == 0);
}

TEST(HP03S_I2CParameter, NoResponse)
{
	expected_result = HP03S_NoDevice;
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(EEPROM_DeviceAddress,
						    1, output_buffer);
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(EEPROM_DeviceAddress,
						    4, expected_parameter);
	MockI2C_Expect_I2C_Run_and_return(I2C_Timeout);

	result = HP03S_ReadSensorParameter(returned_parameter);
}
