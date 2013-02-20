
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "HP03S.h"
#include "HP03S_internal.h"
#include "MockI2C.h"
}

TEST_GROUP(HP03S_I2C)
{
	HP03S_Result result;
	uint8_t output_buffer[1];
	uint8_t parameter[4];

	void setup(void)
	{
		parameter[0] = 1;
		parameter[1] = 4;
		parameter[2] = 4;
		parameter[3] = 9;
		MockI2C_Create(3);
		result = HP03S_ERROR;
	}

	void teardown(void)
	{
		MockI2C_CheckExpectations();
		MockI2C_Destroy();
		LONGS_EQUAL(result, HP03S_OK);
	}
};

TEST(HP03S_I2C, ParameterRead)
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
	output_buffer[0] = 0x1E;

	MockI2C_Expect_I2C_WriteTo_and_check_buffer(0xA0, 1, output_buffer);
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(0xA0, 4, parameter);
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	result = HP03S_ReadSensorParameter(parameter);
}
