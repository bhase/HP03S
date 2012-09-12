#include "mocks.h"
#include "CppUTestExt/MockSupport_c.h"

uint16_t Mock_HP03S_ReadSensorCoefficient(SensorCoefficient coefficient)
{
	mock_c()->actualCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", coefficient);
	return 0;
}

uint8_t Mock_HP03S_ReadSensorParameter(SensorParameter parameter)
{
	mock_c()->actualCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", parameter);
	return 0;
}
