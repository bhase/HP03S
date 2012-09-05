#include "HP03S.h"
#include "HP03S_internal.h"

void HP03S_Create(void)
{
	HP03S_ReadSensorCoefficient(0);
}

void HP03S_Destroy(void)
{
}


Temperature HP03S_GetTemperature(void)
{
	return -72;
}

Pressure HP03S_GetPressure(void)
{
	return 9918;
}

static uint16_t HP03S_ReadSensorCoefficientImpl(SensorCoefficient coefficient)
{
	return 0;
}

uint16_t (*HP03S_ReadSensorCoefficient)(SensorCoefficient coefficient) = HP03S_ReadSensorCoefficientImpl;
