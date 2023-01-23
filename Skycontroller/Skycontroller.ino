

#include "src/Joystick.h"
#include "Mouse.h"
#include "KeyboardAdv.h"

#define sign(x) ((x) > 0 ? 1: ((x) < 0 ? -1 : 0))

//High impedance pin mode
#define OUTPUT_OFF INPUT_PULLUP


// input pins for LX/Y and RX/Y axes (analogs 0-3)
#define PIN_A_LX 0
#define PIN_A_LY 1
#define PIN_A_RX 3
#define PIN_A_RY 2


//input pins for slew (thumbstick) axes and thumbstick buttons (analog 4-7 and digital 51 and 53)
#define PIN_A_SLX 4
#define PIN_A_SLY 5
#define PIN_SLB 42

#define PIN_A_SRX 6
#define PIN_A_SRY 7
#define PIN_SRB 44


// even pins from 30 to 48 are used for buttons
#define PIN_BTN_FRM 30
#define PIN_BTN_HOME 32
#define PIN_BTN_UPDN 34
#define PIN_BTN_REC 36
#define PIN_BTN_BACK 38
#define PIN_BTN_GEAR 40


#define JOYSTICK_TRAVEL joystick.GetAxisTravel()
#define JOYSTICK_CENTER joystick.GetAxisCenter()




enum androidKeyMap 
{
	ANDROID_KEY_A = 96,
	ANDROID_KEY_B = 97,
	ANDROID_KEY_C = 98,
	ANDROID_KEY_X = 99,
	ANDROID_KEY_Y = 100,
	ANDROID_KEY_Z = 101,

	ANDROID_KEY_L1 = 102,
	ANDROID_KEY_L2 = 104,
	ANDROID_KEY_R1 = 103,
	ANDROID_KEY_R2 = 105,

	ANDROID_KEY_THUMBL = 106,
	ANDROID_KEY_THUMBR = 107,

	ANDROID_KEY_MODE = 110,


	ANDROID_KEY_SELECT = 109,
	ANDROID_KEY_START = 108,

	ANDROID_KEY_DPAD_UP = 19,
	ANDROID_KEY_DPAD_DOWN = 20,
	ANDROID_KEY_DPAD_LEFT = 21,
	ANDROID_KEY_DPAD_RIGHT = 22,
	ANDROID_KEY_DPAD_CENTER = 23,

	ANDROID_KEY_BUTTON_1 = 188,
	ANDROID_KEY_BUTTON_2 = 189,
	ANDROID_KEY_BUTTON_3 = 190,
	ANDROID_KEY_BUTTON_4 = 191,
	ANDROID_KEY_BUTTON_5 = 192,
	ANDROID_KEY_BUTTON_6 = 193,
	ANDROID_KEY_BUTTON_7 = 194,
	ANDROID_KEY_BUTTON_8 = 195,
	ANDROID_KEY_BUTTON_9 = 196,
	ANDROID_KEY_BUTTON_10 = 197,
	ANDROID_KEY_BUTTON_11 = 198,
	ANDROID_KEY_BUTTON_12 = 199,
	ANDROID_KEY_BUTTON_13 = 200,
	ANDROID_KEY_BUTTON_14 = 201,
	ANDROID_KEY_BUTTON_15 = 202,
	ANDROID_KEY_BUTTON_16 = 203

};
// assign buttons to keyboard keys
const uint32_t keyMap[] = 
{
	/*BTN_KEY_UPDN*/ ANDROID_KEY_A,
	/*BTN_KEY_HOME*/ ANDROID_KEY_B,
	/*BTN_KEY_FRM*/ ANDROID_KEY_R2,
	
	/*BTN_KEY_BACK*/ ANDROID_KEY_X,
	/*BTN_KEY_GEAR*/ ANDROID_KEY_Y,
	/*BTN_KEY_REC*/ ANDROID_KEY_L2,
	
	/*BTN_KEY_SLB*/ ANDROID_KEY_L1,
	/*BTN_KEY_SRB*/ ANDROID_KEY_R1
};

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
			return 0.0;

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


		return axis;
	}


	double map(double x, double in_min, double in_max, double out_min, double out_max)
	{
		return (x - in_min) / (in_max - in_min) * (out_max - out_min) + out_min;
	}
};

double lerp(double v0, double v1, double t)
{
	return (1.0 - t) * v0 + t * v1;
}

AxisCalibration axisLX = AxisCalibration(395, 2100, 3760, 15);
AxisCalibration axisLY = AxisCalibration(315, 2280, 4056, 15);
AxisCalibration axisRX = AxisCalibration(318, 2100, 3820, 15);
AxisCalibration axisRY = AxisCalibration(383, 2100, 3845, 15);

AxisCalibration axisSLX = AxisCalibration(790, 2080, 3200, 50);
AxisCalibration axisSLY = AxisCalibration(890, 2050, 3140, 50);
AxisCalibration axisSRX = AxisCalibration(1025, 2020, 3370, 50);
AxisCalibration axisSRY = AxisCalibration(980, 2020, 3220, 50);



double cos225, sin225;

Joystick_ joystick;

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
	pinMode(PIN_BTN_HOME, INPUT_PULLUP);
	pinMode(PIN_BTN_FRM, INPUT_PULLUP);
	pinMode(PIN_BTN_UPDN, INPUT_PULLUP);
	pinMode(PIN_BTN_BACK, INPUT_PULLUP);
	pinMode(PIN_BTN_GEAR, INPUT_PULLUP);
	pinMode(PIN_BTN_REC, INPUT_PULLUP);
	
	analogReadResolution(12);
	


	// select mode based on FRM or REC button held down 
	JoystickMode mode = !digitalRead(PIN_BTN_FRM) || !digitalRead(PIN_BTN_REC) ? Mode_ANDROID : Mode_WIN;



	joystick = Joystick_(mode,
		JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
		12, 2,					//btns, hats
		true, true, true, true,	// X (lx), Y (ly), Z (rx), Rx (ry)
		true, true, true, true,	// Ry (slx), Rz (sly), s0 (srx), s1(sry)
		false, false, false		//accel, brake, steering
	);
	
	joystick.begin(false);

	
	Mouse.begin();

	
	//Serial.begin(115200);
	//SerialUSB.println((String)"HRV Skycontroller");

	cos225 = cos(PI / 8);
	sin225 = sin(PI / 8);
}

double lx, ly, rx, ry;
double slx, sly, srx, sry;

double rotSlx, rotSly, rotSrx, rotSry;

byte mouseBtns = 0;
uint32_t keyBtns = 0;

void loop()
{
	
	// print analog pins 0-7 and digital pins 51 and 53	
	//for (int i = 0; i < 8; i++)
	//{
	//	SerialUSB.print((String)"A" + i + ": " + analogRead(i) + " | ");		
	//}
	//SerialUSB.println((String)"D30: " + digitalRead(30) + " | D32: " + digitalRead(32));
	//SerialUSB.println((String)"D34: " + digitalRead(34) + " | D36: " + digitalRead(36));
	//SerialUSB.println((String)"D38: " + digitalRead(38) + " | D40 : " + digitalRead(40));
	//
	//SerialUSB.println();


	lx = lerp(lx, axisLX.processAxis(analogRead(PIN_A_LX)), 0.5);
	ly = lerp(ly, axisLY.processAxis(analogRead(PIN_A_LY)), 0.5);
	joystick.setXAxis(-lx * JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setYAxis(ly * JOYSTICK_TRAVEL + JOYSTICK_CENTER);


	//Serial.println((String)"LX: " + (-lx * JOYSTICK_TRAVEL + JOYSTICK_CENTER) + " | LY: " + (int16_t)(ly * JOYSTICK_TRAVEL + JOYSTICK_CENTER));
	
	rx = lerp(rx, axisRX.processAxis(analogRead(PIN_A_RX)), 0.5);
	ry = lerp(ry, -axisRY.processAxis(analogRead(PIN_A_RY)), 0.5);
	joystick.setZAxis(rx * JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setRzAxis(ry * JOYSTICK_TRAVEL + JOYSTICK_CENTER);

	slx = lerp(slx, -axisSLX.processAxis(analogRead(PIN_A_SLX)), 0.4);
	sly = lerp(sly, axisSLY.processAxis(analogRead(PIN_A_SLY)), 0.4);

	// rotate axes slx and sly by 25 degrees ccw
	rotSlx = slx * cos225 - sly * -sin225;
	rotSly = slx * -sin225 + sly * cos225;

	Mouse.move(rotSlx * 32, rotSly * 32, 0);
	
	joystick.setS0Axis(rotSlx * JOYSTICK_TRAVEL + JOYSTICK_CENTER);	
	joystick.setS1Axis(rotSly * JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setButton(6, !digitalRead(PIN_SLB));
	
	srx = lerp(srx, axisSRX.processAxis(analogRead(PIN_A_SRX)), 0.4);
	sry = lerp(sry, -axisSRY.processAxis(analogRead(PIN_A_SRY)), 0.4);

	// rotate axis srx and sry by 25 degrees cw
	rotSrx = srx * cos225 - sry * sin225;
	rotSry = sry * cos225 + srx * sin225;
	

	joystick.setRxAxis(rotSrx * JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setRyAxis(rotSry * JOYSTICK_TRAVEL + JOYSTICK_CENTER);
	joystick.setButton(7, !digitalRead(PIN_SRB));
	
	Mouse.move(rotSrx * 32, rotSry * 32, 0);
	
	if (joystick.joystick_mode != Mode_ANDROID)
	{
		joystick.setButton(0, !digitalRead(PIN_BTN_UPDN));
		joystick.setButton(1, !digitalRead(PIN_BTN_HOME));
		joystick.setButton(2, !digitalRead(PIN_BTN_GEAR));
		joystick.setButton(3, !digitalRead(PIN_BTN_BACK));
		joystick.setButton(4, !digitalRead(PIN_BTN_REC));
		joystick.setButton(5, !digitalRead(PIN_BTN_FRM));		
	}


	joystick.sendState();

	byte mBtns = 0;
	if (!digitalRead(PIN_SLB)) mBtns |= MOUSE_LEFT;
	if (!digitalRead(PIN_SRB)) mBtns |= MOUSE_RIGHT;
	

	if (mBtns != mouseBtns)
	{
		if (Mouse.isPressed(MOUSE_LEFT) && !(mBtns & MOUSE_LEFT))
			Mouse.release(MOUSE_LEFT);
		else if (!Mouse.isPressed(MOUSE_LEFT) && (mBtns & MOUSE_LEFT))
			Mouse.press(MOUSE_LEFT);
		
		if (Mouse.isPressed(MOUSE_RIGHT) && !(mBtns & MOUSE_RIGHT))
			Mouse.release(MOUSE_RIGHT);
		else if (!Mouse.isPressed(MOUSE_RIGHT) && (mBtns & MOUSE_RIGHT))
			Mouse.press(MOUSE_RIGHT);

		
		mouseBtns = mBtns;
	}

	// in android mode, send keypresses for buttons
	//if (joystick.joystick_mode == Mode_ANDROID)
	{
		// read buttons into keyBtns bits
		uint32_t kBtns = 0;
		if (!digitalRead(PIN_BTN_UPDN)) kBtns |= 1 << 0;
		if (!digitalRead(PIN_BTN_HOME)) kBtns |= 1 << 1;
		if (!digitalRead(PIN_BTN_GEAR)) kBtns |= 1 << 2;
		if (!digitalRead(PIN_BTN_BACK)) kBtns |= 1 << 3;
		if (!digitalRead(PIN_BTN_REC)) kBtns |= 1 << 4;
		if (!digitalRead(PIN_BTN_FRM)) kBtns |= 1 << 5;		
		if (!digitalRead(PIN_SLB)) kBtns |= 1 << 6;
		if (!digitalRead(PIN_SRB)) kBtns |= 1 << 7;		


		// press/release keys based on button map		
		if (keyBtns != kBtns)
		{
			for (int i = 0; i < 8; i++)
			{
				if (!(keyBtns & (1 << i)) && (kBtns & (1 << i)))
					Keyboard.press(keyMap[i]);
				else if (keyBtns & (1 << i) && !(kBtns & (1 << i)))
					Keyboard.release(keyMap[i]);				
			}
			
			keyBtns = kBtns;
		}

	}



	delay(16);
}
