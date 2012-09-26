#include "CppUTest/TestHarness.h"

extern "C"
{
#include "HP03S.h"
#include "HP03S_internal.h"

static uint16_t sensor_coefficients[7];
static uint8_t sensor_parameters[4];

static uint16_t ad_temperature = 0;
static uint16_t ad_pressure = 0;

static uint16_t Mock_ReadPressure(void)
{
	return ad_pressure;
}

static uint16_t Mock_ReadTemperature(void)
{
	return ad_temperature;
}

static uint16_t Mock_ReadSensorCoefficient(SensorCoefficient coefficient)
{
	return sensor_coefficients[coefficient];
}

static uint8_t Mock_ReadSensorParameter(SensorParameter param)
{
	return sensor_parameters[param];
}

}

TEST_GROUP(HP03S)
{

	void setup_default_coefficients(void)
	{
		sensor_coefficients[C1_SensitivityCoefficient] = 29908;
		sensor_coefficients[C2_OffsetCoefficient] = 3724;
		sensor_coefficients[C3_TemperatureCoefficientOfSensitivity] = 312;
		sensor_coefficients[C4_TemperatureCoefficientOfOffset] = 441;
		sensor_coefficients[C5_ReferenceTemperature] = 9191;
		sensor_coefficients[C6_TemperatureCoefficientOfTemperature] = 3990;
		sensor_coefficients[C7_OffsetFineTuning] = 2500;
	}

	void setup_default_parameter(void)
	{
		sensor_parameters[SensorParameter_A] = 1;
		sensor_parameters[SensorParameter_B] = 4;
		sensor_parameters[SensorParameter_C] = 4;
		sensor_parameters[SensorParameter_D] = 9;
	}

	void setup_default_ad_values(void)
	{
		ad_temperature = 4107;
		ad_pressure = 30036;
	}

	void setup()
	{
		setup_default_parameter();
		setup_default_coefficients();
		setup_default_ad_values();

		UT_PTR_SET(HP03S_ReadSensorParameter, Mock_ReadSensorParameter);
		UT_PTR_SET(HP03S_ReadSensorCoefficient, Mock_ReadSensorCoefficient);
		UT_PTR_SET(HP03S_ReadPressure, Mock_ReadPressure);
		UT_PTR_SET(HP03S_ReadTemperature, Mock_ReadTemperature);

		HP03S_Create();
	}

	void teardown()
	{
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

/* Tests:
 * C1 0x100, 0xFFFF
 * C2 0x0, 0x1FFF
 * C3 0x0, 0x400
 * C4 0x0, 0x1000
 * C5 0x1000, 0xFFFF
 * C6 0x0, 0x4000
 * C7 0x960, 0xA28
 * A 0x1, 0x3F
 * B 0x1, 0x3F
 * C 0x1, 0xF
 * D 0x1, 0xF
 * pressure 0x0, 0xFFFF
 * temperature 0x0, 0xFFFF
 * All values like the data sheet example
 * some random combination
 */

TEST(HP03S, DataSheetExample)
{
	HP03S_Measure();
	/* the data sheet calculation gives -7,2 degree as result
	 * but if you truncate the intermediate result of the first division
	 * like the compiler does you get -7,3 degree */
	LONGS_EQUAL(-73, HP03S_GetTemperature());
	/* we expect a pressure of 991,8 hPa */
	LONGS_EQUAL(9918, HP03S_GetPressure());
}

TEST(HP03S, TemperatureMin)
{
	ad_temperature = 0;
	HP03S_Measure();

	/* for an temperature ad value of 0 a temperature
	 * of -36,7 degree Celsius is expected */
	LONGS_EQUAL(-367, HP03S_GetTemperature());
	/* pressure should be 903,1 hPa */
	LONGS_EQUAL(9031, HP03S_GetPressure())
}

TEST(HP03S, TemperatureMax)
{
	ad_temperature = 0xFFFF;
	HP03S_Measure();

	LONGS_EQUAL(2857, HP03S_GetTemperature());
	LONGS_EQUAL(18735, HP03S_GetPressure())
}


TEST(HP03S, PressureMin)
{
	ad_pressure = 0;
	HP03S_Measure();

	LONGS_EQUAL(-73, HP03S_GetTemperature());
	LONGS_EQUAL(-6259, HP03S_GetPressure())
}

/* replace the return values of ReadTemperature and ReadPressure */
/* Replace the values of C1 - C7 and A - D */
/* erroneous values? */
/* device not responding */

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
