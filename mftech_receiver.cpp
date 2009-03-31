#include "mftech_receiver.h"

/**
* On Linux, open the joystick file handler.
*/
MFTechReceiver::MFTechReceiver(void) {
#if LINUX
	joystick = open(JOYSTICK, O_RDONLY|O_NONBLOCK);
#endif
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
#if WINDOWS
	JOYINFOEX joyInfoEx;
	ZeroMemory(&joyInfoEx, sizeof(joyInfoEx));
	joyInfoEx.dwSize = sizeof(joyInfoEx);
	if( joyGetPosEx(JOYSTICKID1, &joyInfoEx) != JOYERR_NOERROR ) {
		printf("Error loading joystick.\n");
		return -1;
	}
	return joyInfoEx.dwYpos;
#elif LINUX
	js_event e;
	//On Linux, we instead check to see if any events occured
	//with the joystick changing position, update the local
	//steering and throttle, then return the required data.
	//The data is -32767 to +32767 instead of 0 to 65535, so
	//make it unto an unsigned int for consistency.
	while( read(joystick, &e, sizeof(struct js_event)) != -1 ) {
		if( e.type == 0x02 || e.type == 0x82 ) {
			if( e.number == 0 ) {
				steering_val = e.value + 32767;
			} else if( e.number == 1 ) {
				throttle_val = e.value + 32767;
			}
		}
	}
	return throttle_val;
#endif
}

/**
* Return the current steering position.
* See MFTech::throttle for implementation details.
*/
int MFTechReceiver::steering() {
#if WINDOWS
	JOYINFOEX joyInfoEx;
	ZeroMemory(&joyInfoEx, sizeof(joyInfoEx));
	joyInfoEx.dwSize = sizeof(joyInfoEx);
	if( joyGetPosEx(JOYSTICKID1, &joyInfoEx) != JOYERR_NOERROR ) {
		printf("Error loading joystick.\n");
		return -1;
	}
	return joyInfoEx.dwXpos;
#elif LINUX
	js_event e;
	//On Linux, we instead check to see if any events occured
	//with the joystick changing position, update the local
	//steering and throttle, then return the required data.
	//The data is -32767 to +32767 instead of 0 to 65535, so
	//make it unto an unsigned int for consistency.
	while( read(joystick, &e, sizeof(struct js_event)) != -1 ) {
		if( e.type == 0x02 || e.type == 0x82 ) {
			if( e.number == 0 ) {
				steering_val = e.value + 32767;
			} else if( e.number == 1 ) {
				throttle_val = e.value + 32767;
			}
		}
	}
	return steering_val;
#endif
}
