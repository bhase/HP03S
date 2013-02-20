
#include "HP03S.h"
#include "HP03S_internal.h"
#include "I2C.h"

static HP03S_Result HP03S_ReadSensorCoefficientImpl(uint16_t *coefficient)
{
	return HP03S_OK;
}

static HP03S_Result HP03S_ReadSensorParameterImpl(uint8_t *parameter)
{
	uint8_t command[1] = { 0x1E };
	I2C_WriteTo(0xA0, 1, command);
	I2C_ReadFrom(0xA0, 4, parameter);
	return I2C_Run();
}

static HP03S_Result HP03S_ReadTemperatureImpl(uint16_t *val)
{
	return 0;
}

static HP03S_Result HP03S_ReadPressureImpl(uint16_t *val)
{
	return 0;
}


HP03S_Result (*HP03S_ReadSensorCoefficient)(uint16_t *coefficient) =
	HP03S_ReadSensorCoefficientImpl;

HP03S_Result (*HP03S_ReadSensorParameter)(uint8_t *parameter) =
	HP03S_ReadSensorParameterImpl;

HP03S_Result (*HP03S_ReadTemperature)(uint16_t *val) = HP03S_ReadTemperatureImpl;
HP03S_Result (*HP03S_ReadPressure)(uint16_t *val) = HP03S_ReadPressureImpl;
