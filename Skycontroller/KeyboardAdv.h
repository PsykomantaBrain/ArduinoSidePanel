#pragma once


#include <Keyboard.h>
#include <HID.h>

// pressRaw() adds the specified USB raw keycode to the persistent key 
// report and sends the report.
size_t KeyboardAdv_::pressRaw(uint8_t k)
{
	uint8_t i;

	// Add k to the key report only if it's not already present
	// and if there is an empty slot.
	if (_keyReport.keys[0] != k && _keyReport.keys[1] != k &&
		_keyReport.keys[2] != k && _keyReport.keys[3] != k &&
		_keyReport.keys[4] != k && _keyReport.keys[5] != k) {

		for (i = 0; i < 6; i++) {
			if (_keyReport.keys[i] == 0x00) {
				_keyReport.keys[i] = k;
				break;
			}
		}
		if (i == 6) {
			setWriteError();
			return 0;
		}
	}
	sendReport(&_keyReport);
	return 1;
}


// releaseRaw() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
size_t KeyboardAdv_::releaseRaw(uint8_t k)
{
	uint8_t i;

	// Test the key report to see if k is present.  Clear it if it exists.
	// Check all positions in case the key is present more than once (which it shouldn't be)
	for (i = 0; i < 6; i++) {
		if (0 != k && _keyReport.keys[i] == k) {
			_keyReport.keys[i] = 0x00;
		}
	}


	sendReport(&_keyReport);
	return 1;
}