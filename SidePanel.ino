/*
Name:		test.ino
Created:	7/19/2017 2:52:44 PM
Author:	HarvesteR
*/
#include "MuxShield.h"
#include "Joystick.h"

#define IO1 1
#define IO2 2
#define IO3 3


#define BTN1 1,3
#define BTN2 1,7
#define BTN3 1,10
#define BTN4 1,11
#define BTN5 1,13
#define BTN6 1,2
#define BTN7 1,6
#define BTN8 1,9
#define BTN9 1,12
#define BTN10 1,14


#define BTN11 1,1
#define BTN12 1,0
#define BTN13 1,5
#define BTN14 1,4
#define BTN15 2,6
#define BTN16 1,8
#define BTN17 1,15
#define BTN18 2,0 
#define BTN19 2,2
#define BTN20 2,1

#define BTN21 2,12
#define BTN22 2,3
#define BTN23 2,4
#define BTN24 2,5
#define BTN25 2,10
#define BTN26 2,11
#define BTN27 2,7
#define BTN28 2,13
#define BTN29 2,14
#define BTN30 2,15

#define ROTARYA 2,8
#define ROTARYB 2,9

#define sign(x) ((x) > 0 ? 1: ((x) < 0 ? -1 : 0))

MuxShield muxShield;
Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 32, 2, true, true, true, false, false, false, false, true, false, false, false);

const int axis_resolution = 1023;

double xAxis;
double yAxis;
double zAxis;
double thrAxis;

// calibration params
double xMin = 430.0;
double xCtr = 606.5;
double xMax = 783.0;
double yMin = 515.0;
double yCtr = 656.5;
double yMax = 798.0;


double x1, y1, x2, y2;

const double smoothing = 0.5;
double xOut, yOut, zOut;



enum Mode
{
	NRM,
	Calibrate_DefineCenter,
	Calibrate_FindExtents
};
Mode mode;


// the setup function runs once when you press reset or power the board
void setup()
{
	//Serial.begin(256000);

	muxShield.setMode(1, DIGITAL_IN_PULLUP);
	muxShield.setMode(2, DIGITAL_IN_PULLUP);
	muxShield.setMode(3, DIGITAL_IN_PULLUP);

	joystick.begin();

	joystick.setXAxisRange(-16384, 16384);
	joystick.setYAxisRange(-16384, 16384);
	joystick.setZAxisRange(-16384, 16384);
	//joystick.setRxAxisRange(0, axis_resolution);
	//joystick.setRyAxisRange(0, axis_resolution);
	//joystick.setRzAxisRange(0, axis_resolution);
	joystick.setThrottleRange(-16384, 16384);
	//joystick.setRudderRange(0, axis_resolution);
	//joystick.setAcceleratorRange(0, axis_resolution);
	//joystick.setBrakeRange(0, axis_resolution);

	mode = NRM;

}



void loop()
{

	if (mode == NRM)
	{
		// 32 buttons, rows 1 and 2
		// first 30 buttons
		joystick.setButton(0, !muxShield.digitalReadMS(BTN1));
		joystick.setButton(1, !muxShield.digitalReadMS(BTN2));
		joystick.setButton(2, !muxShield.digitalReadMS(BTN3));
		joystick.setButton(3, !muxShield.digitalReadMS(BTN4));
		joystick.setButton(4, !muxShield.digitalReadMS(BTN5));
		joystick.setButton(5, !muxShield.digitalReadMS(BTN6));
		joystick.setButton(6, !muxShield.digitalReadMS(BTN7));
		joystick.setButton(7, !muxShield.digitalReadMS(BTN8));
		joystick.setButton(8, !muxShield.digitalReadMS(BTN9));
		joystick.setButton(9, !muxShield.digitalReadMS(BTN10));


		joystick.setButton(10, !muxShield.digitalReadMS(BTN11));
		joystick.setButton(11, !muxShield.digitalReadMS(BTN12));
		joystick.setButton(12, !muxShield.digitalReadMS(BTN13));
		joystick.setButton(13, !muxShield.digitalReadMS(BTN14));
		joystick.setButton(14, !muxShield.digitalReadMS(BTN15));
		joystick.setButton(15, !muxShield.digitalReadMS(BTN16));
		joystick.setButton(16, !muxShield.digitalReadMS(BTN17));
		joystick.setButton(17, !muxShield.digitalReadMS(BTN18));
		joystick.setButton(18, !muxShield.digitalReadMS(BTN19));
		joystick.setButton(19, !muxShield.digitalReadMS(BTN20));


		joystick.setButton(20, !muxShield.digitalReadMS(BTN21));
		joystick.setButton(21, !muxShield.digitalReadMS(BTN22));
		joystick.setButton(22, !muxShield.digitalReadMS(BTN23));
		joystick.setButton(23, !muxShield.digitalReadMS(BTN24));
		joystick.setButton(24, !muxShield.digitalReadMS(BTN25));
		joystick.setButton(25, !muxShield.digitalReadMS(BTN26));
		joystick.setButton(26, !muxShield.digitalReadMS(BTN27));

		//two buttons are actually a rotary encoder
		processEncoder(27, 28);

		//joystick.setButton(19, !muxShield.digitalReadMS(BTN28));
		//joystick.setButton(30, !muxShield.digitalReadMS(BTN29));
		//joystick.setButton(31, !muxShield.digitalReadMS(BTN30));


			// 10 axes, row 3, pins 0 to 9
			// (well, we only have four at the moment, but the idea is to add more)

		xAxis = muxShield.analogReadMS(3, 0);
		yAxis = muxShield.analogReadMS(3, 1);
		zAxis = muxShield.analogReadMS(3, 3);
		thrAxis = muxShield.analogReadMS(3, 7);


		//Serial.print("X: "); Serial.print(xAxis); Serial.print("      Y: "); Serial.print(yAxis); Serial.print("    Z: "); Serial.print(zAxis); Serial.print("    Thr: "); Serial.println(thrAxis);

		x1 = processAxisAdv(xAxis, 1.0, xMin, xCtr, xMax, 0.0);
		y1 = processAxisAdv(yAxis, 1.0, yMin, yCtr, yMax, 0.0);
		x2 = processAxisAdv(xAxis, 6.0, xMin, xCtr, xMax, 0.0);
		y2 = processAxisAdv(yAxis, 6.0, yMin, yCtr, yMax, 0.0);

		xAxis = lerp(x2, x1, min(pow(abs(x1), 1.0), 1.0));
		yAxis = lerp(y2, y1, min(pow(abs(y1), 1.0), 1.0));

		xAxis += x1 * 0.5 * abs(y1);
		yAxis += y1 * 0.5 * abs(x1);

		zAxis = processAxis(zAxis, 4.0, 192.0, 120.0, 0.0);
		thrAxis = processAxis(thrAxis, 1.5, 276.0, 803.0, 0.0);

		//Serial.print("X: "); Serial.print(xAxis); Serial.print("      Y: "); Serial.print(yAxis); Serial.print("    Z: "); Serial.print(zAxis); Serial.print("    Thr: "); Serial.println(thrAxis);

		xOut = lerp(xOut, xAxis, smoothing);
		yOut = lerp(yOut, yAxis, smoothing);
		zOut = lerp(zOut, zAxis, smoothing);

		joystick.setXAxis(xOut * 16384.0);
		joystick.setYAxis(yOut * 16384.0);
		joystick.setZAxis(zOut * 16384.0);
		joystick.setThrottle(thrAxis * 16384.0);


		if (PollCalibrationCombo())
		{
			mode = Calibrate_DefineCenter;
			Serial.println("Calibrating... Leave Stick Centered and Press Button 1");

		}
	}
	else if (mode == Calibrate_DefineCenter)
	{
		joystick.setButton(0, ((millis() % 1000) < 500 ? 1 : 0));

		xCtr = lerp(xCtr, muxShield.analogReadMS(3, 0), 0.2);
		yCtr = lerp(yCtr, muxShield.analogReadMS(3, 1), 0.2);

		Serial.print("X: "); Serial.print(xCtr); Serial.print("      Y: "); Serial.println(yCtr);

		if (!muxShield.digitalReadMS(BTN1))
		{
			mode = Calibrate_FindExtents;
			
			joystick.setButton(0, 0);
			Serial.println("Calibrating... Move Stick to find XY extents and Press Button 2");

			xMax = xCtr;
			xMin = xCtr;
			yMax = yCtr;
			yMin = yCtr;

		}

	}
	else if (mode == Calibrate_FindExtents)
	{
		joystick.setButton(1, ((millis() % 1000) < 500 ? 1 : 0));


		xMax = max((double)xMax, muxShield.analogReadMS(3, 0));
		xMin = min((double)xMin, muxShield.analogReadMS(3, 0));

		yMax = max((double)yMax, muxShield.analogReadMS(3, 1));
		yMin = min((double)yMin, muxShield.analogReadMS(3, 1));


		Serial.print("X Min: "); Serial.print(xMin); Serial.print("    X Max: "); Serial.print(xMax); Serial.print("      Y Min: "); Serial.print(yMin); Serial.print("      Y Max: "); Serial.println(yMax);

		if (!muxShield.digitalReadMS(BTN2))
		{
			mode = NRM;
			Serial.println("Done!");
			joystick.setButton(1, 0);
		}
	}

	//	Serial.flush();

	delay(17);
}


int rotaryIdx(int x, int y)
{
	return (x - 1) * 16 + y;
}

int8_t read_encoder(int A, int B)
{
	static int8_t enc_states[] = { 0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0 };
	static uint8_t old_AB = 0;
	/**/
	old_AB <<= 2;                   //remember previous state
	old_AB |= ((A << 1 | B) & 0x03);  //add current state
	return (enc_states[(old_AB & 0x0f)]);
}


void processEncoder(int rIdxA, int rIdxB)
{
	int8_t rotary = read_encoder(muxShield.digitalReadMS(ROTARYA), muxShield.digitalReadMS(ROTARYB));
	if (rotary > 0)
	{
		joystick.pressButton(rIdxA);
	}
	else
		joystick.releaseButton(rIdxA);


	if (rotary < 0)
		joystick.pressButton(rIdxB);
	else
		joystick.releaseButton(rIdxB);
}





double processAxis(int raw, double response, double inMin, double inMax, double deadzone)
{
	double axis = map((double)raw, inMin, inMax, -1.0, 1.0);
	axis = pow(abs(axis), response) * sign(axis);
	axis = constrain(axis, -1.0, 1.0);

	if (abs(axis) < deadzone)
		axis = 0.0;

	return axis;
}

double processAxisAdv(int raw, double response, double inMin, double inCtr, double inMax, double deadzone)
{
	double axis = (double)raw;
	if (axis == inCtr)
		return 0.0;

	if (axis < inCtr)
	{
		axis = map(axis, inMin, inCtr, -1.0, 0.0);
	}
	if (axis > inCtr)
	{
		axis = map(axis, inCtr, inMax, 0.0, 1.0);
	}

	axis = pow(abs(axis), response) * sign(axis);
	axis = constrain(axis, -1.0, 1.0);

	if (abs(axis) < deadzone)
		axis = 0.0;

	return axis;
}


double map(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double lerp(double v0, double v1, double t)
{
	return (1.0 - t) * v0 + t * v1;
}


bool PollCalibrationCombo()
{
	if (!muxShield.digitalReadMS(BTN4) &&
		!muxShield.digitalReadMS(BTN5) &&
		!muxShield.digitalReadMS(BTN9) &&
		!muxShield.digitalReadMS(BTN10) &&
		!muxShield.digitalReadMS(BTN27))
	{
		joystick.setButton(3,0);
		joystick.setButton(4,0);
		joystick.setButton(8,0);
		joystick.setButton(9,0);
		joystick.setButton(26, 0);

		return true;
	}
	else 
		return false;
}

