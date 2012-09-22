#include "HP03S.h"
#include "HP03S_internal.h"
#include "GPIO.h"

static struct
{
	Temperature temperature;
	Pressure pressure;
} calculated;

void HP03S_Create(void)
{
	GPIO_SetXCLR_Low();

	HP03S_ReadSensorCoefficient(C1_SensitivityCoefficient);
	HP03S_ReadSensorCoefficient(C2_OffsetCoefficient);
	HP03S_ReadSensorCoefficient(C3_TemperatureCoefficientOfSensitivity);
	HP03S_ReadSensorCoefficient(C4_TemperatureCoefficientOfOffset);
	HP03S_ReadSensorCoefficient(C5_ReferenceTemperature);
	HP03S_ReadSensorCoefficient(C6_TemperatureCoefficientOfTemperature);
	HP03S_ReadSensorCoefficient(C7_OffsetFineTuning);

	HP03S_ReadSensorParameter(SensorParameter_A);
	HP03S_ReadSensorParameter(SensorParameter_B);
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

	GPIO_SetXCLR_High();
	measured_temperature = HP03S_ReadTemperature();
	HP03S_ReadPressure();
	GPIO_SetXCLR_Low();

	int temperature_distance  = measured_temperature - /* C5 */9191;
	int dUT = temperature_distance -
		(temperature_distance * temperature_distance  * /* B */4) /
		(16384 * (1 << /*C*/4));
	int OFF = 4 * /* C2 */3724 + (4 * (/* C4 */441 - 1024) * dUT) / 16384;
	int SENS = /* C1 */29908 + (/* C3 */312 * dUT) / 1024;
	int X = (SENS * (/* D1 */30036 - 7168)) / 16384 - OFF;

	calculated.pressure = X * 10 / 32 + /* C7 */2500;
	calculated.temperature = 250 + (dUT * /* C6 */3990) / 65536 - dUT / (1 << /*D*/9);
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
