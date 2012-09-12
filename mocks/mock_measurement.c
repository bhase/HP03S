#include "mocks.h"
#include "CppUTestExt/MockSupport_c.h"

uint16_t Mock_HP03S_ReadTemperature(void)
{
	mock_c()->actualCall("HP03S_ReadTemperature");
	return 0;
}

uint16_t Mock_HP03S_ReadPressure(void)
{
	mock_c()->actualCall("HP03S_ReadPressure");
	return 0;
}

