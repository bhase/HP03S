#include "MockGPIO.h"
#include "CppUTest/TestHarness_c.h"

static uint16_t Mock_ReadPressure(void);
static uint16_t (*saved_ReadPressure)(void);
static uint16_t Mock_ReadTemperature(void);
static uint16_t (*saved_ReadTemperature)(void);

static int expectation_count = 0;

static void fail(char *message)
{
	FAIL_TEXT_C(message);
}

void MockGPIO_Create(int maxExpectations)
{
	saved_ReadPressure = HP03S_ReadPressure;
	saved_ReadTemperature = HP03S_ReadTemperature;

	HP03S_ReadPressure = Mock_ReadPressure;
	HP03S_ReadTemperature = Mock_ReadTemperature;

	expectation_count = 0;
}

void MockGPIO_Destroy(void)
{
	HP03S_ReadPressure = saved_ReadPressure;
	HP03S_ReadTemperature = saved_ReadTemperature;
}

void MockGPIO_Expect_SetXCLR_Low(void)
{
	expectation_count++;
}

void MockGPIO_Expect_SetXCLR_High(void)
{
	expectation_count++;
}

void MockGPIO_Expect_nTimesADRead(int count)
{
	expectation_count += count;
}

void MockGPIO_CheckExpectations(void)
{
	if (expectation_count == 0)
		return;
	fail("got unused expectations");
}

static uint16_t Mock_ReadPressure(void)
{
	expectation_count--;
	return 0;
}

static uint16_t Mock_ReadTemperature(void)
{
	expectation_count--;
	return 0;
}

void GPIO_SetXCLR_High(void)
{
	expectation_count--;
}

void GPIO_SetXCLR_Low(void)
{
	expectation_count--;
}
