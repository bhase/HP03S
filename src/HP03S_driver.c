
#include "HP03S.h"
#include "HP03S_internal.h"
#include "I2C.h"
#include "Time.h"

static const I2C_Address EEPROM_DeviceAddress = 0xA0;
static const uint8_t CoefficientStartAddress = 0x10;
static const uint8_t ParameterStartAddress = 0x1E;

static I2C_Result read_from_eeprom(uint8_t address, uint8_t length, uint8_t *buffer)
{
	uint8_t eeprom_cell_address[1] = { address };
	I2C_WriteTo(EEPROM_DeviceAddress, 1, eeprom_cell_address);
	I2C_ReadFrom(EEPROM_DeviceAddress, length, buffer);
	return I2C_Run();
}

static HP03S_Result HP03S_ReadSensorCoefficientImpl(uint16_t *coefficient)
{
	uint8_t buffer[14];
	int i;
	I2C_Result result = read_from_eeprom(CoefficientStartAddress,
					     14, buffer);
	if (result == I2C_Timeout)
		return HP03S_NoDevice;

	for (i = 0; i < 7; i++)
		coefficient[i] = (uint16_t)((buffer[i*2] << 8) | buffer[i*2 + 1]);
	return result;
}

static HP03S_Result HP03S_ReadSensorParameterImpl(uint8_t *parameter)
{
	I2C_Result result = read_from_eeprom(ParameterStartAddress,
					     4, parameter);
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
	uint8_t buffer[2];
	buffer[0] = 0xFF;
	buffer[1] = 0xF0;

	I2C_WriteTo(0xEE, 2, buffer);
	I2C_Run();

	Time_msWait(40);

	buffer[0] = 0xFD;
	I2C_WriteTo(0xEE, 1, buffer);
	I2C_ReadFrom(0xEE, 2, buffer);
	I2C_Run();

	*val = (uint16_t)(buffer[1] | (buffer[0] << 8));
	return HP03S_OK;
}


HP03S_Result (*HP03S_ReadSensorCoefficient)(uint16_t *coefficient) =
	HP03S_ReadSensorCoefficientImpl;

HP03S_Result (*HP03S_ReadSensorParameter)(uint8_t *parameter) =
	HP03S_ReadSensorParameterImpl;

HP03S_Result (*HP03S_ReadTemperature)(uint16_t *val) = HP03S_ReadTemperatureImpl;
HP03S_Result (*HP03S_ReadPressure)(uint16_t *val) = HP03S_ReadPressureImpl;
