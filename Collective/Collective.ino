
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
#define AXIS_X 0
#define AXIS_Y 1
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

// fore panel L/R toggle
#define FORE_TGL0_L 39
#define FORE_TGL0_R 41

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


// Fore panel -----------------

#define ForePanel_tgl0_left 36
#define ForePanel_tgl0_rgt 37
#define ForePanel_tgl0_ret 38

int t_ForePanel_Tgl0;

// Dev Panel --------------------

#define DEV_CRS 6
#define DEV_SCRWH 7
#define DEV_POVTBS 5

//-------------------------------

// Aft panel -----------------

#define AftPanel_Rtr1_back 28
#define AftPanel_Rtr1_fwd 29
#define AftPanel_Rtr1_click 30

#define AftPanel_Rtr2_back 31
#define AftPanel_Rtr2_fwd 32
#define AftPanel_Rtr2_click 33

#define AftPanel_SW_up 34
#define AftPanel_SW_dn 35
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
AxisCalibration axisColl = AxisCalibration(0, 2048, 4095);
AxisCalibration axisTwist = AxisCalibration(0, 2048, 4095);
AxisCalibration axisX = AxisCalibration(1560, 2723, 3580, 180);
AxisCalibration axisY = AxisCalibration(1630, 2813, 4095);


int headA;
int headB;
int headC;
int headD;
int headE;

double xOut, yOut, zOut, tOut;
double stickAngle;

bool suppressThumbstickButtons;
bool useThbstickHat;
bool useThbCursor;
bool useScrollWheel;

Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 39, 2, true, true, true, false, false, false, false, true, false, false, false);


const uint32_t rotaryPulseTime = 20;

volatile uint32_t rot0_Tback;
volatile uint32_t rot0_Tfwd;
volatile int rot0_a = 0;
volatile int rot0_b = 0;

volatile uint32_t rot1_Tback;
volatile uint32_t rot1_Tfwd;
volatile int rot1_a = 0;
volatile int rot1_b = 0;

volatile uint32_t rot2_Tback;
volatile uint32_t rot2_Tfwd;
volatile int rot2_a = 0;
volatile int rot2_b = 0;

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


	//grip btns
	pinMode(BTN4, INPUT_PULLUP);
	pinMode(BTN5, INPUT_PULLUP);

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

	// fore panel inputs 
	pinMode(FORE_TGL0_L, INPUT_PULLUP);
	pinMode(FORE_TGL0_R, INPUT_PULLUP);
	   
	   
	rot0_Tback = 0;
	rot0_Tfwd  = 0;
	rot1_Tback = 0;
	rot1_Tfwd = 0;
	rot2_Tback = 0;
	rot2_Tfwd = 0;

	attachInterrupt(digitalPinToInterrupt(RTR0_A), interrupt_ROT0, CHANGE);
	attachInterrupt(digitalPinToInterrupt(RTR1_A), interrupt_ROT1, CHANGE);
	attachInterrupt(digitalPinToInterrupt(RTR2_A), interrupt_ROT2, CHANGE);

	analogReadResolution(12);

	joystick.begin();
	joystick.setXAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setYAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setZAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setThrottleRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	
	Mouse.begin();

	SerialUSB.begin(115200);
}

uint32_t mils;
uint32_t tLastCrsRecenter = 0;


void loop()
{
	mils = millis();



	// dev panel switches before anything else, as they control device options
	// pins 5, 6, 7
	//SerialUSB.println((String)"PIN 5: " + digitalRead(5) + " | PIN 6: " + digitalRead(6) + " | PIN 7: " + digitalRead(7));
	bool crsToggle = !digitalRead(DEV_CRS);
	if (crsToggle != useThbCursor)
	{
		useThbCursor = crsToggle;
		
		if (crsToggle)
			tLastCrsRecenter = mils;
	}
	useScrollWheel = !digitalRead(DEV_SCRWH);
	useThbstickHat = !digitalRead(DEV_POVTBS);




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
	
	// rotary 0 (head)
	joystick.setButton(Rtr0_Click, digitalRead(RTR0_Ck) == LOW);
	joystick.setButton(Rtr0_back, rot0_Tback + rotaryPulseTime > millis());
	joystick.setButton(Rtr0_fwd, rot0_Tfwd + rotaryPulseTime > millis());
	   

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
		xOut = lerp(xOut, -processAxisAdv(analogRead(AXIS_X), 1.0, axisX.min, axisX.center, axisX.max, axisX.deadzone), 0.5);
		yOut = lerp(yOut, -processAxisAdv(analogRead(AXIS_Y), 1.0, axisY.min, axisY.center, axisY.max, axisY.deadzone), 0.5);

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


	// rotary 1 (aftPanel left)
	joystick.setButton(AftPanel_Rtr1_click, digitalRead(RTR1_Ck) == LOW);
	joystick.setButton(AftPanel_Rtr1_back, rot1_Tback + rotaryPulseTime > mils);
	joystick.setButton(AftPanel_Rtr1_fwd, rot1_Tfwd + rotaryPulseTime > mils);
	// rotary 2 (aftPanel right)
	joystick.setButton(AftPanel_Rtr2_click, digitalRead(RTR2_Ck) == LOW);
	joystick.setButton(AftPanel_Rtr2_back, rot2_Tback + rotaryPulseTime > mils);
	joystick.setButton(AftPanel_Rtr2_fwd, rot2_Tfwd + rotaryPulseTime > mils);
	// aft panel switch
	joystick.setButton(AftPanel_SW_up, digitalRead(AFT_SW0) == LOW);
	joystick.setButton(AftPanel_SW_dn, digitalRead(AFT_SW1) == LOW);

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
	



	// uncomment for debugging out axis values. remember to uncomment SerialUSB.Begin in setup(). SerialUSB (instead of Serial) outputs on the native port :)
	//SerialUSB.println((String)"Axis Ctr: " + axisX.center + " V Raw: " + analogRead(AXIS_X) + " V Out: " + xOut);

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


void interrupt_ROT0()
{
	encoder0Read(RTR0_A, RTR0_B);	
}
void interrupt_ROT1()
{
	encoder1Read(RTR1_A, RTR1_B);
}
void interrupt_ROT2()
{
	encoder2Read(RTR2_A, RTR2_B);
}


void encoder0Read(int pinA, int pinB) 
{
	int a = digitalRead(pinA);
	int b = digitalRead(pinB);
	if (a != rot0_a)
	{
		rot0_a = a;
		if (b != rot0_b)
		{
			rot0_b = b;

			if (a == b)
			{
				rot0_Tfwd = millis();				
				if (useScrollWheel) Mouse.move(0, 0, 1);
			}
			else
			{
				rot0_Tback = millis();
				if (useScrollWheel) Mouse.move(0, 0, -1);
			}
		}
	}
}


void encoder1Read(int pinA, int pinB)
{
	int a = digitalRead(pinA);
	int b = digitalRead(pinB);
	if (a != rot1_a)
	{
		rot1_a = a;
		if (b != rot1_b)
		{
			rot1_b = b;

			if (a == b)
			{
				rot1_Tfwd = millis();
			}
			else
			{
				rot1_Tback = millis();
			}
		}
	}
}


void encoder2Read(int pinA, int pinB)
{
	int a = digitalRead(pinA);
	int b = digitalRead(pinB);
	if (a != rot2_a)
	{
		rot2_a = a;
		if (b != rot2_b)
		{
			rot2_b = b;

			if (a == b)
			{
				rot2_Tfwd = millis();				
			}
			else
			{
				rot2_Tback = millis();				
			}
		}
	}
}
