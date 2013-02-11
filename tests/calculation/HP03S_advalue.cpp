#include "CppUTest/TestHarness.h"

extern "C"
{
#include "stubs.h"
}

TEST_GROUP(HP03S)
{

	HP03S_Result measure_result;

	void setup()
	{
		Stub_SetupDefault();

		measure_result = HP03S_ERROR;

		UT_PTR_SET(HP03S_ReadSensorParameter, Mock_ReadSensorParameter);
		UT_PTR_SET(HP03S_ReadSensorCoefficient, Mock_ReadSensorCoefficient);
		UT_PTR_SET(HP03S_ReadPressure, Mock_ReadPressure);
		UT_PTR_SET(HP03S_ReadTemperature, Mock_ReadTemperature);

		HP03S_Result result = HP03S_Create();
		LONGS_EQUAL(result, HP03S_OK);
	}

	void teardown()
	{
		LONGS_EQUAL(measure_result, HP03S_OK);
		HP03S_Destroy();
	}
};

/* This is the example from the data sheet

   Step 1: (get temperature value)
   a) D2 >= C5: dUT= D2-C5 - ((D2-C5)/2^7) * ((D2-C5)/2^7) * A / 2^C
   b) D2  < C5: dUT= D2-C5 - ((D2-C5)/2^7) * ((D2-C5)/2^7) * B / 2^C

   Step 2: (calculate offset, sensitivity and final pressure value)
   OFF=(C2+(C4-1024)*dUT/2^14)*4
   SENS = C1+ C3*dUT/2^10
   X= SENS * (D1-7168)/2^14 - OFF
   P=X*10/2^5+C7
   • For altitude measurement system, recommend to use P=X*100/2^5+C7*10
     So that better altitude resolution can be achieved

   Step 3: (calculate temperature)
   T = 250 + dUT * C6 / 2 ^ 16-dUT/2^D

Example:
C1=29908
C2=3724
C3=312
C4=441
C5=9191
C6=3990
C7=2500
A=1
B=4
C=4
D=9
D1=30036
D2=4107
dUT = (4107-9191) - ((4107-9191)*(4107-9191)/128^2) * 4 / 2^4 = -5478
OFF = (3724 + (441-1024) * (-5478) / 2^14) * 4 =15675
SENS= 29908 + 312 * (-5478) / 2^10 = 28238
X= 28238 * (30036-7168) / 2^14 – 15675 = 23738

P= 23738 * 10 /2^5 + 2500 = 9918 = 991.8hpa
T= 250 + (-5478) * 3990 /2^16- (-5478/2^9) =-72 = -7.2°C
*/

TEST(HP03S, DataSheetExample)
{
	measure_result = HP03S_Measure();
	/* the data sheet calculation gives -7,2 degree as result
	 * but if you truncate the intermediate result of the first division
	 * like the compiler does you get -7,3 degree */
	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(9918, HP03S_GetPressure());
}

TEST(HP03S, TemperatureMin)
{
	Stub_SetupRawTemperature(0);
	measure_result = HP03S_Measure();

	LONGS_EQUAL(-367, HP03S_GetTemperature());
	LONGS_EQUAL(9031, HP03S_GetPressure())
}

TEST(HP03S, TemperatureMax)
{
	Stub_SetupRawTemperature(0xFFFF);
	measure_result = HP03S_Measure();

	LONGS_EQUAL(2857, HP03S_GetTemperature());
	LONGS_EQUAL(18735, HP03S_GetPressure())
}

TEST(HP03S, PressureMin)
{
	Stub_SetupRawPressure(0);
	measure_result = HP03S_Measure();

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(-6259, HP03S_GetPressure())
}

TEST(HP03S, PressureMax)
{
	Stub_SetupRawPressure(0xFFFF);
	measure_result = HP03S_Measure();

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(29038, HP03S_GetPressure())
}
