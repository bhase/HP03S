#ifndef MY_MOCKS_H
#define MY_MOCKS_H

#include "HP03S_internal.h"

uint16_t Mock_HP03S_ReadTemperature(void);
uint16_t Mock_HP03S_ReadPressure(void);

uint8_t  Mock_HP03S_ReadSensorParameter(SensorParameter parameter);
uint16_t Mock_HP03S_ReadSensorCoefficient(SensorCoefficient coefficient);

#endif
