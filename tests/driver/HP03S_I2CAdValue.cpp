
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
	uint16_t expected_ad_value;
	uint16_t ad_value;

	static const I2C_Address Ad_DeviceAddress = 0xEE;
	static const uint8_t ChannelPressure = 0xF0;
	static const uint8_t ChannelTemperature = 0xD0;

	void setup(void)
	{
		ad_value = 0;
		expected_ad_value = 0x473;

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
		LONGS_EQUAL(expected_ad_value, ad_value);
	}

	void setupCommon_AdValue_Read(uint8_t ad_channel)
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

		uint8_t ad_command[2];
		uint8_t i2c_answer[2];

		ad_command[0] = 0xFF;
		ad_command[1] = ad_channel;

		i2c_answer[0] = 0x04;
		i2c_answer[1] = 0x73;

		MockI2C_Expect_I2C_WriteTo_and_check_buffer(Ad_DeviceAddress,
							    2, ad_command);
		MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

		/* TODO this does not enforce order */
		mock_c()->expectOneCall("Time_msWait")->withIntParameters("milliseconds", 40);

		ad_command[0] = 0xFD;
		MockI2C_Expect_I2C_WriteTo_and_check_buffer(Ad_DeviceAddress,
							    1, ad_command);
		MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(Ad_DeviceAddress,
							    2, i2c_answer);
		MockI2C_Expect_I2C_Run_and_return(I2C_Ok);
	}

};


TEST(HP03S_AdValue, PressureReadOk)
{
	setupCommon_AdValue_Read(ChannelPressure);

	result = HP03S_ReadPressure(&ad_value);
}


TEST(HP03S_AdValue, TemperatureReadOk)
{
	setupCommon_AdValue_Read(ChannelTemperature);

	result = HP03S_ReadTemperature(&ad_value);
}

