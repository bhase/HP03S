#include "MockI2C.h"
#include "CppUTest/TestHarness_c.h"

typedef struct {
	enum {
		I2C_READ,
		I2C_WRITE,
	} expectation_type;
} Expectation;

static Expectation *expectations = NULL;
static int currently_used_expectation = 0;
static int currently_recorded_expectation = 0;
static size_t max_expectations = 0;

void MockI2C_Create(size_t size)
{
	currently_used_expectation = 0;
	currently_recorded_expectation = 0;
	expectations = malloc(sizeof(Expectation) * size);
	max_expectations = size;
}

void MockI2C_Destroy(void)
{
	free(expectations);
	expectations = NULL;
	max_expectations = 0;
}

void MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(uint16_t device_address, uint8_t len, uint8_t *buffer)
{
	if (currently_recorded_expectation >= max_expectations) {
		FAIL_TEXT_C("too many expectations");
	}
	expectations[currently_recorded_expectation++].expectation_type = I2C_READ;
}

void MockI2C_Expect_I2C_WriteTo_and_check_buffer(uint16_t device_address, uint8_t len, const uint8_t *buffer)
{
	if (currently_recorded_expectation >= max_expectations) {
		FAIL_TEXT_C("too many expectations");
	}
	expectations[currently_recorded_expectation++].expectation_type = I2C_WRITE;
}

void MockI2C_Expect_I2C_Run_and_return(I2C_Result result)
{
}

/* I2C dummy implementation */

void I2C_ReadFrom(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
	if (expectations[currently_used_expectation++].expectation_type != I2C_READ) {
		FAIL_TEXT_C("unexpected read");
	}
}

void I2C_WriteTo(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
	if (expectations[currently_used_expectation++].expectation_type != I2C_WRITE) {
		FAIL_TEXT_C("unexpected write");
	}
}

I2C_Result I2C_Run()
{
	return I2C_Ok;
}

