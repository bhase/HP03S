#include <string.h>
#include <stdarg.h>
#include "MockI2C.h"
#include "CppUTest/TestHarness_c.h"


typedef enum {
	I2C_READ,
	I2C_WRITE,
	I2C_RUN,
} ExpectationType;

typedef struct {
	ExpectationType expectation_type;
	uint16_t address;
	I2C_Result returnValue;
	uint8_t length;
	uint8_t *buffer;
} Expectation;


static Expectation *expectations = NULL;
static int last_used_expectation = 0;
static int last_recorded_expectation = 0;
static size_t max_expectations = 0;

static const char* unexpected_write = "unexpected write";
static const char* unexpected_run   = "unexpected run";
static const char* unexpected_read  = "unexpected read";


static void failWhenNoFreeExpectationLeft(void)
{
	if (last_recorded_expectation >= max_expectations) {
		FAIL_TEXT_C("too many expectations");
	}
}


static void failWhenAllrecordedExpectationsUsed(void)
{
	if (last_used_expectation >= last_recorded_expectation) {
		FAIL_TEXT_C("not enough expectations");
	}
}


static void failWhenExpectationIsNot(ExpectationType type, const char* message)
{
	if (expectations[last_used_expectation].expectation_type != type) {
		FAIL_TEXT_C(message);
	}
}


static void failWhenNotAllExpectionasUsed(void)
{
	if (last_used_expectation < last_recorded_expectation) {
		FAIL_TEXT_C("there are unused expectations");
	}
}


static void failWhenNotInitialized(void)
{
	if (expectations == NULL) {
		FAIL_TEXT_C("not initialized");
	}
}


static void failWhenRecordedAddressIsNot(uint16_t device_address)
{
	if (expectations[last_used_expectation].address != device_address) {
		FAIL_TEXT_C("device address mismatch");
	}
}


static void failWhenRecordedLengthIsNot(uint8_t length)
{
	if (expectations[last_used_expectation].length != length) {
		FAIL_TEXT_C("wrong length");
	}
}


static void failWhenRecordedBufferDiffers(uint8_t *buffer, uint8_t length)
{
	if (memcmp(expectations[last_used_expectation].buffer, buffer, length) != 0) {
		FAIL_TEXT_C("contents mismatch");
	}
}


static void recordExpectation(ExpectationType type, ...)
{
	va_list ap;
	va_start(ap, type);

	expectations[last_recorded_expectation].expectation_type = type;
	if (type == I2C_RUN) {
		I2C_Result res = va_arg(ap, int);
		expectations[last_recorded_expectation].returnValue = res;
	}
	else {
		uint16_t address = (uint16_t)va_arg(ap, int);
		uint8_t len = (uint8_t)va_arg(ap, int);
		uint8_t *buf = (uint8_t *)va_arg(ap, uint8_t *);

		expectations[last_recorded_expectation].address = address;
		expectations[last_recorded_expectation].length = len;
		expectations[last_recorded_expectation].buffer = malloc(len);
		memcpy(expectations[last_recorded_expectation].buffer, buf, len);
	}
	last_recorded_expectation++;

	va_end(ap);
}


void MockI2C_Create(size_t size)
{
	last_used_expectation = 0;
	last_recorded_expectation = 0;
	expectations = calloc(sizeof(Expectation), size);
	max_expectations = size;
}

void MockI2C_Destroy(void)
{
	int i = 0;
	for (i = 0; i < max_expectations; i++) {
		free(expectations[i].buffer);
	}
	free(expectations);
	expectations = NULL;
	max_expectations = 0;
}


void MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(uint16_t device_address,
						 uint8_t len,
						 const uint8_t *buffer)
{
	failWhenNotInitialized();
	failWhenNoFreeExpectationLeft();
	recordExpectation(I2C_READ, device_address, len, buffer);
}

void MockI2C_Expect_I2C_WriteTo_and_check_buffer(uint16_t device_address,
						 uint8_t len,
						 const uint8_t *buffer)
{
	failWhenNotInitialized();
	failWhenNoFreeExpectationLeft();
	recordExpectation(I2C_WRITE, device_address, len, buffer);
}

void MockI2C_Expect_I2C_Run_and_return(I2C_Result result)
{
	failWhenNotInitialized();
	failWhenNoFreeExpectationLeft();
	recordExpectation(I2C_RUN, result);
}

void MockI2C_CheckExpectations(void)
{
	failWhenNotAllExpectionasUsed();
}


/* I2C dummy implementation */

void I2C_ReadFrom(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
	failWhenAllrecordedExpectationsUsed();
	failWhenExpectationIsNot(I2C_READ, unexpected_read);
	failWhenRecordedAddressIsNot(device_address);
	failWhenRecordedLengthIsNot(length);
	memcpy(buffer, expectations[last_used_expectation].buffer, length);
	last_used_expectation++;
}

void I2C_WriteTo(uint16_t device_address, uint8_t length, uint8_t *buffer)
{
	failWhenAllrecordedExpectationsUsed();
	failWhenExpectationIsNot(I2C_WRITE, unexpected_write);
	failWhenRecordedAddressIsNot(device_address);
	failWhenRecordedLengthIsNot(length);
	failWhenRecordedBufferDiffers(buffer, length);
	last_used_expectation++;
}

I2C_Result I2C_Run()
{
	failWhenAllrecordedExpectationsUsed();
	failWhenExpectationIsNot(I2C_RUN, unexpected_run);
	return expectations[last_used_expectation++].returnValue;
}

