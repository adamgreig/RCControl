#include "mftech_receiver.h"

/**
* On Linux, open the joystick file handler.
*/
MFTechReceiver::MFTechReceiver(void) {

}


/**
* Return the current throttle position.
* On Windows, this will request a new joystick info struct
* and obtain the throttle as the Y axis value of this.
* On Linux, the joystick file handler is read for events until
* none are left to process. Each event is used to update the
* internal values of the throttle and steering, and then the
* final internal value is returned when no events are left.
*/
int MFTechReceiver::throttle() {
	JOYINFOEX joyInfoEx;
	ZeroMemory(&joyInfoEx, sizeof(joyInfoEx));
	joyInfoEx.dwSize = sizeof(joyInfoEx);
	if( joyGetPosEx(JOYSTICKID1, &joyInfoEx) != JOYERR_NOERROR ) {
		cout << "Error loading joystick." << endl;
		return -1;
	}
	return joyInfoEx.dwYpos;
}

/**
* Return the current steering position.
* See MFTech::throttle for implementation details.
*/
int MFTechReceiver::steering() {
	JOYINFOEX joyInfoEx;
	ZeroMemory(&joyInfoEx, sizeof(joyInfoEx));
	joyInfoEx.dwSize = sizeof(joyInfoEx);
	if( joyGetPosEx(JOYSTICKID1, &joyInfoEx) != JOYERR_NOERROR ) {
		cout << "Error loading joystick." << endl;
		return -1;
	}
	return joyInfoEx.dwXpos;
}

/**
* Return the current 'x-rotation' (used as mode select)
* See MFTech::throttle for implementation details.
*/
int MFTechReceiver::modeselect() {
	JOYINFOEX joyInfoEx;
	ZeroMemory(&joyInfoEx, sizeof(joyInfoEx));
	joyInfoEx.dwSize = sizeof(joyInfoEx);
	if( joyGetPosEx(JOYSTICKID1, &joyInfoEx) != JOYERR_NOERROR ) {
		cout << "Error loading joystick." << endl;
		return -1;
	}
	return joyInfoEx.dwVpos;
}
