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
} SensorCoefficient;

uint8_t read_sensor_parameter(SensorParameter param);
extern uint16_t (*HP03S_ReadSensorCoefficient)(SensorCoefficient coefficient);
uint16_t read_raw_pressure(void);
uint16_t read_raw_temperature(void);

#endif
