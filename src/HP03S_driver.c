
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
