#ifndef HP03S_INTERNAL_H_
#define HP03S_INTERNAL_H_

#include <stdint.h>

typedef enum
{
	SensorParameter_A,
	SensorParameter_B,
	SensorParameter_C,
	SensorParameter_D,
} SensorParameter;

typedef enum
{
	C1_SensitivityCoefficient,
	C2_OffsetCoefficient,
	C3_TemperatureCoefficientOfSensitivity,
	C4_TemperatureCoefficientOfOffset,
	C5_ReferenceTemperature,
	C6_TemperatureCoefficientOfTemperature,
	C7_OffsetFineTuning,
} SensorCoefficient;

extern HP03S_Result (*HP03S_ReadSensorParameter)(uint8_t *parameter);
extern HP03S_Result (*HP03S_ReadSensorCoefficient)(uint16_t *coefficient);

extern HP03S_Result (*HP03S_ReadTemperature)(uint16_t *advalue);
extern HP03S_Result (*HP03S_ReadPressure)(uint16_t *advalue);

#endif
