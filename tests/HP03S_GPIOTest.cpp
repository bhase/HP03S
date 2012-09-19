
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "HP03S.h"
#include "MockGPIO.h"
}

/* assure that XCLR is in the right state
 * - low in idle state
 * - low while reading coefficients
 * - high before starting AD conversion */

TEST_GROUP(HP03S_GPIO)
{
	void setup(void)
	{
		MockGPIO_Create(12);
	}

	void teardown(void)
	{
		MockGPIO_CheckExpectations();
		MockGPIO_Destroy();
	}
};

TEST(HP03S_GPIO, Measure)
{
	/* sequence: XCLR high -> read AD -> read AD -> XCLR low */
	MockGPIO_Expect_SetXCLR_High();
	MockGPIO_Expect_nTimesADRead(2);
	MockGPIO_Expect_SetXCLR_Low();

	HP03S_Measure();
}

TEST(HP03S_GPIO, Init)
{
	MockGPIO_Expect_SetXCLR_Low();
	MockGPIO_Expect_nTimesEERead(11);

	HP03S_Create();
}
