extern "C"
{
#include "MockGPIO.h"
}

#include "CppUTest/TestHarness.h"
#include "CppUTest/TestOutput.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTest/TestTestingFixture.h"
#include "CppUTest/PlatformSpecificFunctions.h"

TEST_GROUP(MockGPIO)
{
    TestTestingFixture* fixture;
    int expectedErrors;

    void setup()
    {
        fixture = new TestTestingFixture();
	MockGPIO_Create(3);
        expectedErrors = 1;
    }

    void teardown()
    {
	MockGPIO_Destroy();
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
	MockGPIO_Expect_SetXCLR_High();
	MockGPIO_Expect_nTimesADRead(2);
	MockGPIO_Expect_SetXCLR_Low();

	GPIO_SetXCLR_High();
	HP03S_ReadPressure();
	HP03S_ReadTemperature();
	GPIO_SetXCLR_Low();

	MockGPIO_CheckExpectations();
}

TEST(MockGPIO, CanMatchExpectations)
{
	expectedErrors = 0;
	testFailureWith(CanMatchExpectations);
	fixture->assertPrintContains("OK");
}
