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
	GPIO_SetXCLR_Low();

	sensor_coefficients.C1 =
		HP03S_ReadSensorCoefficient(C1_SensitivityCoefficient);
	sensor_coefficients.C2 =
	       HP03S_ReadSensorCoefficient(C2_OffsetCoefficient);
	sensor_coefficients.C3 =
	       HP03S_ReadSensorCoefficient(C3_TemperatureCoefficientOfSensitivity);
	sensor_coefficients.C4 =
	       HP03S_ReadSensorCoefficient(C4_TemperatureCoefficientOfOffset);
	sensor_coefficients.C5 =
	       HP03S_ReadSensorCoefficient(C5_ReferenceTemperature);
	sensor_coefficients.C6 =
	       HP03S_ReadSensorCoefficient(C6_TemperatureCoefficientOfTemperature);
	sensor_coefficients.C7 =
	       HP03S_ReadSensorCoefficient(C7_OffsetFineTuning);

	sensor_parameters.A = HP03S_ReadSensorParameter(SensorParameter_A);
	sensor_parameters.B = HP03S_ReadSensorParameter(SensorParameter_B);
	sensor_parameters.C = HP03S_ReadSensorParameter(SensorParameter_C);
	sensor_parameters.D = HP03S_ReadSensorParameter(SensorParameter_D);

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
	uint16_t measured_temperature;
	uint16_t measured_pressure;

	if (module_state == UnInitialized)
		return HP03S_UNINITIALIZED;

	GPIO_SetXCLR_High();
	measured_temperature = HP03S_ReadTemperature();
	measured_pressure = HP03S_ReadPressure();
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

