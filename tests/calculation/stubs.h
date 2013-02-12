
#ifndef _STUBS_H_
#define _STUBS_H_

#include "HP03S.h"
#include "HP03S_internal.h"

uint16_t Mock_ReadPressure(void);
uint16_t Mock_ReadTemperature(void);
uint16_t Mock_ReadSensorCoefficient(SensorCoefficient coefficient);
uint8_t Mock_ReadSensorParameter(SensorParameter param);

void Stub_SetupDefault(void);
void Stub_SetupRawTemperature(uint16_t temperature);
void Stub_SetupParameters(uint8_t *parameters);
void Stub_SetupCoefficients(uint16_t *coefficients);
void Stub_SetupRawPressure(uint16_t pressure);

#endif