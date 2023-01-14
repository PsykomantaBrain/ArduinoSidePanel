

#include "Joystick.h"
#include "Mouse.h"

#define sign(x) ((x) > 0 ? 1: ((x) < 0 ? -1 : 0))

//High impedance pin mode
#define OUTPUT_OFF INPUT_PULLUP

// axis output 
#define JOYSTICK_RANGE_MIN 0
#define JOYSTICK_RANGE_MAX 1023

// input pins for LX/Y and RX/Y axes (analogs 0-3)
#define PIN_A_LX 0
#define PIN_A_LY 1
#define PIN_A_RX 2
#define PIN_A_RY 3


//input pins for slew (thumbstick) axes and thumbstick buttons (analog 4-7 and digital 51 and 53)
#define PIN_A_SLX 5
#define PIN_A_SLY 4
#define PIN_SLB 51

#define PIN_A_SRX 7
#define PIN_A_SRY 6
#define PIN_SRB 53





Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
								12, 0,					//btns, hats
								true, true, true, true,	// X (lx), Y (ly), Z (rx), Rx (ry)
								true, true, true, true,	// Ry (slx), Rz (sly), s0 (srx), s1(sry)
								false, false, false		//accel, brake, steering
							);

class AxisCalibration
{
public:
	double max;
	double min;
	double center;
	double deadzone;

	AxisCalibration(double pMin, double pCenter, double pMax, double dz = 0.0)
	{
		max = pMax;
		min = pMin;
		center = pCenter;
		deadzone = dz;
	}

	double processAxis(double axis, double response = 1.0)
	{
		if (abs(axis - center) < deadzone)
			return 0.5;

		if (axis < center)
		{
			axis = map(axis + deadzone, min + deadzone, center, -1.0, 0.0);
		}
		if (axis > center)
		{
			axis = map(axis - deadzone, center, max - deadzone, 0.0, 1.0);
		}

		axis = pow(abs(axis), response) * sign(axis);
		axis = constrain(axis, -1.0, 1.0);


		return axis * 0.5 + 0.5;
	}

	double lerp(double v0, double v1, double t)
	{
		return (1.0 - t) * v0 + t * v1;
	}

	double map(double x, double in_min, double in_max, double out_min, double out_max)
	{
		return (x - in_min) / (in_max - in_min) * (out_max - out_min) + out_min;
	}
};


AxisCalibration axisLX = AxisCalibration(395, 2897, 3760, 5);
AxisCalibration axisLY = AxisCalibration(315, 2258, 4056, 5);
AxisCalibration axisRX = AxisCalibration(0, 2048, 4096, 5);
AxisCalibration axisRY = AxisCalibration(0, 2048, 4096, 5);

AxisCalibration axisSLX = AxisCalibration(790, 2080, 3200, 5);
AxisCalibration axisSLY = AxisCalibration(890, 2050, 3140, 5);
AxisCalibration axisSRX = AxisCalibration(1025, 2020, 3370, 5);
AxisCalibration axisSRY = AxisCalibration(980, 2020, 3220, 5);



void setup() 
{
	pinMode(LED_BUILTIN, OUTPUT);
	
	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);
	pinMode(A3, INPUT_PULLUP);
	pinMode(A4, INPUT_PULLUP);
	pinMode(A5, INPUT_PULLUP);
	pinMode(A6, INPUT_PULLUP);
	pinMode(A7, INPUT_PULLUP);


	pinMode(PIN_SLB, INPUT_PULLUP);
	pinMode(PIN_SRB, INPUT_PULLUP);
	


	analogReadResolution(12);

	
	joystick.setXAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setYAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setZAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setRxAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	
	joystick.setRyAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setRzAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);	
	joystick.setS0AxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setS1AxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	
	joystick.begin(false);

	
	Mouse.begin();
		
	SerialUSB.println((String)"HRV Skycontroller");
}

double slx, sly, srx, sry;


void loop()
{
	
	// print analog pins 0-7 and digital pins 51 and 53	
	//for (int i = 0; i < 8; i++)
	//{
	//	SerialUSB.print((String)"A" + i + ": " + analogRead(i) + " | ");		
	//}
	//SerialUSB.println((String)"D51: " + digitalRead(51) + " | D53: " + digitalRead(53));	
	//SerialUSB.println();


	slx = axisSLX.processAxis(analogRead(PIN_A_SLX));
	sly = axisSLY.processAxis(analogRead(PIN_A_SLY));
	joystick.setRyAxis(slx * JOYSTICK_RANGE_MAX);	
	joystick.setRzAxis(sly * JOYSTICK_RANGE_MAX);
	joystick.setButton(6, !digitalRead(PIN_SLB));
	
	srx = axisSRX.processAxis(analogRead(PIN_A_SRX));
	sry = axisSRY.processAxis(analogRead(PIN_A_SRY));
	joystick.setS0Axis(srx * JOYSTICK_RANGE_MAX);
	joystick.setS1Axis(sry * JOYSTICK_RANGE_MAX);
	joystick.setButton(7, !digitalRead(PIN_SRB));
	



	joystick.sendState();

	delay(16);
}
