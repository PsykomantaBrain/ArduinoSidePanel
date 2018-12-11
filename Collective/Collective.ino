
#include "Joystick.h"

Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 52, 0, true, true, true, true, true, true, false, false, false, false, false);

double a, o;void setup(){	Serial.begin(57600);	analogReadResolution(12);
	joystick.begin();
	joystick.setXAxisRange(-16384, 16384);}void loop(){	int a1 = analogRead(0);	a = lerp(a, (double)a1, 0.5);	o = map(a, 0.0, 4096.0, -1.0, 1.0);	
	joystick.setXAxis(o * 16384.0);	delay(20);}double lerp(double v0, double v1, double t){	return (1.0 - t) * v0 + t * v1;}double map(double x, double in_min, double in_max, double out_min, double out_max){	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;}