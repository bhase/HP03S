
#ifndef _STUBS_H_
#define _STUBS_H_

#include "HP03S.h"
#include "HP03S_internal.h"

HP03S_Result Mock_ReadPressure(uint16_t *val);
HP03S_Result Mock_ReadTemperature(uint16_t *val);
HP03S_Result Mock_ReadSensorCoefficient(uint16_t *coefficient);
HP03S_Result Mock_ReadSensorParameter(uint8_t *param);

void Stub_SetupDefault(void);
void Stub_SetupRawTemperature(uint16_t temperature);
void Stub_SetupParameters(uint8_t *parameters);
void Stub_SetupCoefficients(uint16_t *coefficients);
void Stub_SetupRawPressure(uint16_t pressure);

#endif
