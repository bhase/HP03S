
#include "HP03S.h"
#include "HP03S_internal.h"

static uint16_t HP03S_ReadSensorCoefficientImpl(SensorCoefficient coefficient)
{
	return 0;
}

static uint8_t HP03S_ReadSensorParameterImpl(SensorParameter parameter)
{
	return 0;
}

static HP03S_Result HP03S_ReadTemperatureImpl(uint16_t *val)
{
	return 0;
}

static HP03S_Result HP03S_ReadPressureImpl(uint16_t *val)
{
	return 0;
}


uint16_t (*HP03S_ReadSensorCoefficient)(SensorCoefficient coefficient) =
	HP03S_ReadSensorCoefficientImpl;

uint8_t (*HP03S_ReadSensorParameter)(SensorParameter parameter) =
	HP03S_ReadSensorParameterImpl;

HP03S_Result (*HP03S_ReadTemperature)(uint16_t *val) = HP03S_ReadTemperatureImpl;
HP03S_Result (*HP03S_ReadPressure)(uint16_t *val) = HP03S_ReadPressureImpl;
