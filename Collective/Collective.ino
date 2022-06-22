
#include <HID.h>
#include <Mouse.h>
#include "Joystick.h"

#define sign(x) ((x) > 0 ? 1: ((x) < 0 ? -1 : 0))

//High impedance pin mode
#define OUTPUT_OFF INPUT_PULLUP


// head matrix mapping
// see https://www.tinkercad.com/things/fCHrucmTHlC-collective-head-wiring for the wiring
#define MxA 40
#define MxB 44
#define MxC 48
#define MxD 52
#define MxE 50

#define Mx0 34
#define Mx1 38
#define Mx2 42
#define Mx3 46


// pin mappings
#define BTN4 10
#define BTN5 11
#define BTN_TBC 36 // new thumbstick button on unused pin 36 (prob left for exactly this)
#define AXIS_X 1
#define AXIS_Y 0
#define AXIS_COLL 2
#define AXIS_THR 3

// head rotary pins (RTR0)
#define RTR0_A 32
#define RTR0_B 28
#define RTR0_Ck 30


// aft panel left rotary pins (RTR1)
#define RTR1_A 33
#define RTR1_B 31
#define RTR1_Ck 29

// aft panel right rotary pins (RTR2)
#define RTR2_A 23
#define RTR2_B 25
#define RTR2_Ck 27

// aft panel switch
#define AFT_SW0 35
#define AFT_SW1 37

// mid panel rotaries (RTR3, RTR4)

#define RTR4_A 43 //w 
#define RTR3_Ck 45 // yello
#define RTR3_B 47 // blu
#define RTR4_Ck 49 //green
#define RTR4_B 51 // red
#define RTR3_A 53 // w


// fore panel L/R toggle and rotaries (RTR5, RTR6)
#define FORE_TGL0_L 39 
#define FORE_TGL0_R 41
#define RTR5_A 14 
#define RTR6_A 15
#define RTR5_B 16 
#define RTR6_B 17 
#define RTR5_Ck 18
#define RTR6_Ck 19






// axis output 
#define JOYSTICK_RANGE_MIN 0
#define JOYSTICK_RANGE_MAX 1023
#define JOYSTICK_TRAVEL 512 // should be half of the range
#define JOYSTICK_CENTER 512
#define POV_DEADZONE 0.5 // raw units


// button output mapping
#define Trg 0
#define THBS 1
#define SLR 2
#define HS 3
#define GpR 4
#define GpK 5
#define TBC 51 // new thumb click on btn 51

#define RG 6
#define DG 7
#define TSL_up 8
#define TSL_Dn 9
#define TSR_Up 10
#define TSR_Dn 11
#define RTH_lf 13
#define RTH_rt 12
#define IDF 14
#define IDB 15

#define NavUp 16
#define NavRight 17
#define NavDown 18
#define NavLeft 19
#define NavCtr 20


#define Rtr0_back 25
#define Rtr0_fwd 26
#define Rtr0_Click 27

// dev switch output as buttons
#define dev_crs_btn 127
#define dev_scrwh_btn 126
#define dev_povtbs_btn 125

// Fore panel -----------------

#define ForePanel_Rtr1_back 45
#define ForePanel_Rtr1_fwd 46
#define ForePanel_Rtr1_click 47
#define ForePanel_Rtr2_back 48
#define ForePanel_Rtr2_fwd 49
#define ForePanel_Rtr2_click 50

#define ForePanel_tgl0_left 36
#define ForePanel_tgl0_rgt 37
#define ForePanel_tgl0_ret 38

int t_ForePanel_Tgl0;

// Middle panel -------------

#define MidPanel_Rtr1_back 39
#define MidPanel_Rtr1_fwd 40
#define MidPanel_Rtr1_click 41
#define MidPanel_Rtr2_back 42
#define MidPanel_Rtr2_fwd 43
#define MidPanel_Rtr2_click 44


// Aft panel -----------------

#define AftPanel_Rtr1_back 28
#define AftPanel_Rtr1_fwd 29
#define AftPanel_Rtr1_click 30

#define AftPanel_Rtr2_back 31
#define AftPanel_Rtr2_fwd 32
#define AftPanel_Rtr2_click 33

#define AftPanel_SW_up 34
#define AftPanel_SW_dn 35


// Dev Panel --------------------

#define DEV_CRS 6
#define DEV_SCRWH 7
#define DEV_POVTBS 5
#define DEV_ANALOG_XL 7
#define DEV_ANALOG_XR 6

//-------------------------------

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
};


// axis inputs
AxisCalibration axisColl = AxisCalibration(11, 2048, 4095);
AxisCalibration axisTwist = AxisCalibration(11, 2048, 4095);
AxisCalibration axisX = AxisCalibration(15, 2954, 4095, 20);
AxisCalibration axisY = AxisCalibration(17, 2881, 4095, 20);

AxisCalibration axisRX = AxisCalibration(11, 2048, 4095, 50);
AxisCalibration axisRY = AxisCalibration(11, 2048, 4095, 50);

int headA;
int headB;
int headC;
int headD;
int headE;

double xOut, yOut, zOut, tOut, rxOut, ryOut;
double stickAngle;

bool suppressThumbstickButtons;
bool useThbstickHat;
bool useThbCursor;
bool useScrollWheel;

Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 128, 2, true, true, true, true, true, false, false, true, false, false, false);


const uint32_t rotaryPulseTime = 40;

class RotaryEncoder
{
public:
	volatile uint32_t Tback;
	volatile uint32_t Tfwd;
	volatile int a = 0;
	volatile int b = 0;

	int32_t rLast = 0;
};

class Rotary
{
public:
	volatile int bLast;

	volatile uint32_t Tlock = -1;

	volatile int count = 0;
	
	volatile uint32_t Tback;
	volatile uint32_t Tfwd;
};

volatile Rotary rtr0; 
volatile Rotary rtr1; 
volatile Rotary rtr2;
volatile RotaryEncoder rot3;
volatile RotaryEncoder rot4;
volatile RotaryEncoder rot5;
volatile RotaryEncoder rot6;


bool tgl0L, tgl0R;


const int centerFindingPeriodDuration = 2000;

void setup()
{
	// matrix pins
	pinMode(Mx0, INPUT);
	pinMode(Mx1, INPUT);
	pinMode(Mx2, INPUT);
	pinMode(Mx3, INPUT);
	
	pinMode(MxA, OUTPUT_OFF);
	pinMode(MxB, OUTPUT_OFF);
	pinMode(MxC, OUTPUT_OFF);
	pinMode(MxD, OUTPUT_OFF);
	pinMode(MxE, OUTPUT_OFF);


	//grip btns and tbc
	pinMode(BTN4, INPUT_PULLUP);
	pinMode(BTN5, INPUT_PULLUP);
	pinMode(BTN_TBC, INPUT_PULLUP);

	// rotary inputs 
	pinMode(RTR0_A, INPUT_PULLUP);
	pinMode(RTR0_B, INPUT_PULLUP);
	pinMode(RTR0_Ck, INPUT_PULLUP);

	// aft panel inputs 
	pinMode(RTR1_A, INPUT_PULLUP);
	pinMode(RTR1_B, INPUT_PULLUP);
	pinMode(RTR1_Ck, INPUT_PULLUP);

	pinMode(RTR2_A, INPUT_PULLUP);
	pinMode(RTR2_B, INPUT_PULLUP);
	pinMode(RTR2_Ck, INPUT_PULLUP);

	pinMode(AFT_SW0, INPUT_PULLUP);
	pinMode(AFT_SW1, INPUT_PULLUP);

	// mid panel inputs 
	pinMode(RTR3_A, INPUT_PULLUP);
	pinMode(RTR3_B, INPUT_PULLUP);
	pinMode(RTR3_Ck, INPUT_PULLUP);
	pinMode(RTR4_A, INPUT_PULLUP);
	pinMode(RTR4_B, INPUT_PULLUP);
	pinMode(RTR4_Ck, INPUT_PULLUP);

	// fore panel inputs 
	pinMode(FORE_TGL0_L, INPUT_PULLUP);
	pinMode(FORE_TGL0_R, INPUT_PULLUP);	
	pinMode(RTR5_A, INPUT_PULLUP);
	pinMode(RTR5_B, INPUT_PULLUP);
	pinMode(RTR5_Ck, INPUT_PULLUP);
	pinMode(RTR6_A, INPUT_PULLUP);
	pinMode(RTR6_B, INPUT_PULLUP);
	pinMode(RTR6_Ck, INPUT_PULLUP);

	// dev panel inputs
	pinMode(DEV_CRS, INPUT_PULLUP);
	pinMode(DEV_SCRWH, INPUT_PULLUP);
	pinMode(DEV_POVTBS, INPUT_PULLUP);


	rtr0.Tback = 0;
	rtr0.Tfwd = 0;
	attachInterrupt(digitalPinToInterrupt(RTR0_A), interrupt_ROT0_re, RISING);
	attachInterrupt(digitalPinToInterrupt(RTR0_A), interrupt_ROT0_fe, FALLING);

	rtr1.Tback = 0;
	rtr1.Tfwd = 0;
	attachInterrupt(digitalPinToInterrupt(RTR1_A), interrupt_ROT1_re, RISING);	
	attachInterrupt(digitalPinToInterrupt(RTR1_A), interrupt_ROT1_fe, FALLING);

	rtr2.Tback = 0;
	rtr2.Tfwd = 0;
	attachInterrupt(digitalPinToInterrupt(RTR2_A), interrupt_ROT2_re, RISING);
	attachInterrupt(digitalPinToInterrupt(RTR2_A), interrupt_ROT2_fe, FALLING);
		

	attachInterrupt(digitalPinToInterrupt(RTR3_A), interrupt_ROT3, CHANGE);
	attachInterrupt(digitalPinToInterrupt(RTR4_A), interrupt_ROT4, CHANGE);
	attachInterrupt(digitalPinToInterrupt(RTR5_A), interrupt_ROT5, CHANGE);
	attachInterrupt(digitalPinToInterrupt(RTR6_A), interrupt_ROT6, CHANGE);


	analogReadResolution(12);

	joystick.begin();
	joystick.setXAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setYAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setZAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setThrottleRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setRxAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setRyAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	
	Mouse.begin();


	//SerialUSB.begin(115200);
}

uint32_t mils;
uint32_t tLastCrsRecenter = 0;

int32_t r;

void loop()
{
	mils = millis();



	// dev panel switches before anything else, as they control device options
	// pins 5, 6, 7
	//SerialUSB.println((String)"PIN 5: " + digitalRead(5) + " | PIN 6: " + digitalRead(6) + " | PIN 7: " + digitalRead(7));
	//SerialUSB.println((String)"PIN A6: " + analogRead(6) + " | PIN A7: " + analogRead(7));
	bool crsToggle = !digitalRead(DEV_CRS);
	if (crsToggle != useThbCursor)
	{
		useThbCursor = crsToggle;
		
		if (crsToggle)
			tLastCrsRecenter = mils;
	}
	joystick.setButton(dev_crs_btn, crsToggle);

	useScrollWheel = !digitalRead(DEV_SCRWH);
	joystick.setButton(dev_scrwh_btn, useScrollWheel);
	
	useThbstickHat = digitalRead(DEV_POVTBS);
	joystick.setButton(dev_povtbs_btn, useThbstickHat);

	rxOut = processAxisAdv(analogRead(DEV_ANALOG_XL), 1.0, axisRX.min, axisRX.center, axisRX.max, axisRX.deadzone);
	joystick.setRxAxis(rxOut * JOYSTICK_TRAVEL + JOYSTICK_CENTER);

	ryOut = processAxisAdv(analogRead(DEV_ANALOG_XR), 1.0, axisRY.min, axisRY.center, axisRY.max, axisRY.deadzone);
	joystick.setRyAxis(ryOut * JOYSTICK_TRAVEL + JOYSTICK_CENTER);




	// head controls

	// group A (Nav A, Thbs, SLR, HS)
	readHeadMatrixGroup(MxA, &headA);
	joystick.setButton(NavCtr, headA & 0x1);//ctr
	joystick.setButton(THBS, headA & 0x2);
	joystick.setButton(SLR, headA & 0x4);
	joystick.setButton(HS, headA & 0x8);
	
	
	// group B (Nav B, RG, DG, TSR+)
	readHeadMatrixGroup(MxB, &headB);
	joystick.setButton(NavRight, headB & 0x1);//rgt
	joystick.setButton(RG, headB & 0x2);
	joystick.setButton(DG, headB & 0x4);
	joystick.setButton(TSR_Up, headB & 0x8);
	
	
	// group C (Nav C, TSL+, TSL-, TSR-)
	readHeadMatrixGroup(MxC, &headC);
	joystick.setButton(NavUp, headC & 0x1);//up
	joystick.setButton(TSL_up, headC & 0x2);
	joystick.setButton(TSL_Dn, headC & 0x4);
	joystick.setButton(TSR_Dn, headC & 0x8);
	
	// group D (Nav D, RTH-L, RTH-R, unused)
	readHeadMatrixGroup(MxD, &headD);
	joystick.setButton(NavDown, headD & 0x1);//dn
	joystick.setButton(RTH_lf, headD & 0x2);
	joystick.setButton(RTH_rt, headD & 0x4);
	
	// group E (Nav E, Trg, IDF, IDB)
	readHeadMatrixGroup(MxE, &headE);
	joystick.setButton(NavLeft, headE & 0x1);//lft
	joystick.setButton(Trg, headE & 0x2);
	joystick.setButton(IDF, headE & 0x4);
	joystick.setButton(IDB, headE & 0x8);

	
	
	// grip controls
	zOut = processAxisAdv(analogRead(AXIS_COLL), 1.0, axisColl.min, axisColl.center, axisColl.max, axisColl.deadzone);
	tOut = processAxisAdv(analogRead(AXIS_THR), 1.0, axisTwist.min, axisTwist.center, axisTwist.max, axisTwist.deadzone);

	joystick.setZAxis(zOut * JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setThrottle(tOut * JOYSTICK_TRAVEL + JOYSTICK_CENTER);

	joystick.setButton(GpR, digitalRead(BTN4) == LOW);
	joystick.setButton(GpK, digitalRead(BTN5) == LOW);
	joystick.setButton(TBC, digitalRead(BTN_TBC) == LOW);
	
	// rotary 0 (head)
	joystick.setButton(Rtr0_Click, digitalRead(RTR0_Ck) == LOW);

	joystick.setButton(Rtr0_back, rtr0.Tback + rotaryPulseTime > mils);
	joystick.setButton(Rtr0_fwd, rtr0.Tfwd + rotaryPulseTime > mils);
	   

	// the nav switch is also POV2
	setNavHat((headC & 0x1), (headB & 0x1), (headD & 0x1), (headE & 0x1));
	   

	//thumbstick axes (and mouse cursor)	
	if (mils < tLastCrsRecenter + centerFindingPeriodDuration)
	{
		axisX.center = lerp(axisX.center, analogRead(AXIS_X), 0.5);
		axisY.center = lerp(axisY.center, analogRead(AXIS_Y), 0.5);
	}
	else
	{
		xOut = lerp(xOut, processAxisAdv(analogRead(AXIS_X), 1.0, axisX.min, axisX.center, axisX.max, axisX.deadzone), 0.5);
		yOut = lerp(yOut, processAxisAdv(analogRead(AXIS_Y), 1.0, axisY.min, axisY.center, axisY.max, axisY.deadzone), 0.5);

		if (useThbCursor)
		{
			Mouse.move(xOut * 25, yOut * 25, 0);
		}
		
		joystick.setXAxis(xOut * JOYSTICK_TRAVEL + JOYSTICK_CENTER);
		joystick.setYAxis(yOut * JOYSTICK_TRAVEL + JOYSTICK_CENTER);
				
		
		// if enabled, deflecting the thumbstick also controls the hat switch (allows mapping the stick to binary bindings in games)
		if (useThbstickHat && (abs(xOut) > POV_DEADZONE || abs(yOut) > POV_DEADZONE))
		{
			stickAngle = atan2(yOut, xOut) * RAD_TO_DEG;
			stickAngle += 112.5;

			joystick.setHatSwitch(0, stickAngle);
			setHat(stickAngle, 21);
		}
		else
		{
			joystick.setHatSwitch(0, -1);
			setHat(-1, 21);
		}
	}


	// uncomment for debugging out axis values. remember to uncomment SerialUSB.Begin in setup(). SerialUSB (instead of Serial) outputs on the native port :)
	//SerialUSB.println((String)"X: " + axisX.center + " V Raw: " + analogRead(AXIS_X) + " V Out: " + xOut);
	//SerialUSB.println((String)"Y: " + axisY.center + " V Raw: " + analogRead(AXIS_Y) + " V Out: " + yOut);


	// rotary 1 (aftPanel left)
	joystick.setButton(AftPanel_Rtr1_click, digitalRead(RTR1_Ck) == LOW);
	
	joystick.setButton(AftPanel_Rtr1_back, rtr1.Tback + rotaryPulseTime > mils);
	joystick.setButton(AftPanel_Rtr1_fwd, rtr1.Tfwd + rotaryPulseTime > mils);


	// rotary 2 (aftPanel right)
	joystick.setButton(AftPanel_Rtr2_click, digitalRead(RTR2_Ck) == LOW);

	joystick.setButton(AftPanel_Rtr2_back, rtr2.Tback + rotaryPulseTime > mils);
	joystick.setButton(AftPanel_Rtr2_fwd, rtr2.Tfwd + rotaryPulseTime > mils);


	// aft panel switch
	joystick.setButton(AftPanel_SW_up, digitalRead(AFT_SW0) == LOW);
	joystick.setButton(AftPanel_SW_dn, digitalRead(AFT_SW1) == LOW);

	// mid panel rotaries
	joystick.setButton(MidPanel_Rtr1_click, digitalRead(RTR3_Ck) == LOW);
	joystick.setButton(MidPanel_Rtr1_back, rot3.Tback + rotaryPulseTime > mils);
	joystick.setButton(MidPanel_Rtr1_fwd, rot3.Tfwd + rotaryPulseTime > mils);
	joystick.setButton(MidPanel_Rtr2_click, digitalRead(RTR4_Ck) == LOW);
	joystick.setButton(MidPanel_Rtr2_back, rot4.Tback + rotaryPulseTime > mils);
	joystick.setButton(MidPanel_Rtr2_fwd, rot4.Tfwd + rotaryPulseTime > mils);


	// fore panel switch
	tgl0L = digitalRead(FORE_TGL0_L) == LOW;
	tgl0R = digitalRead(FORE_TGL0_R) == LOW;
	joystick.setButton(ForePanel_tgl0_left, tgl0L);
	joystick.setButton(ForePanel_tgl0_rgt, tgl0R);

	// fore panel switch return-to-center pulse 
	if (tgl0L || tgl0R)
	{
		joystick.setButton(ForePanel_tgl0_ret, false);
		t_ForePanel_Tgl0 = mils + 200;
	}
	else
	{
		joystick.setButton(ForePanel_tgl0_ret, t_ForePanel_Tgl0 > mils);
	}
	
	// fore panel rotaries
	joystick.setButton(ForePanel_Rtr1_click, digitalRead(RTR5_Ck) == LOW);
	joystick.setButton(ForePanel_Rtr1_back, rot5.Tback + rotaryPulseTime > mils);
	joystick.setButton(ForePanel_Rtr1_fwd, rot5.Tfwd + rotaryPulseTime > mils);
	joystick.setButton(ForePanel_Rtr2_click, digitalRead(RTR6_Ck) == LOW);
	joystick.setButton(ForePanel_Rtr2_back, rot6.Tback + rotaryPulseTime > mils);
	joystick.setButton(ForePanel_Rtr2_fwd, rot6.Tfwd + rotaryPulseTime > mils);




	delay(16);
}



void readHeadMatrixGroup(int group, int *mxBits)
{
	*mxBits = 0x0;

	pinMode(group, OUTPUT);
	digitalWrite(group, LOW);

	pinMode(Mx0, INPUT_PULLUP);
	if (digitalRead(Mx0) == LOW)
		*mxBits |= 0x1;
	pinMode(Mx0, INPUT);

	pinMode(Mx1, INPUT_PULLUP);
	if (digitalRead(Mx1) == LOW)
		*mxBits |= 0x2;
	pinMode(Mx1, INPUT);

	pinMode(Mx2, INPUT_PULLUP);
	if (digitalRead(Mx2) == LOW)
		*mxBits |= 0x4;		
	pinMode(Mx2, INPUT);

	pinMode(Mx3, INPUT_PULLUP);
	if (digitalRead(Mx3) == LOW)
		*mxBits |= 0x8;
	pinMode(Mx3, INPUT);

	pinMode(group, OUTPUT_OFF);
}



double lerp(double v0, double v1, double t)
{
	return (1.0 - t) * v0 + t * v1;
}

double map(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


//double processAxis(int raw, double response, double inMin, double inMax, double deadzone)
//{
//	double axis = map((double)raw, inMin, inMax, -1.0, 1.0);
//
//	axis = pow(abs(axis), response) * sign(axis);
//	axis = constrain(axis, -1.0, 1.0);
//
//	if (abs(axis) < deadzone)
//		axis = 0.0;
//
//	return axis;
//}

double processAxisAdv(int raw, double response, double inMin, double inCtr, double inMax, double deadzone)
{
	double axis = (double)raw;
	if (abs(axis - inCtr) < deadzone)
		return 0.0;

	if (axis < inCtr)
	{
		axis = map(axis + deadzone, inMin + deadzone, inCtr, -1.0, 0.0);
	}
	if (axis > inCtr)
	{
		axis = map(axis - deadzone, inCtr, inMax - deadzone, 0.0, 1.0);
	}

	axis = pow(abs(axis), response) * sign(axis);
	axis = constrain(axis, -1.0, 1.0);


	return axis;
}



int hatAngle;
void setHat(int angle, int firstButton)
{
	// the 'hat' switch is actually a group of buttons, because some games apparently read hats as other axes
	if (angle != -1)
	{
		if (angle < 360.0) angle += 360;
		if (angle > 360.0) angle -= 360;
		hatAngle = angle / 90; // / 45;
	}
	else
	{
		hatAngle = -1;
	}

	joystick.setButton(firstButton, hatAngle == 0);
	joystick.setButton(firstButton + 1, hatAngle == 1);
	joystick.setButton(firstButton + 2, hatAngle == 2);
	joystick.setButton(firstButton + 3, hatAngle == 3);
	//joystick.setButton(firstButton + 4, hatAngle == 4);
	//joystick.setButton(firstButton + 5, hatAngle == 5);
	//joystick.setButton(firstButton + 6, hatAngle == 6);
	//joystick.setButton(firstButton + 7, hatAngle == 7);
}


int navAngle;
void setNavHat(bool up, bool right, bool down, bool left)
{
	double iy = down ? .1 : (up ? -.1 : 0.0);
	double ix = right ? .1 : (left ? -.1 : 0.0);
		

	if (ix != 0 || iy != 0)
	{
		navAngle = atan2(iy, ix) * RAD_TO_DEG;
		navAngle += 112.5;
		//navAngle = (int)navAngle % 360;

		joystick.setHatSwitch(1, navAngle);
	}
	else
	{
		joystick.setHatSwitch(1, -1);
	}
	
}



void interrupt_ROT3()
{
	encoderRead(RTR3_A, RTR3_B, &rot3, false);
}
void interrupt_ROT4()
{
	encoderRead(RTR4_A, RTR4_B, &rot4, false);
}
void interrupt_ROT5()
{
	encoderRead(RTR5_A, RTR5_B, &rot5, false);
}
void interrupt_ROT6()
{
	encoderRead(RTR6_A, RTR6_B, &rot6, false);
}
// less-stable encoder read, but works well for the concentric module ones. 
void encoderRead(int pinA, int pinB, volatile RotaryEncoder* rot, bool useScrollWheel)
{
	int a = digitalRead(pinA);
	int b = digitalRead(pinB);
	
	if (a != rot->a)
	{
		rot->a = a;
		if (b != rot->b)
		{
			rot->b = b;


			if (rot->Tfwd > millis() - rotaryPulseTime || rot->Tback > millis() - rotaryPulseTime)
				return;

			if (a == b)
			{
				rot->Tfwd = millis();
				if (useScrollWheel) Mouse.move(0, 0, 1);
			}
			else
			{
				rot->Tback = millis();
				if (useScrollWheel) Mouse.move(0, 0, -1);
			}
		}
	}
}




void interrupt_ROT0_re()
{
	encoderStableRead_RisingEdge(RTR0_B, &rtr0);
}
void interrupt_ROT0_fe()
{
	encoderStableRead_FallingEdge(RTR0_B, &rtr0);
}

void interrupt_ROT1_re() 
{
	encoderStableRead_RisingEdge(RTR1_B, &rtr1); 
}

void interrupt_ROT1_fe() 
{
	encoderStableRead_FallingEdge(RTR1_B, &rtr1); 
}

void interrupt_ROT2_re()
{
	encoderStableRead_RisingEdge(RTR2_B, &rtr2); 
}
void interrupt_ROT2_fe()
{
	encoderStableRead_FallingEdge(RTR2_B, &rtr2); 
}




uint32_t rtrLockTimeMS = 8; // tuned to something that seems good. Probably different for each encoder. could maybe make this a member of the Rotary class. 8ms seems to give the best reads for both

// a more stable way to read bouncy-af encoders, as seen on Hackaday:
// https://hackaday.com/2022/04/20/a-rotary-encoder-how-hard-can-it-be/
// reads hopefully-not-bouncing pin B on transitions of pin A
void encoderStableRead_RisingEdge(int pinB, volatile Rotary* rtr)
{
	int b = digitalRead(pinB);
	if (rtr->Tlock != -1 && rtr->bLast == b) return;

	rtr->Tlock = millis() + rtrLockTimeMS;
	rtr->bLast = b;

}
void encoderStableRead_FallingEdge(int pinB, volatile Rotary* rtr)
{
	int b;
	
	if (rtr->Tlock < millis())
	{
		rtr->Tlock = -1;
		rtr->bLast = 2; // impossible value to force a read
	}

	if (rtr->Tlock != -1) return;

	b = digitalRead(pinB);

	if (b == rtr->bLast) return;


	if (b)
	{
		if (rtr->Tback + rotaryPulseTime > millis()) return; // no sign changes while we are still pulsing the other way

		rtr->Tfwd = millis();
		rtr->count++;
	}
	else
	{
		if (rtr->Tfwd + rotaryPulseTime > millis()) return; // no sign changes while we are still pulsing the other way

		rtr->Tback = millis();
		rtr->count--;
	}

	rtr->Tlock = mils + rtrLockTimeMS;
	rtr->bLast = b;

}
