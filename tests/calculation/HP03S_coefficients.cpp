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


TEST_GROUP(HP03S_Coefficients)
{
	HP03S_Result measure_result;
	HP03S_Result create_result;

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
		ad_pressure = 30036;
	}

	void setup()
	{
		setup_default_parameter();
		setup_default_coefficients();
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

	void testWithCoefficient(SensorCoefficient c, uint16_t value)
	{
		sensor_coefficients[c] = value;
		create_result = HP03S_Create();
		measure_result = HP03S_Measure();
	}

	void testWithParameter(SensorParameter p, uint8_t value)
	{
		sensor_parameters[p] = value;
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

	LONGS_EQUAL(-6763, HP03S_GetTemperature());
	LONGS_EQUAL(-10218, HP03S_GetPressure());
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

TEST(HP03S_Coefficients, AMin)
{
	/* to test A we need a higher temperature that it has influence */
	ad_temperature = 12345;
	testWithParameter(SensorParameter_A, 1);

	LONGS_EQUAL(433, HP03S_GetTemperature());
	LONGS_EQUAL(11442, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, AMax)
{
	/* to test A we need a higher temperature that it has influence */
	ad_temperature = 12345;
	testWithParameter(SensorParameter_A, 0x3F);

	LONGS_EQUAL(295, HP03S_GetTemperature());
	LONGS_EQUAL(11024, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, BMin)
{
	testWithParameter(SensorParameter_B, 1);

	LONGS_EQUAL(-55, HP03S_GetTemperature());
	LONGS_EQUAL(9971, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, BMax)
{
	testWithParameter(SensorParameter_B, 63);

	LONGS_EQUAL(-415, HP03S_GetTemperature());
	LONGS_EQUAL(8886, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, CMin)
{
	testWithParameter(SensorParameter_C, 1);

	LONGS_EQUAL(-235, HP03S_GetTemperature());
	LONGS_EQUAL(9428, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, CMax)
{
	testWithParameter(SensorParameter_C, 15);

	LONGS_EQUAL(-50, HP03S_GetTemperature());
	LONGS_EQUAL(9988, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, DMin)
{
	testWithParameter(SensorParameter_D, 1);

	LONGS_EQUAL(2656, HP03S_GetTemperature());
	LONGS_EQUAL(9918, HP03S_GetPressure());
}

TEST(HP03S_Coefficients, DMax)
{
	testWithParameter(SensorParameter_D, 15);

	LONGS_EQUAL(-83, HP03S_GetTemperature());
	LONGS_EQUAL(9918, HP03S_GetPressure());
}


TEST(HP03S_Coefficients, MaximumValues)
{
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

TEST(HP03S_Coefficients, MinimumValues)
{
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
