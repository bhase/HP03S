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
	MockGPIO_Create(4);
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

static void ReadPressureWhenXCLRHighExpectedFails(void)
{
	MockGPIO_Expect_SetXCLR_High();

	HP03S_ReadPressure();
}

TEST(MockGPIO, ReadPressureWhenXCLRHighExpectedFails)
{
	testFailureWith(ReadPressureWhenXCLRHighExpectedFails);
	fixture->assertPrintContains("unexpected pressure read");
}

static void ReadTemperatureWhenXCLRHighExpectedFails(void)
{
	MockGPIO_Expect_SetXCLR_High();

	HP03S_ReadTemperature();
}

TEST(MockGPIO, ReadTemperatureWhenXCLRHighExpectedFails)
{
	testFailureWith(ReadTemperatureWhenXCLRHighExpectedFails);
	fixture->assertPrintContains("unexpected temperature read");
}

static void XCLRLowWhenXCLRHighExpectedFails(void)
{
	MockGPIO_Expect_SetXCLR_High();

	GPIO_SetXCLR_Low();
}

TEST(MockGPIO, XCLRLowWhenXCLRHighExpectedFails)
{
	testFailureWith(XCLRLowWhenXCLRHighExpectedFails);
	fixture->assertPrintContains("unexpected XCLR low");
}

static void XCLRHighWhenLowExpectedFails(void)
{
	MockGPIO_Expect_SetXCLR_Low();

	GPIO_SetXCLR_High();
}

TEST(MockGPIO, XCLRHighWhenLowExpectedFails)
{
	testFailureWith(XCLRHighWhenLowExpectedFails);
	fixture->assertPrintContains("unexpected XCLR high");
}

static void TooManyExpectations(void)
{
	MockGPIO_Expect_SetXCLR_Low();
	MockGPIO_Expect_nTimesADRead(1);
	MockGPIO_Expect_SetXCLR_High();
	MockGPIO_Expect_nTimesADRead(1);
	MockGPIO_Expect_SetXCLR_Low();
}

TEST(MockGPIO, TooManyExpectations)
{
	testFailureWith(TooManyExpectations);
	fixture->assertPrintContains("too many expectations");
}

static void NotAllExpectationsUsed(void)
{
	MockGPIO_Expect_SetXCLR_Low();
	MockGPIO_Expect_nTimesADRead(1);
	MockGPIO_CheckExpectations();
}

TEST(MockGPIO, NotAllExpectationsUsed)
{
	testFailureWith(NotAllExpectationsUsed);
	fixture->assertPrintContains("not all expectations used");
}
