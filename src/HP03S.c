#include "HP03S.h"
#include "HP03S_internal.h"

void HP03S_Create(void)
{
	HP03S_ReadSensorCoefficient(C1_SensitivityCoefficient);
	HP03S_ReadSensorCoefficient(C2_OffsetCoefficient);
	HP03S_ReadSensorCoefficient(C3_TemperatureCoefficientOfSensitivity);
	HP03S_ReadSensorCoefficient(C4_TemperatureCoefficientOfOffset);
	HP03S_ReadSensorCoefficient(C5_ReferenceTemperature);
	HP03S_ReadSensorCoefficient(C6_TemperatureCoefficientOfTemperature);
	HP03S_ReadSensorCoefficient(C7_OffsetFineTuning);

	HP03S_ReadSensorParameter(SensorParameter_A);
	HP03S_ReadSensorParameter(SensorParameter_B);
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

static uint8_t HP03S_ReadSensorParameterImpl(SensorParameter parameter)
{
	return 0;
}

uint16_t (*HP03S_ReadSensorCoefficient)(SensorCoefficient coefficient) =
	HP03S_ReadSensorCoefficientImpl;

uint8_t (*HP03S_ReadSensorParameter)(SensorParameter parameter) =
	HP03S_ReadSensorParameterImpl;
