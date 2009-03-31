#include "mftech_receiver.h"

///Open the joystick in Linux
MFTechReceiver::MFTechReceiver(void) {
#if LINUX
	joystick = open(JOYSTICK, O_RDONLY|O_NONBLOCK);
#endif
}


///Get the throttle position between 0 and 65535
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
