
#include "Joystick.h"


Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 52, 0, true, true, true, false, false, false , false, true, false, false, false);



#define sign(x) ((x) > 0 ? 1: ((x) < 0 ? -1 : 0))

//pin mappings
#define BTN4 10
#define BTN5 11
#define AXIS_COLL 2
#define AXIS_THR 3

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
// axis inputsAxisCalibration axisColl = AxisCalibration(0, 2048, 4096);
AxisCalibration axisTwist = AxisCalibration(0, 2048, 4096);
AxisCalibration axisX = AxisCalibration(0, 2048, 4096);
AxisCalibration axisY = AxisCalibration(0, 2048, 4096);//axis output #define JOYSTICK_RANGE_MIN 0
#define JOYSTICK_RANGE_MAX 1023
#define JOYSTICK_TRAVEL 512 // should be half of the range
#define JOYSTICK_CENTER 512double zOut, tOut;
void setup(){	//Serial.begin(115200);	pinMode(BTN4, INPUT_PULLUP);
	pinMode(BTN5, INPUT_PULLUP);	analogReadResolution(12);

	joystick.begin();
	joystick.setXAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setYAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setZAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setThrottleRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	}void loop(){
	zOut = processAxisAdv(analogRead(AXIS_COLL), 1.0, axisColl.min, axisColl.center, axisColl.max, 0.0);
	tOut = processAxisAdv(analogRead(AXIS_THR), 1.0, axisTwist.min, axisTwist.center, axisTwist.max, 0.0);
	
	joystick.setZAxis(zOut *  JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setThrottle(tOut * JOYSTICK_TRAVEL + JOYSTICK_CENTER);	joystick.setButton(4, digitalRead(BTN4) == LOW);
	joystick.setButton(5, digitalRead(BTN5) == LOW);		delay(20);}double lerp(double v0, double v1, double t){	return (1.0 - t) * v0 + t * v1;}double map(double x, double in_min, double in_max, double out_min, double out_max){	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;}
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
