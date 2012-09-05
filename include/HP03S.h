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

void HP03S_Create(void);
void HP03S_Destroy(void);

Temperature HP03S_GetTemperature(void);
Pressure HP03S_GetPressure(void);

#endif  /* D_FakeHP03S_H */
