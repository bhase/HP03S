#ifndef D_MockI2C_H
#define D_MockI2C_H

/**********************************************************
 *
 * MockI2C is responsible for ...
 *
 **********************************************************/

#include "I2C.h"

void MockI2C_Create(size_t size);
void MockI2C_Destroy(void);

void MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(uint16_t device_address, uint8_t len, uint8_t *fillContents);
void MockI2C_Expect_I2C_WriteTo_and_check_buffer(uint16_t device_address, uint8_t len, const uint8_t *buffer);
void MockI2C_Expect_I2C_Run_and_return(I2C_Result result);

void MockI2C_CheckExpectations(void);
#endif  /* D_FakeMockI2C_H */
