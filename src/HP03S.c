#include "HP03S.h"
#include "HP03S_internal.h"
#include "GPIO.h"

static struct
{
	Temperature temperature;
	Pressure pressure;
} calculated;

static struct
{
	uint16_t C1;
	uint16_t C2;
	uint16_t C3;
	uint16_t C4;
	uint16_t C5;
	uint16_t C6;
	uint16_t C7;
} sensor_coefficients;

static struct
{
	uint8_t A;
	uint8_t B;
} sensor_parameters;

void HP03S_Create(void)
{
	GPIO_SetXCLR_Low();

	sensor_coefficients.C1 = HP03S_ReadSensorCoefficient(C1_SensitivityCoefficient);
	sensor_coefficients.C2 = HP03S_ReadSensorCoefficient(C2_OffsetCoefficient);
	sensor_coefficients.C3 = HP03S_ReadSensorCoefficient(C3_TemperatureCoefficientOfSensitivity);
	sensor_coefficients.C4 = HP03S_ReadSensorCoefficient(C4_TemperatureCoefficientOfOffset);
	sensor_coefficients.C5 = HP03S_ReadSensorCoefficient(C5_ReferenceTemperature);
	sensor_coefficients.C6 = HP03S_ReadSensorCoefficient(C6_TemperatureCoefficientOfTemperature);
	sensor_coefficients.C7 = HP03S_ReadSensorCoefficient(C7_OffsetFineTuning);

	sensor_parameters.A = HP03S_ReadSensorParameter(SensorParameter_A);
	sensor_parameters.B = HP03S_ReadSensorParameter(SensorParameter_B);
	HP03S_ReadSensorParameter(SensorParameter_C);
	HP03S_ReadSensorParameter(SensorParameter_D);
}

void HP03S_Destroy(void)
{
}


Temperature HP03S_GetTemperature(void)
{
	return calculated.temperature;
}

Pressure HP03S_GetPressure(void)
{
	return calculated.pressure;
}


void HP03S_Measure(void)
{
	uint16_t measured_temperature;
	uint16_t measured_pressure;

	GPIO_SetXCLR_High();
	measured_temperature = HP03S_ReadTemperature();
	measured_pressure = HP03S_ReadPressure();
	GPIO_SetXCLR_Low();

	int temperature_distance  = measured_temperature - sensor_coefficients.C5;
	/* 64 bit is needed for high temperatures */
	int64_t factor = measured_temperature < sensor_coefficients.C5 ? sensor_parameters.B : sensor_parameters.A;
	int dUT = temperature_distance -
		(int)((temperature_distance * factor * temperature_distance) /
		(16384l * (1 << /*C*/4)));
	int OFF = 4 * sensor_coefficients.C2 + (4 * (sensor_coefficients.C4 - 1024) * dUT) / 16384;
	int SENS = sensor_coefficients.C1 + (sensor_coefficients.C3 * dUT) / 1024;
	int X = (SENS * (measured_pressure - 7168)) / 16384 - OFF;

	calculated.pressure = X * 10 / 32 + sensor_coefficients.C7;
	calculated.temperature = 250 + (dUT * sensor_coefficients.C6) / 65536 - dUT / (1 << /*D*/9);
}


static uint16_t HP03S_ReadSensorCoefficientImpl(SensorCoefficient coefficient)
{
	return 0;
}

static uint8_t HP03S_ReadSensorParameterImpl(SensorParameter parameter)
{
	return 0;
}

static uint16_t HP03S_ReadTemperatureImpl(void)
{
	return 0;
}

static uint16_t HP03S_ReadPressureImpl(void)
{
	return 0;
}

uint16_t (*HP03S_ReadSensorCoefficient)(SensorCoefficient coefficient) =
	HP03S_ReadSensorCoefficientImpl;

uint8_t (*HP03S_ReadSensorParameter)(SensorParameter parameter) =
	HP03S_ReadSensorParameterImpl;

uint16_t (*HP03S_ReadTemperature)(void) = HP03S_ReadTemperatureImpl;
uint16_t (*HP03S_ReadPressure)(void) = HP03S_ReadPressureImpl;
