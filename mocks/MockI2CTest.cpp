extern "C"
{
#include "MockI2C.h"
#include <string.h>
}

#include "CppUTest/TestHarness.h"
#include "CppUTest/TestOutput.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTest/TestTestingFixture.h"
#include "CppUTest/PlatformSpecificFunctions.h"

static uint16_t device_address;
static uint8_t buffer[4];

TEST_GROUP(MockI2C)
{
	TestTestingFixture* fixture;
	int expectedErrors;

	void setup()
	{
		device_address = 0xEE;
		memset(buffer, 0, sizeof(buffer));
		fixture = new TestTestingFixture();
		expectedErrors = 1;
		MockI2C_Create(3);
	}

	void teardown()
	{
		MockI2C_Destroy();
		delete fixture;
	}

	void testFailureWith(void (*method)())
	{
		fixture->setTestFunction(method);
		fixture->runAllTests();
		LONGS_EQUAL(expectedErrors, fixture->getFailureCount());
	}

};

static void CanMatchExpectations(void)
{
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 1, buffer);
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 2, buffer);
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	I2C_ReadFrom(device_address, 1, buffer);
	I2C_WriteTo(device_address, 2, buffer);
	LONGS_EQUAL(I2C_Run(), I2C_Ok);
}

TEST(MockI2C, CanMatchExpectations)
{
	expectedErrors = 0;
	testFailureWith(CanMatchExpectations);
	fixture->assertPrintContains("OK");
}

/* what could go wrong?
 * - read if write expected
 * - write if read expected
 * - too many read
 * - too many write
 * - not enough read
 * - not enough write
 * - wrong parameter:
 *   - device address
 *   - length
 *   - for writing buffer content - at least some byte (TODO ignore flag)
 */
