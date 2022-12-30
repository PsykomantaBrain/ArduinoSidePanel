# HRV Skycontroller
Code for an Arduino-based board replacement for the Parrot Skycontroller device


The Skycontroller (one) unit that was launched with the Bebop 2 drone looked really cool, but unfortunately it was very incompatilbe with everything that wasn't the bebop drone itself. Parrot's decision to use an android device in the controller meant it isn't a simple joystick, and the joystick itself isn't separate from the rest of the device. All wires just run into a single custom PCB.

That all means there's no way to just plug the SC as a joystick. Hence this project, which aims to replace the android box with an Arduino Due, to use the skycontroller's hardware with an entirely new, HID-compatible controller

The SC hardware itself can stay mostly unmodified, using a custom wiring hookup for the internal JST connectors from the controller components. However, there's nothing that says we can't also add new components now that we've taken over the firmware implementation. There's certainly room in the massive skycontroller to add additional buttons and switches, and there's definitely more than enough pins on the Due to read them.


The only downside is not using that enormous wifi antenna the SC has. However, that thing has its own driver board (outboard from the main device), so it wouldn't be impossible to reuse that if I can find the datasheet for it. 

For now though, I'm ok with just using it over USB. It's not as if there isn't going to be a PC or mobile device in the immediate vicinity for anything I'd use this for anyway.