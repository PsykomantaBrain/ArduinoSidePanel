
#include "Joystick.h"

Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 52, 0, true, true, true, true, true, true, false, false, false, false, false);

double a, o;
	joystick.begin();
	joystick.setXAxisRange(-16384, 16384);
	joystick.setXAxis(o * 16384.0);