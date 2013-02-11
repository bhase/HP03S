#include "stubs.h"

static const uint16_t default_sensor_coefficients[7] = {
	/* C1_SensitivityCoefficient */ 29908,
	/* C2_OffsetCoefficient */ 3724,
	/* C3_TemperatureCoefficientOfSensitivity */ 312,
	/* C4_TemperatureCoefficientOfOffset */ 441,
	/* C5_ReferenceTemperature */ 9191,
	/* C6_TemperatureCoefficientOfTemperature */ 3990,
	/* C7_OffsetFineTuning */ 2500,
};
static const uint8_t default_sensor_parameters[4] = {
	/* SensorParameter_A */ 1,
	/* SensorParameter_B */ 4,
	/* SensorParameter_C */ 4,
	/* SensorParameter_D */ 9,
};

static const uint16_t *sensor_coefficients = default_sensor_coefficients;
static const uint8_t *sensor_parameters = default_sensor_parameters;

static uint16_t ad_temperature = 4107;
static uint16_t ad_pressure = 30036;

uint16_t Mock_ReadPressure(void)
{
	return ad_pressure;
}

uint16_t Mock_ReadTemperature(void)
{
	return ad_temperature;
}

uint16_t Mock_ReadSensorCoefficient(SensorCoefficient coefficient)
{
	return sensor_coefficients[coefficient];
}

uint8_t Mock_ReadSensorParameter(SensorParameter param)
{
	return sensor_parameters[param];
}


void Stub_SetupDefault(void)
{
	ad_temperature = 4107;
	ad_pressure = 30036;
	sensor_parameters = default_sensor_parameters;
	sensor_coefficients = default_sensor_coefficients;
}

void Stub_SetupCoefficients(uint16_t *coefficients)
{
	sensor_coefficients = coefficients;
}

void Stub_SetupParameters(uint8_t *parameters)
{
	sensor_parameters = parameters;
}

void Stub_SetupRawPressure(uint16_t pressure)
{
	ad_pressure = pressure;
}

void Stub_SetupRawTemperature(uint16_t temperature)
{
	ad_temperature = temperature;
}

