#include <string.h>
#include <stdio.h>
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
	I2C_Address address;
	I2C_Result returnValue;
	uint8_t length;
	uint8_t *buffer;
} Expectation;


static Expectation *expectations = NULL;
static size_t last_used_expectation = 0;
static size_t last_recorded_expectation = 0;
static size_t max_expectations = 0;

static const char* unexpected_write = "unexpected I2C_Write, expected %s";
static const char* unexpected_run   = "unexpected I2C_Run, expected %s";
static const char* unexpected_read  = "unexpected I2C_Read, expected %s";

static const char *wrong_write_length = "wrong length for I2C_Write: expected %d, got %d";
static const char *wrong_read_length = "wrong length for I2C_Read: expected %d, got %d";

static char* expectationTypeToString(ExpectationType et)
{
	switch (et) {
	case I2C_READ:
		return "I2C_Read";
	case I2C_WRITE:
		return "I2C_Write";
	case I2C_RUN:
		return "I2C_Run";
	default:
		return "unknown";
	}
}

static void failWhenNoFreeExpectationLeft(void)
{
	if (last_recorded_expectation >= max_expectations) {
		FAIL_TEXT_C("too many expectations, give bigger size to MockI2C_Create");
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
	char message_buffer[512];
	if (expectations[last_used_expectation].expectation_type != type) {
		snprintf(message_buffer, sizeof(message_buffer), message,
			 expectationTypeToString(expectations[last_used_expectation].expectation_type));
		FAIL_TEXT_C(message_buffer);
	}
}


static void failWhenNotAllExpectionasUsed(void)
{
	char buffer[1024];
	if (last_used_expectation < last_recorded_expectation) {
		size_t i = last_used_expectation;
		snprintf(buffer, sizeof(buffer), "there are unused expectations:\n");
		while (i < last_recorded_expectation) {
			snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
				 "\t* expected one call to %s\n",
				 expectationTypeToString(expectations[i].expectation_type));
			i++;
		}
		FAIL_TEXT_C(buffer);
	}
}


static void failWhenNotInitialized(void)
{
	if (expectations == NULL) {
		FAIL_TEXT_C("MockI2C is not initialized, call MockI2C_Create before");
	}
}


static void failWhenRecordedAddressIsNot(I2C_Address device_address)
{
	char buffer[512];
	if ((expectations[last_used_expectation].address | 1) != (device_address | 1)) {
		snprintf(buffer, sizeof(buffer), "device address mismatch, expected 0x%X, got 0x%X",
			 expectations[last_used_expectation].address, device_address);
		FAIL_TEXT_C(buffer);
	}
}


static void failWhenRecordedLengthIsNot(const char *message, uint8_t length)
{
	char buffer[512];
	if (expectations[last_used_expectation].length != length) {
		snprintf(buffer, sizeof(buffer), message,
			 expectations[last_used_expectation].length, length);
		FAIL_TEXT_C(buffer);
	}
}


static void failWhenRecordedBufferDiffers(uint8_t *buffer, uint8_t length)
{
	if (memcmp(expectations[last_used_expectation].buffer, buffer, length) != 0) {
		FAIL_TEXT_C("I2C_WriteTo: the output buffer contents do not match");
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
		I2C_Address address = (uint16_t)va_arg(ap, int);
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


void MockI2C_Expect_I2C_ReadFrom_and_fill_buffer(I2C_Address device_address,
						 uint8_t len,
						 const uint8_t *buffer)
{
	failWhenNotInitialized();
	failWhenNoFreeExpectationLeft();
	recordExpectation(I2C_READ, device_address, len, buffer);
}

void MockI2C_Expect_I2C_WriteTo_and_check_buffer(I2C_Address device_address,
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

void I2C_ReadFrom(I2C_Address device_address, uint8_t length, uint8_t *buffer)
{
	failWhenAllrecordedExpectationsUsed();
	failWhenExpectationIsNot(I2C_READ, unexpected_read);
	failWhenRecordedAddressIsNot(device_address);
	failWhenRecordedLengthIsNot(wrong_read_length, length);
	memcpy(buffer, expectations[last_used_expectation].buffer, length);
	last_used_expectation++;
}

void I2C_WriteTo(I2C_Address device_address, uint8_t length, uint8_t *buffer)
{
	failWhenAllrecordedExpectationsUsed();
	failWhenExpectationIsNot(I2C_WRITE, unexpected_write);
	failWhenRecordedAddressIsNot(device_address);
	failWhenRecordedLengthIsNot(wrong_write_length, length);
	failWhenRecordedBufferDiffers(buffer, length);
	last_used_expectation++;
}

I2C_Result I2C_Run()
{
	failWhenAllrecordedExpectationsUsed();
	failWhenExpectationIsNot(I2C_RUN, unexpected_run);
	return expectations[last_used_expectation++].returnValue;
}

