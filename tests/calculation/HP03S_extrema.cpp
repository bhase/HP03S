#include "CppUTest/TestHarness.h"

extern "C"
{
#include "HP03S.h"
#include "HP03S_internal.h"

static uint16_t sensor_coefficients[7];
static uint8_t sensor_parameters[4];

static uint16_t ad_temperature = 0;
static uint16_t ad_pressure = 0;

static uint16_t Mock_ReadPressure(void)
{
	return ad_pressure;
}

static uint16_t Mock_ReadTemperature(void)
{
	return ad_temperature;
}

static uint16_t Mock_ReadSensorCoefficient(SensorCoefficient coefficient)
{
	return sensor_coefficients[coefficient];
}

static uint8_t Mock_ReadSensorParameter(SensorParameter param)
{
	return sensor_parameters[param];
}

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
	sensor_coefficients[C1_SensitivityCoefficient] = 65535;
	sensor_coefficients[C2_OffsetCoefficient] = 0;
	sensor_coefficients[C3_TemperatureCoefficientOfSensitivity] = 1024;
	sensor_coefficients[C4_TemperatureCoefficientOfOffset] = 0;
	sensor_coefficients[C5_ReferenceTemperature] = 4096;
	sensor_coefficients[C6_TemperatureCoefficientOfTemperature] = 16384;
	sensor_coefficients[C7_OffsetFineTuning] = 2600;

	sensor_parameters[SensorParameter_A] = 63;
	sensor_parameters[SensorParameter_B] = 4; /* does not matter */
	sensor_parameters[SensorParameter_C] = 1;
	sensor_parameters[SensorParameter_D] = 1;

	ad_temperature = 65535;
	ad_pressure = 65535;

	create_result = HP03S_Create();
	measure_result = HP03S_Measure();

	LONGS_EQUAL(1799231, HP03S_GetTemperature());
	LONGS_EQUAL(-8497573, HP03S_GetPressure());
}

TEST(HP03S_Extrema, MinimumValues)
{
	/* the parameter are chosen that the _result_ is minimal */
	sensor_coefficients[C1_SensitivityCoefficient] = 256;
	sensor_coefficients[C2_OffsetCoefficient] = 8191;
	sensor_coefficients[C3_TemperatureCoefficientOfSensitivity] = 0;
	sensor_coefficients[C4_TemperatureCoefficientOfOffset] = 4096;
	sensor_coefficients[C5_ReferenceTemperature] = 65535;
	sensor_coefficients[C6_TemperatureCoefficientOfTemperature] = 0;
	sensor_coefficients[C7_OffsetFineTuning] = 2400;

	sensor_parameters[SensorParameter_A] = 1; /* does not matter */
	sensor_parameters[SensorParameter_B] = 1;
	sensor_parameters[SensorParameter_C] = 15;
	sensor_parameters[SensorParameter_D] = 15;

	ad_temperature = 0;
	ad_pressure = 0;

	create_result = HP03S_Create();
	measure_result = HP03S_Measure();

	LONGS_EQUAL(252, HP03S_GetTemperature());
	LONGS_EQUAL(7487, HP03S_GetPressure());
}
