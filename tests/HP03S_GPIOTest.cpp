
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "HP03S.h"
#include "HP03S_internal.h"
#include "CppUTestExt/MockSupport_c.h"

static bool gpio_testing = 0;
static bool xclr_state = 0;

enum {
	XCLR_LOW,
	XCLR_HIGH,
};

void GPIO_SetXCLR_High(void)
{
	xclr_state = XCLR_HIGH;
	if (gpio_testing == 1)
		mock_c()->actualCall("GPIO_SetXCLR_High");
}

void GPIO_SetXCLR_Low(void)
{
	xclr_state = XCLR_LOW;
	if (gpio_testing == 1)
		mock_c()->actualCall("GPIO_SetXCLR_Low");
}

static HP03S_Result Mock_ReadSensorCoefficient(uint16_t *coefficient)
{
	CHECK(xclr_state == XCLR_LOW);
	mock_c()->actualCall("HP03S_ReadSensorCoefficient");
	return HP03S_OK;
}

static HP03S_Result Mock_ReadSensorParameter(uint8_t *parameter)
{
	CHECK(xclr_state == XCLR_LOW);
	mock_c()->actualCall("HP03S_ReadSensorParameter");
	return HP03S_OK;
}

static HP03S_Result Mock_ReadPressure(uint16_t *val)
{
	CHECK(xclr_state == XCLR_HIGH);
	mock_c()->actualCall("HP03S_ReadPressure");
	return HP03S_OK;
}

static HP03S_Result Mock_ReadTemperature(uint16_t *val)
{
	CHECK(xclr_state == XCLR_HIGH);
	mock_c()->actualCall("HP03S_ReadTemperature");
	return HP03S_OK;
}

}

/* assure that XCLR is in the right state
 * - low in idle state
 * - low while reading coefficients
 * - high before starting AD conversion */

TEST_GROUP(HP03S_GPIO)
{
	void setup(void)
	{
		UT_PTR_SET(HP03S_ReadSensorCoefficient, Mock_ReadSensorCoefficient);
		UT_PTR_SET(HP03S_ReadSensorParameter, Mock_ReadSensorParameter);
		UT_PTR_SET(HP03S_ReadPressure, Mock_ReadPressure);
		UT_PTR_SET(HP03S_ReadTemperature, Mock_ReadTemperature);

		gpio_testing = 1;
		xclr_state = 2;
	}

	void teardown(void)
	{
		CHECK(xclr_state == XCLR_LOW);
		gpio_testing = 0;

		mock_c()->checkExpectations();
		mock_c()->clear();
	}
};

TEST(HP03S_GPIO, Measure)
{
	/* call sequence is checked in mocks */
	mock_c()->expectOneCall("GPIO_SetXCLR_High");
	mock_c()->expectOneCall("HP03S_ReadTemperature");
	mock_c()->expectOneCall("HP03S_ReadPressure");
	mock_c()->expectOneCall("GPIO_SetXCLR_Low");

	HP03S_Measure();
}

TEST(HP03S_GPIO, Init)
{
	/* call sequence is checked in mocks */
	mock_c()->expectOneCall("GPIO_SetXCLR_Low");
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient");
	mock_c()->expectOneCall("HP03S_ReadSensorParameter");

	HP03S_Create();
}
