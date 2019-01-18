/*
Name:		test.ino
Created:	7/19/2017 2:52:44 PM
Author:	HarvesteR
*/
#include <SoftReset.h>
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

#define STICK_ROTATION_DEG -20

#define JOYSTICK_RANGE_MIN 0
#define JOYSTICK_RANGE_MAX 1023
#define JOYSTICK_TRAVEL 512 // should be half of the range
#define JOYSTICK_CENTER 512


MuxShield muxShield;
Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 52, 0, true, true, true, true, true, true, false, false, false, false, false);


class AxisCalibration
{
	public:
	double max;
	double min;
	double center;

	AxisCalibration(double pMin, double pCenter, double pMax)
	{
		max = pMax;
		min = pMin;
		center = pCenter;
	}
};

const int axis_resolution = 1023;

double xAxis;
double yAxis;
double zAxis;
double thrAxis;
double sl1Axis;
double sl2Axis;

double stickAngle;

// calibration params
AxisCalibration xCal = AxisCalibration(430.0, 606.5, 783.0);
AxisCalibration yCal = AxisCalibration(515.0, 656.5, 798.0);
AxisCalibration zCal = AxisCalibration(192.0, 156.0, 120.0);

double x1, y1, x2, y2;

const double smoothing = 0.5;
double xRot, yRot;
double xOut, yOut, zOut;
double sl1, sl2;

long binaryThrTLast = 0;
bool binaryThrLast = 0;
long toggle1TLast = 0;
bool toggle1Last = 0;
long toggle2TLast = 0;
bool toggle2Last = 0;

long binaryPulseTime = 100;

long centerCalibrateTime = 1000;
long extentsCalibrateTime = 10000;
long centerCalibrateTLast = 0;

int encoderCount = 0;
long encoderTLast = 0;

double rCos, rSin;

bool sendSwitch29Pulses = true;
bool sendSwitch30Pulses = true;



// software reset function 
void softReset() 
{
	joystick.end();
	soft_restart();
}

// the setup function runs once when you press reset or power the board
void setup()
{
	//Serial.begin(256000);

	muxShield.setMode(1, DIGITAL_IN_PULLUP);
	muxShield.setMode(2, DIGITAL_IN_PULLUP);
	muxShield.setMode(3, DIGITAL_IN_PULLUP);


	joystick.setXAxisRange(JOYSTICK_RANGE_MIN,   JOYSTICK_RANGE_MAX);
	joystick.setYAxisRange(JOYSTICK_RANGE_MIN,   JOYSTICK_RANGE_MAX);
	joystick.setZAxisRange(JOYSTICK_RANGE_MIN,   JOYSTICK_RANGE_MAX);
	joystick.setRxAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setRyAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setRzAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
		
	//joystick.setThrottleRange(-16384, 16384);
	//joystick.setRudderRange(-16384, 16384);
	//joystick.setAcceleratorRange(-16384, 16384);
	//joystick.setBrakeRange(-16384, 16384);

	joystick.begin();

	double stickRotation = STICK_ROTATION_DEG * DEG_TO_RAD;
	rCos = cos(stickRotation);
	rSin = sin(stickRotation);

}



void loop()
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

	// buttons 27, 28 are the rotary encoder
	processEncoder(27, 28);

	// button 29 is underside toggle switch
	if (sendSwitch29Pulses)
	{
		if (muxShield.digitalReadMS(BTN29) != toggle1Last)
		{
			toggle1Last = muxShield.digitalReadMS(BTN29);
			toggle1TLast = millis() + binaryPulseTime;
		}
		joystick.setButton(29, toggle1TLast > millis());
	}
	joystick.setButton(40, muxShield.digitalReadMS(BTN29));


	// button 30 is the sidewall toggle
	if (sendSwitch30Pulses)
	{
		if (muxShield.digitalReadMS(BTN30) != toggle2Last)
		{
			toggle2Last = muxShield.digitalReadMS(BTN30);
			toggle2TLast = millis() + binaryPulseTime;
		}
		joystick.setButton(30, toggle2TLast > millis());
	}
	joystick.setButton(41, !muxShield.digitalReadMS(BTN30));

	// button 31 is the side switch with cover
	joystick.setButton(31, !muxShield.digitalReadMS(BTN28));




	// 10 axes, row 3, pins 0 to 9
	// (well, we only have six at the moment, but the idea is to add more)

	xAxis = muxShield.analogReadMS(3, 0);
	yAxis = muxShield.analogReadMS(3, 1);
	zAxis = muxShield.analogReadMS(3, 3);

	thrAxis = muxShield.analogReadMS(3, 7);
	sl1Axis = muxShield.analogReadMS(3, 14);
	sl2Axis = muxShield.analogReadMS(3, 15);


	// center-finding happens for some time after startup or pressing the recalibration combo
	if (millis() - centerCalibrateTLast < centerCalibrateTime)
	{
		xCal.center = lerp(xCal.center, xAxis, 0.2);
		yCal.center = lerp(yCal.center, yAxis, 0.2);
		zCal.center = lerp(zCal.center, zAxis, 0.2);

		xCal.max = xCal.center;
		xCal.min = xCal.center;
		yCal.min = yCal.center;
		yCal.max = yCal.center;
		zCal.min = zCal.center;
		zCal.max = zCal.center;

		delay(17);
		return;
	}
	else if (millis() - centerCalibrateTLast < extentsCalibrateTime)
	{
		// continuous calibration based on max deflection (x52 style) for extents calibration time  (then it stops)
		xCal.max = max(xCal.max, xAxis);
		xCal.min = min(xCal.min, xAxis);
		yCal.max = max(yCal.max, yAxis);
		yCal.min = min(yCal.min, yAxis);
		zCal.min = min(zCal.min, zAxis);
		zCal.max = max(zCal.max, zAxis);
	}


	//Serial.print("X: "); Serial.print(xAxis); Serial.print("      Y: "); Serial.print(yAxis); Serial.print("    Z: "); Serial.print(zAxis); Serial.print("    Thr: "); Serial.print(thrAxis);  Serial.print("    S1: "); Serial.print(sl1Axis); Serial.print("    S2: "); Serial.println(sl2Axis);

	x1 = processAxisAdv(xAxis, 1.0, xCal.min, xCal.center, xCal.max, 0.0);
	y1 = processAxisAdv(yAxis, 1.0, yCal.min, yCal.center, yCal.max, 0.0);
	x2 = processAxisAdv(xAxis, 6.0, xCal.min, xCal.center, xCal.max, 0.0);
	y2 = processAxisAdv(yAxis, 6.0, yCal.min, yCal.center, yCal.max, 0.0);

	xAxis = lerp(x2, x1, min(pow(abs(x1), 1.0), 1.0));
	yAxis = lerp(y2, y1, min(pow(abs(y1), 1.0), 1.0));

	xAxis += x1 * 0.5 * abs(y1);
	yAxis += y1 * 0.5 * abs(x1);

	zAxis = -processAxisAdv(zAxis, 4.0, zCal.min, zCal.center, zCal.max, 0.05);

	thrAxis = processAxis(thrAxis, 1.5, 276.0, 803.0, 0.0);
	sl1Axis = processAxis(sl1Axis, 1, 18.0, 1023.0, 0.0);
	sl2Axis = processAxis(sl2Axis, 1, 18.0, 1023.0, 0.0);

	//Serial.print("X: "); Serial.print(xAxis); Serial.print("      Y: "); Serial.print(yAxis); Serial.print("    Z: "); Serial.print(zAxis); Serial.print("    Thr: "); Serial.print(thrAxis);  Serial.print("    S1: "); Serial.print(sl1Axis); Serial.print("    S2: "); Serial.println(sl2Axis);

	// apply rotation to XY axes (because the sidepanel is mounted to the side and it's more natural to push it at an angle)
	xRot = xAxis * rCos + yAxis * rSin;
	yRot = -xAxis * rSin + yAxis * rCos;


	xOut = lerp(xOut, xRot, smoothing);
	yOut = lerp(yOut, yRot, smoothing);
	zOut = lerp(zOut, zAxis, smoothing);

	joystick.setXAxis(xOut *      JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setYAxis(yOut *      JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setZAxis(zOut *      JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setRzAxis(thrAxis * JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	sl1 = lerp(sl1, sl1Axis *       JOYSTICK_TRAVEL + JOYSTICK_CENTER, smoothing);
	sl2 = lerp(sl2, sl2Axis *       JOYSTICK_TRAVEL + JOYSTICK_CENTER, smoothing);

	joystick.setRxAxis(sl1);
	joystick.setRyAxis(sl2);


	processAxisButtons(thrAxis, &binaryThrLast, &binaryThrTLast, 42, 43, 44, -0.9, 0.9);
	//processAxisButtons(sl1Axis, 0, 0, -1, 45, 46, -0.66, 0.66);
	//processAxisButtons(sl2Axis, 0, 0, -1, 47, 48, -0.66, 0.66);
	processAxisButtons(zAxis, 0, 0, -1, 49, 50, -0.8, 0.8);



	// deflecting the main stick also controls the hat switch (allows mapping the stick to binary bindings in games)
	// hat takes up buttons 32 through 39
	if (abs(xOut) > 0.5 || abs(yOut) > 0.5)
	{
		stickAngle = atan2(yOut, xOut) * RAD_TO_DEG;
		stickAngle += 112.5;

		//joystick.setHatSwitch(1, stickAngle);
		setHat(stickAngle, 32);
	}
	else
	{
		//joystick.setHatSwitch(1, -1);
		setHat(-1, 32);
	}

	if (PollCalibrationCombo())
	{
		if (!muxShield.digitalReadMS(BTN25)) // btn 25 is the red button on top of the stick
			softReset();

		if (!muxShield.digitalReadMS(BTN27))
			centerCalibrateTLast = millis();

		if (!muxShield.digitalReadMS(BTN28))
		{
			sendSwitch30Pulses = !muxShield.digitalReadMS(BTN30);
			sendSwitch29Pulses = !muxShield.digitalReadMS(BTN29);
		}
	}

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

	// awkward and ugly debouncing here, but I don't feel like rewiring the encoder to the interrupt lines

	// the current decouncing scheme works by acummulating encoder reads before emitting actual input. 
	// doing this averages out the encoder signal to eliminate false readings (inputting left when rotated right, and so on)
	// somewhat analogous to a 'software capacitor'.

	// This scheme does not do much to even out the frequency of inputs as the encoder turns though.
	// another issue is that turning the encoder very quickly results in lots of bad reads and no input sent. Turn encoder gently.

	if (rotary != 0 && abs(encoderCount) <= 1)
	{
		//delay(2);
		encoderCount += rotary;

		if (abs(encoderCount) > 1)
		{
			if (encoderCount > 0)
			{
				joystick.pressButton(rIdxA);
			}
			if (encoderCount < 0)
			{
				joystick.pressButton(rIdxB);
			}

			encoderTLast = millis() + binaryPulseTime;
		}
	}

	if (encoderTLast > 0 && encoderTLast < millis())
	{
		joystick.releaseButton(rIdxB);
		joystick.releaseButton(rIdxA);

		encoderTLast = 0;
		encoderCount = 0;
	}
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
	return !muxShield.digitalReadMS(BTN4) && !muxShield.digitalReadMS(BTN5) && !muxShield.digitalReadMS(BTN9) && !muxShield.digitalReadMS(BTN10);
}


void processAxisButtons(double axis, bool *axisLast, long *axisTLast, int btnFlick, int btnMin, int btnMax, double minThreshold, double maxThreshold)
{
	if (btnFlick != -1)
	{
		// pulse 'flick' button whenever the axis moves through the center
		if (axis > 0 && !*axisLast)
		{
			*axisLast = true;
			*axisTLast = millis() + binaryPulseTime;
		}
		if (axis < 0 && *axisLast)
		{
			*axisLast = false;
			*axisTLast = millis() + binaryPulseTime;
		}

		joystick.setButton(btnFlick, *axisTLast > millis());
	}

	// buttons 35 and 36 are on when the axis is at min and max
	if (btnMin != -1)
		joystick.setButton(btnMin, axis < minThreshold);
	if (btnMax != -1)
		joystick.setButton(btnMax, axis > maxThreshold);

}

int hatAngle;
void setHat(int angle, int firstButton)
{
	// the 'hat' switch is actually a group of buttons, because some games apparently read hats as other axes
	if (angle != -1)
	{
		if (angle < 360.0) angle += 360;
		if (angle > 360.0) angle -= 360;
		hatAngle = angle / 45;
	}
	else
	{
		hatAngle = -1;
	}

	joystick.setButton(firstButton, hatAngle == 0);
	joystick.setButton(firstButton + 1, hatAngle == 1);
	joystick.setButton(firstButton + 2, hatAngle == 2);
	joystick.setButton(firstButton + 3, hatAngle == 3);
	joystick.setButton(firstButton + 4, hatAngle == 4);
	joystick.setButton(firstButton + 5, hatAngle == 5);
	joystick.setButton(firstButton + 6, hatAngle == 6);
	joystick.setButton(firstButton + 7, hatAngle == 7);
}