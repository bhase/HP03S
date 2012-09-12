#ifndef D_MockGPIO_H
#define D_MockGPIO_H

#include "GPIO.h"
#include "HP03S_internal.h"

void MockGPIO_Create(unsigned int maxExpectations);
void MockGPIO_Destroy(void);

void MockGPIO_Expect_SetXCLR_High();
void MockGPIO_Expect_SetXCLR_Low();
void MockGPIO_Expect_nTimesADRead(unsigned int count);

void MockGPIO_CheckExpectations(void);

#endif  /* D_FakeMockGPIO_H */
