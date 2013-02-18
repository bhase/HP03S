#include "CppUTest/TestHarness.h"

extern "C"
{

#include "HP03S.h"
#include "HP03S_internal.h"

#include "CppUTestExt/MockSupport_c.h"

static HP03S_Result Mock_HP03S_ReadSensorCoefficient(uint16_t *coefficient)
{
	mock_c()->actualCall("HP03S_ReadSensorCoefficient");
	return HP03S_OK;
}

static HP03S_Result Mock_HP03S_ReadSensorParameter(uint8_t *parameter)
{
	mock_c()->actualCall("HP03S_ReadSensorParameter");
	return HP03S_OK;
}

} /* extern "C" */

/* Init should do a range check on parameter and coefficients */
/* I2C error: device not responding */

TEST_GROUP(HP03S_Init)
{
	HP03S_Result init_result;
	HP03S_Result expected_result;

	void setup()
	{
		UT_PTR_SET(HP03S_ReadSensorCoefficient, Mock_HP03S_ReadSensorCoefficient);
		UT_PTR_SET(HP03S_ReadSensorParameter, Mock_HP03S_ReadSensorParameter);
		init_result = HP03S_ERROR;
		expected_result = HP03S_OK;
	}

	void teardown()
	{
		mock_c()->checkExpectations();
		mock_c()->clear();
		LONGS_EQUAL(init_result, expected_result);
	}
};

TEST(HP03S_Init, Create)
{
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient");
	mock_c()->expectOneCall("HP03S_ReadSensorParameter");

	init_result = HP03S_Create();
}

TEST(HP03S_Init, Uninitialized)
{
	expected_result = HP03S_UNINITIALIZED;
	init_result = HP03S_Measure();
}

