#include "MockI2C.h"
#include "CppUTest/TestHarness_c.h"

typedef struct {
	enum {
		I2C_READ,
		I2C_WRITE,
		I2C_RUN,
	} expectation_type;
	uint16_t address;
	I2C_Result returnValue;
	uint8_t length;
} Expectation;

static Expectation *expectations = NULL;
static int last_used_expectation = 0;
static int last_recorded_expectation = 0;
static size_t max_expectations = 0;

void MockI2C_Create(size_t size)
{
	last_used_expectation = 0;
	last_recorded_expectation = 0;
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
	if (last_recorded_expectation >= max_expectations) {
		FAIL_TEXT_C("too many expectations");
	}
	expectations[last_recorded_expectation].expectation_type = I2C_READ;
	expectations[last_recorded_expectation].address = device_address;
	expectations[last_recorded_expectation].length = len;
	last_recorded_expectation++;
}

void MockI2C_Expect_I2C_WriteTo_and_check_buffer(uint16_t device_address, uint8_t len, const uint8_t *buffer)
{
	if (last_recorded_expectation >= max_expectations) {
		FAIL_TEXT_C("too many expectations");
	}
	expectations[last_recorded_expectation].expectation_type = I2C_WRITE;
	expectations[last_recorded_expectation].address = device_address;
	expectations[last_recorded_expectation].length = len;
	last_recorded_expectation++;
}

void MockI2C_Expect_I2C_Run_and_return(I2C_Result result)
{
	if (last_recorded_expectation >= max_expectations) {
		FAIL_TEXT_C("too many expectations");
	}
	expectations[last_recorded_expectation].expectation_type = I2C_RUN;
	expectations[last_recorded_expectation].returnValue = result;
	last_recorded_expectation++;
}

void MockI2C_CheckExpectations(void)
{
	if (last_used_expectation < last_recorded_expectation) {
		FAIL_TEXT_C("there are unused expectations");
	}
}

/* I2C dummy implementation */

void I2C_ReadFrom(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
	if (last_used_expectation >= last_recorded_expectation) {
		FAIL_TEXT_C("not enough expectations");
	}
	if (expectations[last_used_expectation].expectation_type != I2C_READ) {
		FAIL_TEXT_C("unexpected read");
	}
	if (expectations[last_used_expectation].address != device_address) {
		FAIL_TEXT_C("device address mismatch");
	}
	if (expectations[last_used_expectation].length != length) {
		FAIL_TEXT_C("wrong length");
	}
	last_used_expectation++;
}

void I2C_WriteTo(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
	if (last_used_expectation >= last_recorded_expectation) {
		FAIL_TEXT_C("not enough expectations");
	}
	if (expectations[last_used_expectation].expectation_type != I2C_WRITE) {
		FAIL_TEXT_C("unexpected write");
	}
	if (expectations[last_used_expectation].address != device_address) {
		FAIL_TEXT_C("device address mismatch");
	}
	if (expectations[last_used_expectation].length != length) {
		FAIL_TEXT_C("wrong length");
	}
	last_used_expectation++;
}

I2C_Result I2C_Run()
{
	if (expectations[last_used_expectation].expectation_type != I2C_RUN) {
		FAIL_TEXT_C("unexpected run");
	}
	return expectations[last_used_expectation++].returnValue;
}

