# ArduinoSidePanel
Code for an Arduino-powered Logitech (Saitek) Side Panel


The Side Panel's controller PCB was replaced by an Arduino Leonardo, coupled with a Mayhew Labs Multiplexer Shield.

The pots, rotary encoder and buttons are read by the Arduino, which processes their input to drive a custom joystick device in Windows.

The Side Panel's original joystick device cannot be calibrated, and has a very large deadzone in all axes by default. This custom controller allows for complete customization of all axes, as well as mapping of its controls to any type of input (coming eventually).

