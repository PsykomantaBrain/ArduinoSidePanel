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

Joystick_ joystick = Joystick_(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 128, 6, true, true, true, true, true, false, false, true, false, false, false);

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
    B10000,
    B01000,
    B00100,
    B00010,
    B10001,
    B01000,
    B00000,
    B00000,
};
byte cc2[8] = {
    B00001,
    B00010,
    B00100,
    B01000,
    B10001,
    B00010,    
    B00000,
    B00000,
};
byte cc3[8] = {
    B11111,
    B0000,
    B00000,
    B11111,
    B00000,
    B00000,
    B00000,
    B00000,
};

void setup()
{
    joystick.begin();

    lcd.init();
    lcd.createChar(0, cc0);    
    lcd.createChar(1, cc1);   
    lcd.createChar(2, cc2);   
    lcd.createChar(3, cc3);   

    
    lcd.backlight();
    lcd.setBacklight(128);

    lcd.setCursor(0, 0);
    lcd.print(" HRV Simviator ");
    lcd.setCursor(0, 1);
    lcd.print("  \x08\x08\x03\x03\x01 \x02\x03\x03\x08\x08  ");

}

// Add the main program code into the continuous loop() function
void loop()
{
    
    delay(16);
}
