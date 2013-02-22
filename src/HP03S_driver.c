
#include "HP03S.h"
#include "HP03S_internal.h"
#include "I2C.h"

static const I2C_Address EEPROM_DeviceAddress = 0xA0;

static HP03S_Result HP03S_ReadSensorCoefficientImpl(uint16_t *coefficient)
{
	uint8_t eeprom_cell_address[1] = { 0x10 };
	I2C_WriteTo(EEPROM_DeviceAddress, 1, eeprom_cell_address);
	I2C_ReadFrom(EEPROM_DeviceAddress, 14, (uint8_t *)coefficient);
	I2C_Run();
	return HP03S_OK;
}

static HP03S_Result HP03S_ReadSensorParameterImpl(uint8_t *parameter)
{
	uint8_t eeprom_cell_address[1] = { 0x1E };
	I2C_WriteTo(EEPROM_DeviceAddress, 1, eeprom_cell_address);
	I2C_ReadFrom(EEPROM_DeviceAddress, 4, parameter);
	I2C_Result result = I2C_Run();
	if (result == I2C_Timeout)
		return HP03S_NoDevice;
	return result;
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
