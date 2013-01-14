#ifndef D_HP03S_H
#define D_HP03S_H

#include <stdint.h>

/**********************************************************
 *
 * HP03S is responsible for ...
 *
 **********************************************************/

typedef int Temperature;
typedef int Pressure;

typedef enum {
	HP03S_OK = 0,
	HP03S_ERROR,
} HP03S_Result;

HP03S_Result HP03S_Create(void);
void HP03S_Destroy(void);

Temperature HP03S_GetTemperature(void);
Pressure HP03S_GetPressure(void);

HP03S_Result HP03S_Measure(void);

#endif  /* D_FakeHP03S_H */
