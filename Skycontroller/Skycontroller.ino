

#include "Joystick.h"
#include "Mouse.h"

#define sign(x) ((x) > 0 ? 1: ((x) < 0 ? -1 : 0))

//High impedance pin mode
#define OUTPUT_OFF INPUT_PULLUP

// axis output 
#define JOYSTICK_RANGE_MIN 0
#define JOYSTICK_RANGE_MAX 1023

Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 
								8, 0,				//btns, hats
								true, true, true,	// X, Y, Z
								true, true, true,	// Rx, Ry, Rz
								true, true,			// rudder, throttle
								false, false, false //accel, brake, steering
							);





// placeholder code, just flash 'HRV' in morse code on the builtin LED

char* letters[] = {
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R 
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

char* text = "HRV";
int dotDelay = 80;





void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	
	analogReadResolution(12);

	joystick.begin();
	joystick.setXAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setYAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setZAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setThrottleRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setRxAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
	joystick.setRyAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);

	Mouse.begin();

	
}

void loop()
{
	flashMorse(text);
}

void flashMorse(char* txt)
{
	delay(dotDelay * 6);
	
	int i = 0;
	do
	{
		char c = txt[i];
		if (c >= 'A' && c <= 'Z')
		{
			flashChar( letters[c - 'A']);
		}
		else if (c >= 'a' && c <= 'z')
		{
			flashChar(letters[c - 'a']);
		}
		else if (c == ' ')
		{
			flashChar(" ");
		}
		i++;
	} while (txt[i] != NULL);
}

void flashChar(char* sequence)
{
	delay(dotDelay * 4);
	
	int i = 0;
	do 
	{
		flashDotOrDash(sequence[i]);		
		i++;
	} while (sequence[i] != NULL);
}


void flashDotOrDash(char dotOrDash) 
{	
	if (dotOrDash == '.')
	{
		digitalWrite(LED_BUILTIN, HIGH);
		delay(dotDelay);
		
		digitalWrite(LED_BUILTIN, LOW); 
		delay(dotDelay * 2);
	}
	else if (dotOrDash == '-')
	{
		digitalWrite(LED_BUILTIN, HIGH);
		delay(dotDelay * 3);
		
		digitalWrite(LED_BUILTIN, LOW); 
		delay(dotDelay * 2);
	}
	else if (dotOrDash == ' ')
	{
		//digitalWrite(LED_BUILTIN, LOW);
		delay(dotDelay * 3);
	}
	
}