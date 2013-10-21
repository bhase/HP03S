#ifndef D_HP03S_H
#define D_HP03S_H

#include <stdint.h>

/**********************************************************
 *
 * HP03S is a library for the Hope pressure sensor.
 * It requires a GPIO pin and an I2C interface
 * (see I2C.h and GPIO.h for details)
 *
 **********************************************************/

typedef int Temperature;
typedef int Pressure;

typedef int32_t HP03S_Bigint;

typedef enum {
	HP03S_OK = 0,
	HP03S_NoDevice,
	HP03S_DeviceError,
	HP03S_RangeError,
	HP03S_UNINITIALIZED,
	HP03S_ERROR,
} HP03S_Result;

HP03S_Result HP03S_Create(void);
void HP03S_Destroy(void);

Temperature HP03S_GetTemperature(void);
Pressure HP03S_GetPressure(void);

HP03S_Result HP03S_Measure(void);

#endif  /* D_FakeHP03S_H */
