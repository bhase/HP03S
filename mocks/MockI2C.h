#ifndef D_MOCKI2C_H
#define D_MOCKI2C_H

/**********************************************************
 *
 * MockI2C is responsible for replacing real I2C
 * interaction. It provides possibities to simulate
 * successful transactions as well as errors.
 *
 * Usage: Create a mock with MockI2C_Create,
 * feed expected input with MockI2C_Expect_*
 * and run your code under test inside CppUTest.
 * If your code behaves well, everything is fine.
 * If not, you will get an assertion. To see,
 * if your code made all expected calls, use
 * MockI2C_CheckExpectations. A good place can
 * be your teardown method ;-)
 *
 * Limitations: Currently there is a hard
 * implementation of I2C_Run, I2C_WriteTo and I2C_ReadFrom
 * in the mock so that real implementations will not
 * be linked in. This is no problem for HP03S, which
 * does not provide implementations for this API.
 *
 **********************************************************/

#include "I2C.h"

void MockI2C_Create(size_t size);
void MockI2C_Destroy(void);

void MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(I2C_Address device_address,
						 uint8_t len,
						 const uint8_t *fillContents);
void MockI2C_Expect_I2C_WriteTo_and_check_buffer(I2C_Address device_address,
						 uint8_t len,
						 const uint8_t *buffer);
void MockI2C_Expect_I2C_Run_and_return(I2C_Result result);

void MockI2C_CheckExpectations(void);
#endif  /* D_MOCKI2C_H */
