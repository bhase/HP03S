extern "C"
{

#include "HP03S.h"
#include "HP03S_internal.h"
#include <string.h>
#include "CppUTestExt/MockSupport_c.h"

uint16_t MockHP03S_ReadSensorCoefficient(SensorCoefficient coefficient)
{
	mock_c()->actualCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", coefficient);
	return 0;
}

uint8_t MockHP03S_ReadSensorParameter(SensorParameter parameter)
{
	mock_c()->actualCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", parameter);
	return 0;
}

}

#include "CppUTest/TestHarness.h"

/* Init should do a range check on parameter and coefficients */
/* I2C error: device not responding */

TEST_GROUP(HP03S_Init)
{
	void setup()
	{
		UT_PTR_SET(HP03S_ReadSensorCoefficient, MockHP03S_ReadSensorCoefficient);
		UT_PTR_SET(HP03S_ReadSensorParameter, MockHP03S_ReadSensorParameter);
	}

	void teardown()
	{
		mock_c()->clear();
	}
};

TEST(HP03S_Init, Create)
{
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C1_SensitivityCoefficient);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C2_OffsetCoefficient);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C3_TemperatureCoefficientOfSensitivity);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C4_TemperatureCoefficientOfOffset);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C5_ReferenceTemperature);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C6_TemperatureCoefficientOfTemperature);
	mock_c()->expectOneCall("HP03S_ReadSensorCoefficient")
		->withIntParameters("coefficient", C7_OffsetFineTuning);

	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", SensorParameter_A);
	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", SensorParameter_B);
	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", SensorParameter_C);
	mock_c()->expectOneCall("HP03S_ReadSensorParameter")
		->withIntParameters("parameter", SensorParameter_D);

	HP03S_Create();

	mock_c()->checkExpectations();
}


extern "C"
{
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

}

TEST_GROUP(HP03S_Application)
{
	void setup()
	{
		UT_PTR_SET(HP03S_ReadTemperature, Mock_HP03S_ReadTemperature);
		UT_PTR_SET(HP03S_ReadPressure, Mock_HP03S_ReadPressure);
		HP03S_Create();
	}

	void teardown()
	{
		mock_c()->clear();
		HP03S_Destroy();
	}
};

/*
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

TEST(HP03S_Application, GetTemperature)
{
	/* we expect a temperature of -7,2 degree Celsius */
	CHECK(HP03S_GetTemperature() == -72);
}

TEST(HP03S_Application, GetPressure)
{
	/* we expect a pressure of 991,8 hPa */
	CHECK(HP03S_GetPressure() == 9918);
}

TEST(HP03S_Application, Measure)
{
	mock_c()->expectOneCall("HP03S_ReadTemperature");
	mock_c()->expectOneCall("HP03S_ReadPressure");

	HP03S_Measure();

	mock_c()->checkExpectations();
}

/* replace the return values of ReadTemperature and ReadPressure */
/* Replace the values of C1 - C7 and A - D */
/* erroneous values? */
/* device not responding */
/* assure that XCLR is in the right state!
 * - low in idle state
 * - low while reading coefficients
 * - high before starting AD conversion */

/*  For testing I2C:

    Coefficient  EEPROM ADDRESS
    -----------  --------------
    C1(MSB:LSB)  (16:17)
    C2(MSB:LSB)  (18:19)
    C3(MSB:LSB)  (20:21)
    C4(MSB:LSB)  (22:23)
    C5(MSB:LSB)  (24:25)
    C6(MSB:LSB)  (26:27)
    C7(MSB:LSB)  (28:29)
    A            (30)
    B            (31)
    C            (32)
    D            (33)
    */
