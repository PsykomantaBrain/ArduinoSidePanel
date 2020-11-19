// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       Yoke.ino
    Created:	10/21/2020 12:03:28 PM
    Author:     BIGRIGMK12\ffala
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "Joystick.h"
#include <Mouse.h>
#include <Keyboard.h>


#define sign(x) ((x) > 0 ? 1: ((x) < 0 ? -1 : 0))

Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 128, 6, true, true, true, true, true, true, false, false, false, false, false);

//initialize the liquid crystal library
//the first parameter is the I2C address
//the second parameter is how many rows are on your screen
//the third parameter is how many columns are on your screen
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte cc0[8] = {
    B11111,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
};
byte cc1[8] = {
    B11000,
    B00100,
    B00010,
    B00001,    
    B10000,
    B01000,
    B00000,
    B00000,
};
byte cc2[8] = {
    B00011,    
    B00100,
    B01000,
    B10000,
    B00001,    
    B00010,
    B00000,
    B00000,
};
byte cc3[8] = {
    B11111,
    B00000,
    B00000,
    B11111,
    B00000,
    B00000,
    B00000,
    B00000,
};
byte cc4[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,    
    B00000,
    B00000,    
};


//High impedance pin mode
#define OUTPUT_OFF INPUT_PULLUP

// axis output 
#define JOYSTICK_RANGE_MIN 0
#define JOYSTICK_RANGE_MAX 4096


// hdlR rotary pins (RTR0)
#define RTR0_A 38
#define RTR0_B 42
#define RTR0_Ck 40

// hdlR rotary buttons | output
#define Rtr0_Click 8
#define Rtr0_fwd 10
#define Rtr0_back 9

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


AxisCalibration axisRHX = AxisCalibration(1120.0, 2060.0, 3220.0, 50.0);
AxisCalibration axisRHY = AxisCalibration(860.0, 2048.0, 3200.0, 50.0);

AxisCalibration axisLHX = AxisCalibration(1120.0, 2060.0, 3220.0, 50.0);
AxisCalibration axisLHY = AxisCalibration(860.0, 2048.0, 3200.0, 50.0);




const bool useScrollWheel = true;
const uint32_t rotaryPulseTime = 20;

volatile uint32_t rot0_Tback;
volatile uint32_t rot0_Tfwd;
volatile int rot0_a = 0;
volatile int rot0_b = 0;


void setup()
{
    

    Wire.begin();
    lcd.init();
    
    lcd.backlight();
    lcd.setBacklight(128);
    
    delay(2000);
    
    lcd.createChar(0, cc0);    
    lcd.createChar(1, cc1);   
    lcd.createChar(2, cc2);   
    lcd.createChar(3, cc3);   
    lcd.createChar(4, cc4);   
    
    
    
    lcd.setCursor(0, 0);
    lcd.print(" HRV Simviator ");
    lcd.setCursor(0, 1);
    lcd.print("  \x08\x08\x03\x03\x01\x04\x02\x03\x03\x08\x08  ");



    analogReadResolution(12);
    
    joystick.begin();
    joystick.setXAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
    joystick.setYAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
    joystick.setZAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
    joystick.setRxAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
    joystick.setRyAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);
    joystick.setRzAxisRange(JOYSTICK_RANGE_MIN, JOYSTICK_RANGE_MAX);

    pinMode(22, INPUT_PULLUP);
    pinMode(24, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
    pinMode(28, INPUT_PULLUP);
    pinMode(30, INPUT_PULLUP);
    pinMode(32, INPUT_PULLUP);
    pinMode(34, INPUT_PULLUP);
    pinMode(36, INPUT_PULLUP);


    // rotary inputs 
    pinMode(RTR0_A, INPUT_PULLUP);
    pinMode(RTR0_B, INPUT_PULLUP);
    pinMode(RTR0_Ck, INPUT_PULLUP);

    rot0_Tback = 0;
    rot0_Tfwd = 0;
    attachInterrupt(digitalPinToInterrupt(RTR0_A), interrupt_ROT0, CHANGE);

    Mouse.begin();


    //SerialUSB.begin(115200);

}

uint32_t mils;

double pitchFaderA, pitchFaderB, pitch;
double roll;

// Add the main program code into the continuous loop() function
void loop()
{
    mils = millis();

    pitchFaderA = analogRead(A0);
    pitchFaderB = analogRead(A2);
    pitch = pitchFaderA; // the B fader pot looks busted. Until the replacement arrives, we can just skip it.
   // pitch = (pitchFaderA + pitchFaderB) * 0.5f;
    joystick.setYAxis(pitch);
   
    
    roll = JOYSTICK_RANGE_MAX - analogRead(A7);
    joystick.setXAxis(roll);
   


    // rh buttons
    joystick.setButton(0, !digitalRead(32));
    joystick.setButton(1, !digitalRead(36));
    joystick.setButton(2, !digitalRead(34));
    // lh buttons
    joystick.setButton(3, !digitalRead(24));
    joystick.setButton(4, !digitalRead(22));
    joystick.setButton(5, !digitalRead(26));


    // RH thumbstick
    uint32_t a8 = analogRead(A8);
    double rx = axisRHX.processAxis(a8);
    uint32_t a9 = analogRead(A9);
    double ry = axisRHY.processAxis(a9);

    joystick.setZAxis(rx * JOYSTICK_RANGE_MAX);
    joystick.setRxAxis(ry * JOYSTICK_RANGE_MAX);
    joystick.setButton(6, !digitalRead(30));


    // LH thumbstick
    uint32_t a4 = analogRead(A4);
    double lx = axisRHX.processAxis(a4);
    uint32_t a5 = analogRead(A5);
    double ly = axisRHY.processAxis(a5);

    joystick.setRyAxis(lx * JOYSTICK_RANGE_MAX);
    joystick.setRzAxis(ly * JOYSTICK_RANGE_MAX);
    joystick.setButton(7, !digitalRead(28));

    //SerialUSB.println((String)"RX: " + rx + " [" + (a8) + "] |  RY: " + ry + " [" + (a9) + "]");



    // rotary 0 (hdlR)
    joystick.setButton(Rtr0_Click, digitalRead(RTR0_Ck) == LOW);
    joystick.setButton(Rtr0_back, rot0_Tback + rotaryPulseTime > mils);
    joystick.setButton(Rtr0_fwd, rot0_Tfwd + rotaryPulseTime > mils);

    delay(16);
}





void interrupt_ROT0()
{
    encoderRead(RTR0_A, RTR0_B, &rot0_a, &rot0_b, &rot0_Tfwd, &rot0_Tback, useScrollWheel);
}

void encoderRead(int pinA, int pinB, volatile int* a0, volatile int* b0, volatile uint32_t* rot_Tfwd, volatile uint32_t* rot_Tback, bool useScrollWheel)
{
    int a = digitalRead(pinA);
    int b = digitalRead(pinB);
    if (a != *a0)
    {
        *a0 = a;
        if (b != *b0)
        {
            *b0 = b;

            if (a == b)
            {
                *rot_Tfwd = millis();
                if (useScrollWheel) Mouse.move(0, 0, 1);
            }
            else
            {
                *rot_Tback = millis();
                if (useScrollWheel) Mouse.move(0, 0, -1);
            }
        }
    }
}