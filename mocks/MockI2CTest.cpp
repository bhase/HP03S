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

static I2C_Address device_address;
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

	MockI2C_CheckExpectations();
}

TEST(MockI2C, CanMatchExpectations)
{
	expectedErrors = 0;
	testFailureWith(CanMatchExpectations);
	fixture->assertPrintContains("OK");
}


static void I2C_Read_IgnoresLowBitInAddress(void)
{
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer((device_address | 1), 1, buffer);
	I2C_ReadFrom((I2C_Address)(device_address & ~1), 1, buffer);
}

TEST(MockI2C, I2C_Read_IgnoresLowBitInAddress)
{
	expectedErrors = 0;
	testFailureWith(I2C_Read_IgnoresLowBitInAddress);
	fixture->assertPrintContains("OK");
}


static void WriteWhenReadExpectedFails(void)
{
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 1, buffer);

	I2C_WriteTo(device_address, 1, buffer);
}

TEST(MockI2C, WriteWhenReadExpectedFails)
{
	testFailureWith(WriteWhenReadExpectedFails);
	fixture->assertPrintContains("unexpected I2C_Write");
}


static void ReadWhenWriteExpectedFails(void)
{
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 1, buffer);

	I2C_ReadFrom(device_address, 1, buffer);
}

TEST(MockI2C, ReadWhenWriteExpectedFails)
{
	testFailureWith(ReadWhenWriteExpectedFails);
	fixture->assertPrintContains("unexpected I2C_Read");
}


static void TooManyRead(void)
{
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 1, buffer);

	I2C_ReadFrom(device_address, 1, buffer);
	I2C_ReadFrom(device_address, 1, buffer);
}

TEST(MockI2C, TooManyRead)
{
	testFailureWith(TooManyRead);
	fixture->assertPrintContains("not enough expectations");
}


static void TooManyWrite(void)
{
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 1, buffer);

	I2C_WriteTo(device_address, 1, buffer);
	I2C_WriteTo(device_address, 1, buffer);
}

TEST(MockI2C, TooManyWrite)
{
	testFailureWith(TooManyWrite);
	fixture->assertPrintContains("not enough expectations");
}


static void TooManyRun(void)
{
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	I2C_Run();
	I2C_Run();
}

TEST(MockI2C, TooManyRun)
{
	testFailureWith(TooManyRun);
	fixture->assertPrintContains("not enough expectations");
}

static void WrongSequence(void)
{
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 1, buffer);
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 1, buffer);

	I2C_ReadFrom(device_address, 1, buffer);
	I2C_WriteTo(device_address, 1, buffer);
}

TEST(MockI2C, WrongSequence)
{
	testFailureWith(WrongSequence);
	fixture->assertPrintContains("unexpected I2C_Read");
}


static void TooManyExpectations(void)
{
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 1, buffer);
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 1, buffer);
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 1, buffer);
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 1, buffer);
}

TEST(MockI2C, TooManyExpectations)
{
	testFailureWith(TooManyExpectations);
	fixture->assertPrintContains("too many expectations");
}


static void NotAllExpectationsUsed(void)
{
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 1, buffer);
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	I2C_WriteTo(device_address, 1, buffer);

	MockI2C_CheckExpectations();
}

TEST(MockI2C, NotAllExpectationsUsed)
{
	testFailureWith(NotAllExpectationsUsed);
	fixture->assertPrintContains("unused expectations");
}


static void UnexpectedRun(void)
{
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 1, buffer);

	I2C_Run();
}

TEST(MockI2C, UnexpectedRun)
{
	testFailureWith(UnexpectedRun);
	fixture->assertPrintContains("unexpected I2C_Run");
}


static void I2C_Read_WrongParameter_Address(void)
{
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 1, buffer);

	I2C_ReadFrom((uint16_t)~device_address, 1, buffer);
}

TEST(MockI2C, I2C_Read_WrongParameter_Address)
{
	testFailureWith(I2C_Read_WrongParameter_Address);
	fixture->assertPrintContains("device address mismatch");
}


static void I2C_Write_WrongParameter_Address(void)
{
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 1, buffer);

	I2C_WriteTo((uint16_t)~device_address, 1, buffer);
}

TEST(MockI2C, I2C_Write_WrongParameter_Address)
{
	testFailureWith(I2C_Write_WrongParameter_Address);
	fixture->assertPrintContains("device address mismatch");
}


static void MockI2C_Run_returns_given_parameter(void)
{
	MockI2C_Expect_I2C_Run_and_return(I2C_Timeout);
	MockI2C_Expect_I2C_Run_and_return(I2C_AckFailure);
	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);

	LONGS_EQUAL(I2C_Run(), I2C_Timeout);
	LONGS_EQUAL(I2C_Run(), I2C_AckFailure);
	LONGS_EQUAL(I2C_Run(), I2C_Ok);
}

TEST(MockI2C, MockI2C_Run_returns_given_parameter)
{
	expectedErrors = 0;
	testFailureWith(MockI2C_Run_returns_given_parameter);
	fixture->assertPrintContains("OK");
}


static void I2C_Write_WronParameter_Length(void)
{
	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 4, buffer);

	I2C_WriteTo(device_address, 3, buffer);
}

TEST(MockI2C, I2C_Write_WronParameter_Length)
{
	testFailureWith(I2C_Write_WronParameter_Length);
	fixture->assertPrintContains("wrong length");
}


static void I2C_Read_WrongParameter_Length(void)
{
	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 3, buffer);

	I2C_ReadFrom(device_address, 4, buffer);
}

TEST(MockI2C, I2C_Read_WrongParameter_Length)
{
	testFailureWith(I2C_Read_WrongParameter_Length);
	fixture->assertPrintContains("wrong length");
}


static void I2C_Read_FillsBufferAsGiven(void)
{
	uint8_t output_buffer[3];

	buffer[0] = 0x55;
	buffer[1] = 0xAA;
	buffer[2] = 0x77;

	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 3, buffer);
	I2C_ReadFrom(device_address, 3, output_buffer);
	CHECK(memcmp(buffer, output_buffer, 3) == 0);
}

TEST(MockI2C, I2C_Read_FillsBufferAsGiven)
{
	expectedErrors = 0;
	testFailureWith(I2C_Read_FillsBufferAsGiven);
	fixture->assertPrintContains("OK");
}


static void I2C_Write_ChecksBuffer(void)
{
	uint8_t input_buffer[3] = {0xA, 0xB, 0xC};
	buffer[0] = 1;

	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 3, input_buffer);

	I2C_WriteTo(device_address, 3, buffer);
}

TEST(MockI2C, I2C_Write_ChecksBuffer)
{
	testFailureWith(I2C_Write_ChecksBuffer);
	fixture->assertPrintContains("contents do not match");
}


static void NotInitialized_Write(void)
{
	MockI2C_Destroy();

	MockI2C_Expect_I2C_WriteTo_and_check_buffer(device_address, 1, buffer);
}

TEST(MockI2C, NotInitialized_Write)
{
	testFailureWith(NotInitialized_Write);
	fixture->assertPrintContains("not initialized");
}


static void NotInitialized_Read(void)
{
	MockI2C_Destroy();

	MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(device_address, 1, buffer);
}

TEST(MockI2C, NotInitialized_Read)
{
	testFailureWith(NotInitialized_Read);
	fixture->assertPrintContains("not initialized");
}


static void NotInitialized_Run(void)
{
	MockI2C_Destroy();

	MockI2C_Expect_I2C_Run_and_return(I2C_Ok);
}

TEST(MockI2C, NotInitialized_Run)
{
	testFailureWith(NotInitialized_Run);
	fixture->assertPrintContains("not initialized");
}
