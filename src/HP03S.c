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
	uint8_t C;
	uint8_t D;
} sensor_parameters;

static enum {
	UnInitialized = 0,
	Ready,
} module_state;

HP03S_Result HP03S_Create(void)
{
	uint16_t coefficients[7];
	uint8_t parameter[4];
	HP03S_Result result;

	GPIO_SetXCLR_Low();

	result = HP03S_ReadSensorParameter(parameter);
	if (result != HP03S_OK)
		return result;

	result = HP03S_ReadSensorCoefficient(coefficients);
	if (result != HP03S_OK)
		return HP03S_DeviceError;

	if (coefficients[C1_SensitivityCoefficient] < 0x100)
		return HP03S_RangeError;
	if (coefficients[C2_OffsetCoefficient] > 0x1FFF)
		return HP03S_RangeError;
	if (coefficients[C3_TemperatureCoefficientOfSensitivity] > 0x400)
		return HP03S_RangeError;
	if (coefficients[C4_TemperatureCoefficientOfOffset] > 0x1000)
		return HP03S_RangeError;
	if (coefficients[C5_ReferenceTemperature] < 0x1000)
		return HP03S_RangeError;
	if (coefficients[C6_TemperatureCoefficientOfTemperature] > 0x4000)
		return HP03S_RangeError;
	if (coefficients[C7_OffsetFineTuning] < 0x960)
		return HP03S_RangeError;
	if (coefficients[C7_OffsetFineTuning] > 0xA28)
		return HP03S_RangeError;

	if (parameter[SensorParameter_A] < 1)
		return HP03S_RangeError;
	if (parameter[SensorParameter_A] > 0x3F)
		return HP03S_RangeError;
	if (parameter[SensorParameter_B] < 1)
		return HP03S_RangeError;
	if (parameter[SensorParameter_B] > 0x3F)
		return HP03S_RangeError;
	if (parameter[SensorParameter_C] < 1)
		return HP03S_RangeError;
	if (parameter[SensorParameter_C] > 0xF)
		return HP03S_RangeError;

	sensor_coefficients.C1 = coefficients[C1_SensitivityCoefficient];
	sensor_coefficients.C2 = coefficients[C2_OffsetCoefficient];
	sensor_coefficients.C3 = coefficients[C3_TemperatureCoefficientOfSensitivity];
	sensor_coefficients.C4 = coefficients[C4_TemperatureCoefficientOfOffset];
	sensor_coefficients.C5 = coefficients[C5_ReferenceTemperature];
	sensor_coefficients.C6 = coefficients[C6_TemperatureCoefficientOfTemperature];
	sensor_coefficients.C7 = coefficients[C7_OffsetFineTuning];
	sensor_parameters.A = parameter[SensorParameter_A];
	sensor_parameters.B = parameter[SensorParameter_B];
	sensor_parameters.C = parameter[SensorParameter_C];
	sensor_parameters.D = parameter[SensorParameter_D];

	module_state = Ready;
	return HP03S_OK;
}

void HP03S_Destroy(void)
{
	module_state = UnInitialized;
}


Temperature HP03S_GetTemperature(void)
{
	return calculated.temperature;
}

Pressure HP03S_GetPressure(void)
{
	return calculated.pressure;
}


HP03S_Result HP03S_Measure(void)
{
	uint16_t measured_temperature = 4107;
	uint16_t measured_pressure = 30036;

	if (module_state == UnInitialized)
		return HP03S_UNINITIALIZED;

	GPIO_SetXCLR_High();
	HP03S_ReadTemperature(&measured_temperature);
	HP03S_ReadPressure(&measured_pressure);
	GPIO_SetXCLR_Low();

	int temperature_distance  = measured_temperature - sensor_coefficients.C5;
	/* 64 bit is needed for high temperatures */
	int64_t factor = measured_temperature < sensor_coefficients.C5
		       ? sensor_parameters.B
		       : sensor_parameters.A;
	int64_t dUT = temperature_distance -
		((temperature_distance * factor * temperature_distance) /
		 (16384l * (1 << sensor_parameters.C)));
	int OFF = 4 * sensor_coefficients.C2
		+ (int)((4 * (sensor_coefficients.C4 - 1024) * dUT) / 16384);
	int SENS = sensor_coefficients.C1
		 + (int)((sensor_coefficients.C3 * dUT) / 1024);
	int X = (int)((SENS * (int64_t)(measured_pressure - 7168)) / 16384) - OFF;

	calculated.pressure = X * 10 / 32 + sensor_coefficients.C7;
	calculated.temperature = 250 + (int)((dUT * sensor_coefficients.C6) / 65536)
		                     - (int)(dUT / (1 << sensor_parameters.D));

	return HP03S_OK;
}

