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

static char *unexpected_pressure_read = "unexpected pressure read";
static char *unexpected_temperature_read = "unexpected temperature read";
static char *unexpected_xclr_high = "unexpected XCLR high";
static char *unexpected_xclr_low = "unexpected XCLR low";

static void fail(char *message)
{
	FAIL_TEXT_C(message);
}

static void failWhenNoRoomLeft(void)
{
	if (used_expectations >= max_expectations)
		fail("too many expectations");
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
	failWhenNoRoomLeft();
	expectations[used_expectations].type = XCLR_LOW;
	used_expectations++;
}

void MockGPIO_Expect_SetXCLR_High(void)
{
	failWhenNoRoomLeft();
	expectations[used_expectations].type = XCLR_HIGH;
	used_expectations++;
}

void MockGPIO_Expect_nTimesADRead(unsigned int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		failWhenNoRoomLeft();
		expectations[used_expectations].type = AD_READ;
		used_expectations++;
	}
}

void MockGPIO_CheckExpectations(void)
{
	if (used_expectations < max_expectations)
		fail("not all expectations used");
}

/*
 * mock the functions
 */

static uint16_t Mock_ReadPressure(void)
{
	if (expectations[checked_expectations].type != AD_READ)
		fail(unexpected_pressure_read);
	checked_expectations++;
	return 0;
}

static uint16_t Mock_ReadTemperature(void)
{
	if (expectations[checked_expectations].type != AD_READ)
		fail(unexpected_temperature_read);
	checked_expectations++;
	return 0;
}

void GPIO_SetXCLR_High(void)
{
	if (expectations[checked_expectations].type != XCLR_HIGH)
		fail(unexpected_xclr_high);
	checked_expectations++;
}

void GPIO_SetXCLR_Low(void)
{
	if (expectations[checked_expectations].type != XCLR_LOW)
		fail(unexpected_xclr_low);
	checked_expectations++;
}
