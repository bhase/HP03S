#include "HP03S.h"
#include "HP03S_internal.h"
#include "GPIO.h"

typedef int HP03SBOOL;

#include <limits.h>
static HP03SBOOL multiplicationInRange(signed int si_a, signed int si_b);

static struct HP03S_results
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


static int coefficient_out_of_range(uint16_t *coefficients)
{
	if (   (coefficients[C1_SensitivityCoefficient] < 0x100)
	    || (coefficients[C2_OffsetCoefficient] > 0x1FFF)
	    || (coefficients[C3_TemperatureCoefficientOfSensitivity] > 0x400)
	    || (coefficients[C4_TemperatureCoefficientOfOffset] > 0x1000)
	    || (coefficients[C5_ReferenceTemperature] < 0x1000)
	    || (coefficients[C6_TemperatureCoefficientOfTemperature] > 0x4000)
	    || (coefficients[C7_OffsetFineTuning] < 0x960)
	    || (coefficients[C7_OffsetFineTuning] > 0xA28))
		return 1;
	return 0;
}

static int parameter_out_of_range(uint8_t *parameter)
{
	if (   (parameter[SensorParameter_A] < 1)
	    || (parameter[SensorParameter_A] > 0x3F)
	    || (parameter[SensorParameter_B] < 1)
	    || (parameter[SensorParameter_B] > 0x3F)
	    || (parameter[SensorParameter_C] < 1)
	    || (parameter[SensorParameter_C] > 0xF)
	    || (parameter[SensorParameter_D] < 1)
	    || (parameter[SensorParameter_D] > 0xF))
		return 1;
	return 0;
}


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

	if (coefficient_out_of_range(coefficients))
		return HP03S_RangeError;
	if (parameter_out_of_range(parameter))
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
	HP03S_Result ad_result;

	if (module_state == UnInitialized)
		return HP03S_UNINITIALIZED;

	GPIO_SetXCLR_High();
	ad_result = HP03S_ReadTemperature(&measured_temperature);
	if (ad_result == HP03S_OK)
		ad_result = HP03S_ReadPressure(&measured_pressure);
	GPIO_SetXCLR_Low();

	if (ad_result != HP03S_OK)
		return HP03S_DeviceError;

	int temperature_distance  = measured_temperature - sensor_coefficients.C5;
	/* 64 bit is needed for high temperatures */
	HP03S_Bigint factor = measured_temperature < sensor_coefficients.C5
		            ? sensor_parameters.B
		            : sensor_parameters.A;
	HP03S_Bigint dUT = 0;
	if (multiplicationInRange(temperature_distance, temperature_distance)) {
		dUT = temperature_distance -
		((temperature_distance * temperature_distance / 128) * factor) / ((1 << sensor_parameters.C) * 128);
	}
	else {
		dUT = temperature_distance -
		((temperature_distance * factor / 128) * temperature_distance) / ((1 << sensor_parameters.C) * 128);
	}
	int off = 4 * sensor_coefficients.C2
		+ (int)((4 * (sensor_coefficients.C4 - 1024) * dUT) / 16384);
	int sens = 0;
	if (multiplicationInRange(dUT, sensor_coefficients.C3)) {
		 sens = sensor_coefficients.C1
		      + (int)((sensor_coefficients.C3 * dUT) / 1024l);
	}
	else {
		 sens = sensor_coefficients.C1
		      + (int)((sensor_coefficients.C3 / 1024l * dUT));
	}
	int x = 0;
	if (multiplicationInRange(sens, measured_pressure)) {
		x = (int)((sens * (HP03S_Bigint)(measured_pressure - 7168)) / 16384) - off;
	}
	else {
		x = (int)(sens / 16384 * (HP03S_Bigint)((measured_pressure - 7168))) - off;
	}

	calculated.pressure = x * 10 / 32 + sensor_coefficients.C7;
	if (multiplicationInRange(dUT, sensor_coefficients.C6)) {
		calculated.temperature = 250 + (int)((dUT * sensor_coefficients.C6) / 65536)
					     - (int)(dUT / (1 << sensor_parameters.D));
	}
	else {
		calculated.temperature = 250 + (int)((dUT / 65536) * sensor_coefficients.C6)
					     - (int)(dUT / (1 << sensor_parameters.D));
	}

	return HP03S_OK;
}


static HP03SBOOL multiplicationInRange(signed int si_a, signed int si_b)
{
	HP03SBOOL result = 1;
	/* si_a is positive */
	if (si_a > 0) {
		/* si_a and si_b are positive */
		if (si_b > 0) {
			if (si_a > (INT_MAX / si_b)) {
				result = 0;
			}
		} else { /* si_a positive, si_b nonpositive */
			if (si_b < (INT_MIN / si_a)) {
				result = 0;
			}
		} /* si_a positive, si_b nonpositive */
	} else { /* si_a is nonpositive */
		if (si_b > 0) { /* si_a is nonpositive, si_b is positive */
			if (si_a < (INT_MIN / si_b)) {
				result = 0;
			}
		} else { /* si_a and si_b are nonpositive */
			if ( (si_a != 0) && (si_b < (INT_MAX / si_a))) {
				result = 0;
			}
		} /* End if si_a and si_b are nonpositive */
	} /* End if si_a is nonpositive */
	return result;
}
