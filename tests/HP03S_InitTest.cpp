#include "CppUTest/TestHarness.h"

extern "C"
{
#include <string.h>
#include "HP03S.h"
#include "HP03S_internal.h"
#include "CppUTestExt/MockSupport_c.h"

static uint16_t sensor_coefficients[7];
static uint8_t sensor_parameter[4];

static HP03S_Result Mock_HP03S_ReadSensorCoefficient(uint16_t *coefficient)
{
	mock_c()->actualCall("HP03S_ReadSensorCoefficient");
	memcpy(coefficient, sensor_coefficients, sizeof(sensor_coefficients));
	return (HP03S_Result)mock_c()->returnValue().value.intValue;
}

static HP03S_Result Mock_HP03S_ReadSensorParameter(uint8_t *parameter)
{
	mock_c()->actualCall("HP03S_ReadSensorParameter");
	memcpy(parameter, sensor_parameter, sizeof(sensor_parameter));
	return (HP03S_Result)mock_c()->returnValue().value.intValue;
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

		sensor_coefficients[C1_SensitivityCoefficient] = 29908;
		sensor_coefficients[C2_OffsetCoefficient] = 3724;
		sensor_coefficients[C3_TemperatureCoefficientOfSensitivity] = 312;
		sensor_coefficients[C4_TemperatureCoefficientOfOffset] = 441;
		sensor_coefficients[C5_ReferenceTemperature] =  9191;
		sensor_coefficients[C6_TemperatureCoefficientOfTemperature] = 3990;
		sensor_coefficients[C7_OffsetFineTuning] = 2500;

		sensor_parameter[SensorParameter_A] = 1;
		sensor_parameter[SensorParameter_B] = 4;
		sensor_parameter[SensorParameter_C] = 4;
		sensor_parameter[SensorParameter_D] = 9;
	}

	void setup_defaultCalls(void)
	{
		mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
			->andReturnIntValue(HP03S_OK);
		mock_c()->expectOneCall("HP03S_ReadSensorParameter")
			->andReturnIntValue(HP03S_OK);
	}

	void setup_coefficientRangeError(SensorCoefficient c, uint16_t value)
	{
		setup_defaultCalls();

		sensor_coefficients[c] = value;
		expected_result = HP03S_RangeError;
	}

	void setup_parameterRangeError(SensorParameter p, uint8_t value)
	{
		setup_defaultCalls();

		sensor_parameter[p] = value;
		expected_result = HP03S_RangeError;
	}

	void teardown()
	{
		mock_c()->checkExpectations();
		mock_c()->clear();
		LONGS_EQUAL(expected_result, init_result);
	}
};

TEST(HP03S_Init, Create)
{
	setup_defaultCalls();
	init_result = HP03S_Create();
}

TEST(HP03S_Init, Uninitialized)
{
	expected_result = HP03S_UNINITIALIZED;
	init_result = HP03S_Measure();
}

TEST(HP03S_Init, NoDevice)
{
	expected_result = HP03S_NoDevice;

	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->andReturnIntValue(HP03S_NoDevice);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, DeviceError)
{
	expected_result = HP03S_DeviceError;

	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->andReturnIntValue(HP03S_OK);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->andReturnIntValue(HP03S_NoDevice);

	init_result = HP03S_Create();
}


TEST(HP03S_Init, RangeError_C1)
{
	/* out of range low, high goes to 0xFFFF */
	setup_coefficientRangeError(C1_SensitivityCoefficient, 0x9F);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_C2)
{
	/* out of range high, low goes to 0 */
	setup_coefficientRangeError(C2_OffsetCoefficient, 0x2000);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_C3)
{
	/* out of range high, low goes to 0 */
	setup_coefficientRangeError(C3_TemperatureCoefficientOfSensitivity, 0x401);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_C4)
{
	/* out of range high, low goes to 0 */
	setup_coefficientRangeError(C4_TemperatureCoefficientOfOffset, 0x1001);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_C5)
{
	/* out of range low, high goes to 0xFFFF */
	setup_coefficientRangeError(C5_ReferenceTemperature, 0x9FF);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_C6)
{
	/* out of range high, low goes to 0 */
	setup_coefficientRangeError(C6_TemperatureCoefficientOfTemperature, 0x4001);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_C7_low)
{
	/* out of range low */
	setup_coefficientRangeError(C7_OffsetFineTuning, 0x95F);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_C7_high)
{
	/* out of range high */
	setup_coefficientRangeError(C7_OffsetFineTuning, 0xA29);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_A_low)
{
	/* out of range low */
	setup_parameterRangeError(SensorParameter_A, 0);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_A_high)
{
	/* out of range low */
	setup_parameterRangeError(SensorParameter_A, 0x40);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_B_low)
{
	/* out of range low */
	setup_parameterRangeError(SensorParameter_B, 0);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_B_high)
{
	/* out of range low */
	setup_parameterRangeError(SensorParameter_B, 0x40);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_C_low)
{
	/* out of range low */
	setup_parameterRangeError(SensorParameter_C, 0);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_C_high)
{
	/* out of range low */
	setup_parameterRangeError(SensorParameter_C, 0x10);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_D_low)
{
	/* out of range low */
	setup_parameterRangeError(SensorParameter_D, 0);

	init_result = HP03S_Create();
}

TEST(HP03S_Init, RangeError_D_high)
{
	/* out of range low */
	setup_parameterRangeError(SensorParameter_D, 0x10);

	init_result = HP03S_Create();
}

