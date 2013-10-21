#include "CppUTest/TestHarness.h"

extern "C"
{
#include "stubs.h"
}


TEST_GROUP(HP03S_Coefficients)
{
	HP03S_Result measure_result;
	HP03S_Result create_result;

	uint16_t sensor_coefficients[7];

	void setup_default_coefficients(void)
	{
		sensor_coefficients[C1_SensitivityCoefficient] = 29908;
		sensor_coefficients[C2_OffsetCoefficient] = 3724;
		sensor_coefficients[C3_TemperatureCoefficientOfSensitivity] = 312;
		sensor_coefficients[C4_TemperatureCoefficientOfOffset] = 441;
		sensor_coefficients[C5_ReferenceTemperature] = 9191;
		sensor_coefficients[C6_TemperatureCoefficientOfTemperature] = 3990;
		sensor_coefficients[C7_OffsetFineTuning] = 2500;
	}

	void setup()
	{
		Stub_SetupDefault();
		setup_default_coefficients();

		measure_result = HP03S_ERROR;

		UT_PTR_SET(HP03S_ReadSensorParameter, Mock_ReadSensorParameter);
		UT_PTR_SET(HP03S_ReadSensorCoefficient, Mock_ReadSensorCoefficient);
		UT_PTR_SET(HP03S_ReadPressure, Mock_ReadPressure);
		UT_PTR_SET(HP03S_ReadTemperature, Mock_ReadTemperature);
	}

	void teardown()
	{
		LONGS_EQUAL(measure_result, HP03S_OK);
		LONGS_EQUAL(create_result, HP03S_OK);
		HP03S_Destroy();
	}

	void testWithCoefficient(SensorCoefficient c, uint16_t value)
	{
		sensor_coefficients[c] = value;
		Stub_SetupCoefficients(sensor_coefficients);
		create_result = HP03S_Create();
		measure_result = HP03S_Measure();
	}
};


TEST(HP03S_Coefficients, C1Min)
{
	testWithCoefficient(C1_SensitivityCoefficient, 0x100);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(-3014, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C1Max)
{
	testWithCoefficient(C1_SensitivityCoefficient, 0xFFFF);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(25458, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C2Min)
{
	testWithCoefficient(C2_OffsetCoefficient, 0);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(14573, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C2Max)
{
	testWithCoefficient(C2_OffsetCoefficient, 0x1FFF);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(4334, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C3Min)
{
	testWithCoefficient(C3_TemperatureCoefficientOfSensitivity, 0);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(10646, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C3Max)
{
	testWithCoefficient(C3_TemperatureCoefficientOfSensitivity, 0x400);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(8257, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C4Min)
{
	testWithCoefficient(C4_TemperatureCoefficientOfOffset, 0);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(9734, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C4Max)
{
	testWithCoefficient(C4_TemperatureCoefficientOfOffset, 0x1000);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(11445, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C5Min)
{
	testWithCoefficient(C5_ReferenceTemperature, 0x1000);

	LONGS_EQUAL(250, HP03S_GetTemperature());
	LONGS_EQUAL(10891, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C5Max)
{
	testWithCoefficient(C5_ReferenceTemperature, 0xFFFF);

	LONGS_EQUAL(-6762, HP03S_GetTemperature());
	LONGS_EQUAL(-10214, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C6Min)
{
	testWithCoefficient(C6_TemperatureCoefficientOfTemperature, 0);

	LONGS_EQUAL(260, HP03S_GetTemperature());
	LONGS_EQUAL(9918, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C6Max)
{
	testWithCoefficient(C6_TemperatureCoefficientOfTemperature, 0x4000);

	LONGS_EQUAL(-1109, HP03S_GetTemperature());
	LONGS_EQUAL(9918, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C7Min)
{
	testWithCoefficient(C7_OffsetFineTuning, 2400);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(9818, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, C7Max)
{
	testWithCoefficient(C7_OffsetFineTuning, 2600);

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(10018, HP03S_GetPressure());
}
