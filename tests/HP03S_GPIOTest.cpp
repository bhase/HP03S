
#include "CppUTest/TestHarness.h"

TEST_GROUP(HP03S_GPIO)
{
};

TEST(HP03S_GPIO, Measure)
{
	/* sequence: XCLR high -> read AD -> read AD -> XCLR low */

	/* expect that Measure() pulls XCLR high */
	/* after measurement XCLR must be low */
}

