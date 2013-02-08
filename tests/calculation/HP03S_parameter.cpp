#include "CppUTest/TestHarness.h"

extern "C"
{
#include "HP03S.h"
#include "HP03S_internal.h"

static uint8_t sensor_parameters[4];

static uint16_t ad_temperature = 0;

static uint16_t Mock_ReadPressure(void)
{
	return 30036;
}

static uint16_t Mock_ReadTemperature(void)
{
	return ad_temperature;
}

static uint16_t Mock_ReadSensorCoefficient(SensorCoefficient coefficient)
{
	const uint16_t sensor_coefficients[7] = {
		 /* C1_SensitivityCoefficient */ 29908,
		 /* C2_OffsetCoefficient */ 3724,
		 /* C3_TemperatureCoefficientOfSensitivity */ 312,
		 /* C4_TemperatureCoefficientOfOffset */ 441,
		 /* C5_ReferenceTemperature */ 9191,
		 /* C6_TemperatureCoefficientOfTemperature */ 3990,
		 /* C7_OffsetFineTuning */ 2500,
	};
	return sensor_coefficients[coefficient];
}

static uint8_t Mock_ReadSensorParameter(SensorParameter param)
{
	return sensor_parameters[param];
}

}


TEST_GROUP(HP03S_Parameter)
{
	HP03S_Result measure_result;
	HP03S_Result create_result;

	void setup_default_parameter(void)
	{
		sensor_parameters[SensorParameter_A] = 1;
		sensor_parameters[SensorParameter_B] = 4;
		sensor_parameters[SensorParameter_C] = 4;
		sensor_parameters[SensorParameter_D] = 9;
	}

	void setup_default_ad_values(void)
	{
		ad_temperature = 4107;
	}

	void setup()
	{
		setup_default_parameter();
		setup_default_ad_values();

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

	void testWithParameter(SensorParameter p, uint8_t value)
	{
		sensor_parameters[p] = value;
		create_result = HP03S_Create();
		measure_result = HP03S_Measure();
	}
};


TEST(HP03S_Parameter, AMin)
{
	/* to test A we need a higher temperature that it has influence */
	ad_temperature = 12345;
	testWithParameter(SensorParameter_A, 1);

	LONGS_EQUAL(433, HP03S_GetTemperature());
	LONGS_EQUAL(11442, HP03S_GetPressure());
}

TEST(HP03S_Parameter, AMax)
{
	/* to test A we need a higher temperature that it has influence */
	ad_temperature = 12345;
	testWithParameter(SensorParameter_A, 0x3F);

	LONGS_EQUAL(295, HP03S_GetTemperature());
	LONGS_EQUAL(11024, HP03S_GetPressure());
}

TEST(HP03S_Parameter, BMin)
{
	testWithParameter(SensorParameter_B, 1);

	LONGS_EQUAL(-55, HP03S_GetTemperature());
	LONGS_EQUAL(9971, HP03S_GetPressure());
}

TEST(HP03S_Parameter, BMax)
{
	testWithParameter(SensorParameter_B, 63);

	LONGS_EQUAL(-415, HP03S_GetTemperature());
	LONGS_EQUAL(8886, HP03S_GetPressure());
}

TEST(HP03S_Parameter, CMin)
{
	testWithParameter(SensorParameter_C, 1);

	LONGS_EQUAL(-235, HP03S_GetTemperature());
	LONGS_EQUAL(9428, HP03S_GetPressure());
}

TEST(HP03S_Parameter, CMax)
{
	testWithParameter(SensorParameter_C, 15);

	LONGS_EQUAL(-50, HP03S_GetTemperature());
	LONGS_EQUAL(9988, HP03S_GetPressure());
}

TEST(HP03S_Parameter, DMin)
{
	testWithParameter(SensorParameter_D, 1);

	LONGS_EQUAL(2656, HP03S_GetTemperature());
	LONGS_EQUAL(9918, HP03S_GetPressure());
}

TEST(HP03S_Parameter, DMax)
{
	testWithParameter(SensorParameter_D, 15);

	LONGS_EQUAL(-83, HP03S_GetTemperature());
	LONGS_EQUAL(9918, HP03S_GetPressure());
}
