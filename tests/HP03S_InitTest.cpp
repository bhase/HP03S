#include "CppUTest/TestHarness.h"

extern "C"
{

#include "HP03S.h"
#include "HP03S_internal.h"

#include "CppUTestExt/MockSupport_c.h"

static uint16_t Mock_HP03S_ReadSensorCoefficient(SensorCoefficient coefficient)
{
	mock_c()->actualCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", coefficient);
	return 0;
}

static uint8_t Mock_HP03S_ReadSensorParameter(SensorParameter parameter)
{
	mock_c()->actualCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", parameter);
	return 0;
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
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C1_SensitivityCoefficient);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C2_OffsetCoefficient);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C3_TemperatureCoefficientOfSensitivity);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C4_TemperatureCoefficientOfOffset);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C5_ReferenceTemperature);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C6_TemperatureCoefficientOfTemperature);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C7_OffsetFineTuning);

	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", SensorParameter_A);
	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", SensorParameter_B);
	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", SensorParameter_C);
	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", SensorParameter_D);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, Uninitialized)
{
	expected_result = HP03S_UNINITIALIZED;
	init_result = HP03S_Measure();
}

