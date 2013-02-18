#include "MockGPIO.h"
#include "CppUTest/TestHarness_c.h"
#include <stdio.h>

static HP03S_Result Mock_ReadPressure(uint16_t *val);
static HP03S_Result (*saved_ReadPressure)(uint16_t *val);
static HP03S_Result Mock_ReadTemperature(uint16_t *val);
static HP03S_Result (*saved_ReadTemperature)(uint16_t *val);

static uint8_t (*saved_ReadSensorParameter)(SensorParameter param);
static uint8_t Mock_ReadSensorParameter(SensorParameter param);
static uint16_t (*saved_ReadSensorCoefficient)(SensorCoefficient coefficient);
static uint16_t Mock_ReadSensorCoefficient(SensorCoefficient coefficient);

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
		EE_READ,
	} type;
} Expectation;

static Expectation *expectations = NULL;

static char *unexpected_pressure_read = "unexpected pressure read";
static char *unexpected_temperature_read = "unexpected temperature read";
static char *unexpected_xclr_high = "unexpected XCLR high";
static char *unexpected_xclr_low = "unexpected XCLR low";
static char *unexpected_parameter_read = "unexpected parameter read";
static char *unexpected_coefficient_read = "unexpected coefficient read";

static void fail(char *message)
{
	FAIL_TEXT_C(message);
}

static void failWhenNotInitialized(void)
{
	if (expectations == NULL)
		fail("do not use an uninitialized GPIO mock");
}

static void failWhenNoRoomLeft(void)
{
	failWhenNotInitialized();
	if (used_expectations >= max_expectations)
		fail("too many expectations");
}


void MockGPIO_Create(unsigned int maxExpectations)
{
	saved_ReadPressure = HP03S_ReadPressure;
	saved_ReadTemperature = HP03S_ReadTemperature;

	HP03S_ReadPressure = Mock_ReadPressure;
	HP03S_ReadTemperature = Mock_ReadTemperature;

	saved_ReadSensorCoefficient = HP03S_ReadSensorCoefficient;
	saved_ReadSensorParameter = HP03S_ReadSensorParameter;

	HP03S_ReadSensorCoefficient = Mock_ReadSensorCoefficient;
	HP03S_ReadSensorParameter = Mock_ReadSensorParameter;

	expectations = calloc(maxExpectations, sizeof(Expectation));
	if (expectations == NULL)
	{
		perror("calloc failed");
		abort();
	}
	max_expectations = maxExpectations;
	used_expectations = 0;
	checked_expectations = 0;
}

void MockGPIO_Destroy(void)
{
	HP03S_ReadPressure = saved_ReadPressure;
	HP03S_ReadTemperature = saved_ReadTemperature;

	HP03S_ReadSensorParameter = saved_ReadSensorParameter;
	HP03S_ReadSensorCoefficient = saved_ReadSensorCoefficient;

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

void MockGPIO_Expect_nTimesEERead(unsigned int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		failWhenNoRoomLeft();
		expectations[used_expectations].type = EE_READ;
		used_expectations++;
	}
}

void MockGPIO_CheckExpectations(void)
{
	failWhenNotInitialized();
	if (checked_expectations < used_expectations)
		fail("not all expectations used");
}

/*
 * mock the functions to read AD values
 */

static HP03S_Result Mock_ReadPressure(uint16_t *val)
{
	if (expectations[checked_expectations].type != AD_READ)
		fail(unexpected_pressure_read);
	checked_expectations++;
	return HP03S_OK;
}

static HP03S_Result Mock_ReadTemperature(uint16_t *val)
{
	if (expectations[checked_expectations].type != AD_READ)
		fail(unexpected_temperature_read);
	checked_expectations++;
	return HP03S_OK;
}

/*
 * mock the sensor parameter read functions
 */

static uint8_t Mock_ReadSensorParameter(SensorParameter param)
{
	if (expectations[checked_expectations].type != EE_READ)
		fail(unexpected_parameter_read);
	checked_expectations++;
	return 0;
}

static uint16_t Mock_ReadSensorCoefficient(SensorCoefficient coefficient)
{
	if (expectations[checked_expectations].type != EE_READ)
		fail(unexpected_coefficient_read);
	checked_expectations++;
	return 0;
}

/*       _\|/_
         (o o)
 +----oOO-{_}-OOo------------------------------------------------------+
 |                                                                     |
 | These are the only implementations of GPIO in the module.           |
 | Since they may be called outside of a mock, they should             |
 | not fail when not initialized.                                      |
 |                                                                     |
 +--------------------------------------------------------------------*/

void GPIO_SetXCLR_High(void)
{
	if (expectations == NULL)
		return;
	if (expectations[checked_expectations].type != XCLR_HIGH)
		fail(unexpected_xclr_high);
	checked_expectations++;
}

void GPIO_SetXCLR_Low(void)
{
	if (expectations == NULL)
		return;
	if (expectations[checked_expectations].type != XCLR_LOW)
		fail(unexpected_xclr_low);
	checked_expectations++;
}
