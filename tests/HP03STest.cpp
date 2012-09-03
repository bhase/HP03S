extern "C"
{
#include "HP03S.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(HP03S)
{
	void setup()
	{
		HP03S_Create();
	}

	void teardown()
	{
		HP03S_Destroy();
	}
};

TEST(HP03S, Create)
{
	/* what should init do?
	 * it can read all coefficients from i2c eeprom
	 * - C1 to C7
	 * - A, B, C, D (these names come from the data sheet)
	 * and it should pull XCLR low while doing this

	 Coefficient  EEPROM ADDRESS
	 -----------  --------------
	 C1(MSB:LSB)  (16:17)
	 C2(MSB:LSB)  (18:19)
	 C3(MSB:LSB)  (20:21)
	 C4(MSB:LSB)  (22:23)
	 C5(MSB:LSB)  (24:25)
	 C6(MSB:LSB)  (26:27)
	 C7(MSB:LSB)  (28:29)
	 A            (30)
	 B            (31)
	 C            (32)
	 D          (33)
	 */
}

TEST(HP03S, GetTemperature)
{
	/* we expect a temperature of -7,2 degree Celsius */
	CHECK(HP03S_GetTemperature() == -72);
}

TEST(HP03S, GetPressure)
{
	/* we expect a pressure of 991,8 hPa */
	CHECK(HP03S_GetPressure() == 9918);
}
