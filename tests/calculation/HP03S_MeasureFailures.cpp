#include "CppUTest/TestHarness.h"

extern "C"
{
#include <string.h>
#include "HP03S.h"
#include "HP03S_internal.h"
#include "CppUTestExt/MockSupport_c.h"
#include "stubs.h"

static HP03S_Result My_HP03S_ReadTemperature(uint16_t *temperature)
{
	mock_c()->actualCall("HP03S_ReadTemperature");
	return (HP03S_Result)mock_c()->returnValue().value.intValue;
}

static HP03S_Result My_HP03S_ReadPressure(uint16_t *pressure)
{
	mock_c()->actualCall("HP03S_ReadPressure");
	return (HP03S_Result)mock_c()->returnValue().value.intValue;
}

} /* extern "C" */

/* Init should do a range check on parameter and coefficients */
/* I2C error: device not responding */

TEST_GROUP(HP03S_MeasureFailure)
{
	HP03S_Result result;
	HP03S_Result expected_result;

	void setup()
	{
		Stub_SetupDefault();

		UT_PTR_SET(HP03S_ReadPressure, My_HP03S_ReadPressure);
		UT_PTR_SET(HP03S_ReadTemperature, My_HP03S_ReadTemperature);

		UT_PTR_SET(HP03S_ReadSensorCoefficient, Mock_ReadSensorCoefficient);
		UT_PTR_SET(HP03S_ReadSensorParameter, Mock_ReadSensorParameter);

		HP03S_Create();

		result = HP03S_ERROR;
		expected_result = HP03S_OK;
	}

	void teardown()
	{
		mock_c()->checkExpectations();
		mock_c()->clear();
		LONGS_EQUAL(expected_result, result);

		HP03S_Destroy();
	}
};

TEST(HP03S_MeasureFailure, Uninitialized)
{
	expected_result = HP03S_UNINITIALIZED;
	HP03S_Destroy();

	result = HP03S_Measure();
}

TEST(HP03S_MeasureFailure, Timeout2ndRead)
{
	expected_result = HP03S_DeviceError;
	mock_c()->expectOneCall("HP03S_ReadTemperature")
		->andReturnIntValue(HP03S_OK);
	mock_c()->expectOneCall("HP03S_ReadPressure")
		->andReturnIntValue(HP03S_NoDevice);

	result = HP03S_Measure();
}

TEST(HP03S_MeasureFailure, Timeout1stRead)
{
	expected_result = HP03S_DeviceError;
	mock_c()->expectOneCall("HP03S_ReadTemperature")
		->andReturnIntValue(HP03S_NoDevice);

	result = HP03S_Measure();
}
