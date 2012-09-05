#ifndef HP03S_INTERNAL_H_
#define HP03S_INTERNAL_H_

typedef enum
{
	dummyparam,
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

extern uint8_t (*HP03S_ReadSensorParameter)(SensorParameter param);
extern uint16_t (*HP03S_ReadSensorCoefficient)(SensorCoefficient coefficient);

uint16_t read_raw_pressure(void);
uint16_t read_raw_temperature(void);

#endif
