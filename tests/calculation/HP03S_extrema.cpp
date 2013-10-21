#include "CppUTest/TestHarness.h"

extern "C"
{
#include "stubs.h"
}


TEST_GROUP(HP03S_Extrema)
{
	HP03S_Result measure_result;
	HP03S_Result create_result;

	void setup()
	{
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
};


TEST(HP03S_Extrema, MaximumValues)
{
	/* the parameter are chosen that the _result_ is maximal */
	uint16_t sensor_coefficients[7];
	sensor_coefficients[C1_SensitivityCoefficient] = 65535;
	sensor_coefficients[C2_OffsetCoefficient] = 0;
	sensor_coefficients[C3_TemperatureCoefficientOfSensitivity] = 1024;
	sensor_coefficients[C4_TemperatureCoefficientOfOffset] = 0;
	sensor_coefficients[C5_ReferenceTemperature] = 4096;
	sensor_coefficients[C6_TemperatureCoefficientOfTemperature] = 16384;
	sensor_coefficients[C7_OffsetFineTuning] = 2600;
	Stub_SetupCoefficients(sensor_coefficients);

	uint8_t sensor_parameters[4];
	sensor_parameters[SensorParameter_A] = 63;
	sensor_parameters[SensorParameter_B] = 4; /* does not matter */
	sensor_parameters[SensorParameter_C] = 1;
	sensor_parameters[SensorParameter_D] = 1;
	Stub_SetupParameters(sensor_parameters);

	Stub_SetupRawTemperature(65535);
	Stub_SetupRawPressure(65535);

	create_result = HP03S_Create();
	measure_result = HP03S_Measure();

	LONGS_EQUAL(1812295, HP03S_GetTemperature());
	LONGS_EQUAL(-7920396, HP03S_GetPressure());
}

TEST(HP03S_Extrema, MinimumValues)
{
	/* the parameter are chosen that the _result_ is minimal */
	uint16_t sensor_coefficients[7];
	sensor_coefficients[C1_SensitivityCoefficient] = 256;
	sensor_coefficients[C2_OffsetCoefficient] = 8191;
	sensor_coefficients[C3_TemperatureCoefficientOfSensitivity] = 0;
	sensor_coefficients[C4_TemperatureCoefficientOfOffset] = 4096;
	sensor_coefficients[C5_ReferenceTemperature] = 65535;
	sensor_coefficients[C6_TemperatureCoefficientOfTemperature] = 0;
	sensor_coefficients[C7_OffsetFineTuning] = 2400;
	Stub_SetupCoefficients(sensor_coefficients);

	uint8_t sensor_parameters[4];
	sensor_parameters[SensorParameter_A] = 1; /* does not matter */
	sensor_parameters[SensorParameter_B] = 1;
	sensor_parameters[SensorParameter_C] = 15;
	sensor_parameters[SensorParameter_D] = 15;
	Stub_SetupParameters(sensor_parameters);

	Stub_SetupRawTemperature(0);
	Stub_SetupRawPressure(0);

	create_result = HP03S_Create();
	measure_result = HP03S_Measure();

	LONGS_EQUAL(252, HP03S_GetTemperature());
	LONGS_EQUAL(7487, HP03S_GetPressure());
}
