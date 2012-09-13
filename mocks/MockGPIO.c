#include "MockGPIO.h"
#include "CppUTest/TestHarness_c.h"

static uint16_t Mock_ReadPressure(void);
static uint16_t (*saved_ReadPressure)(void);
static uint16_t Mock_ReadTemperature(void);
static uint16_t (*saved_ReadTemperature)(void);

static unsigned int used_expectations = 0;
static unsigned int max_expectations = 0;
static unsigned int checked_expectations = 0;

typedef struct expectation
{
	enum
	{
		XCLR_HIGH,
		XCLR_LOW,
		AD_READ,
	} type;
} Expectation;

static Expectation *expectations = NULL;

static void fail(char *message)
{
	FAIL_TEXT_C(message);
}

void MockGPIO_Create(unsigned int maxExpectations)
{
	saved_ReadPressure = HP03S_ReadPressure;
	saved_ReadTemperature = HP03S_ReadTemperature;

	HP03S_ReadPressure = Mock_ReadPressure;
	HP03S_ReadTemperature = Mock_ReadTemperature;

	expectations = calloc(maxExpectations, sizeof(Expectation));
	max_expectations = maxExpectations;
	used_expectations = 0;
	checked_expectations = 0;
}

void MockGPIO_Destroy(void)
{
	HP03S_ReadPressure = saved_ReadPressure;
	HP03S_ReadTemperature = saved_ReadTemperature;

	free(expectations);
	expectations = NULL;
}

void MockGPIO_Expect_SetXCLR_Low(void)
{
	expectations[used_expectations].type = XCLR_LOW;
	used_expectations++;
}

void MockGPIO_Expect_SetXCLR_High(void)
{
	expectations[used_expectations].type = XCLR_HIGH;
	used_expectations++;
}

void MockGPIO_Expect_nTimesADRead(unsigned int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		expectations[used_expectations].type = AD_READ;
		used_expectations++;
	}
}

void MockGPIO_CheckExpectations(void)
{
}

/*
 * mock the functions
 */

static uint16_t Mock_ReadPressure(void)
{
	if (expectations[checked_expectations].type != AD_READ)
		fail("unexpected pressure read");
	checked_expectations++;
	return 0;
}

static uint16_t Mock_ReadTemperature(void)
{
	if (expectations[checked_expectations].type != AD_READ)
		fail("unexpected temperature read");
	checked_expectations++;
	return 0;
}

void GPIO_SetXCLR_High(void)
{
	if (expectations[checked_expectations].type != XCLR_HIGH)
		fail("unexpected XCLR high");
	checked_expectations++;
}

void GPIO_SetXCLR_Low(void)
{
	if (expectations[checked_expectations].type != XCLR_LOW)
		fail("unexpected XCLR low");
	checked_expectations++;
}
