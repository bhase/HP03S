
#include "CppUTest/TestHarness.h"

extern "C"
{
#include <string.h>

#include "HP03S.h"
#include "HP03S_internal.h"
#include "MockI2C.h"
#include "CppUTestExt/MockSupport_c.h"

void Time_msWait(int milliseconds)
{
	mock_c()->actualCall("Time_msWait")
		->withIntParameters("milliseconds", milliseconds);
}

}

TEST_GROUP(HP03S_AdValue)
{
	HP03S_Result result;
	HP03S_Result expected_result;
	uint8_t ad_command[2];
	uint8_t expected_value[2];
	uint16_t expected_pressure;
	uint16_t pressure;

	static const I2C_Address Ad_DeviceAddress = 0xEE;

	void setup(void)
	{
		expected_value[0] = 0x04;
		expected_value[1] = 0x73;
		pressure = 0;
		expected_pressure = 0x473;

		result = HP03S_ERROR;
		expected_result = HP03S_OK;

		MockI2C_Create(5);
	}

	void teardown(void)
	{
		MockI2C_CheckExpectations();
		MockI2C_Destroy();

		mock_c()->checkExpectations();
		mock_c()->clear();

		LONGS_EQUAL(expected_result, result);
		LONGS_EQUAL(expected_pressure, pressure);
	}
};

TEST(HP03S_AdValue, PressureReadOk)
{
	/* expect: I2C start
	 * I2C write (AD address, 0xFF, 0xF0)
	 * => I2C_WriteTo(...)
	 * => I2C_Run()
	 * Delay 40 ms
	 * I2C write (AD address, 0xFD)
	 * => I2C_WriteTo(...)
	 * read ad value
	 * => I2C_readFrom(...)
	 * => I2C_Run
	 */
	ad_command[0] = 0xFF;
	ad_command[1] = 0xF0;

	MockI2C_Expect_I2C_WriteTo_and_check_buffer(Ad_DeviceAddress,
						    2, ad_command);
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	/* TODO this does not enforce order */
	mock_c()->expectOneCall("Time_msWait")->withIntParameters("milliseconds", 40);

	ad_command[0] = 0xFD;
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(Ad_DeviceAddress,
						    1, ad_command);
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(Ad_DeviceAddress,
						    2, expected_value);
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	result = HP03S_ReadPressure(&pressure);
}
