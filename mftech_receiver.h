#pragma once
#include "stdafx.h"

#if WINDOWS
#include <windows.h>
#include <mmsystem.h>
#include <winbase.h>
#elif LINUX
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#define JOYSTICK "/dev/input/js0"
struct js_event {
	__u32 time;
	__s16 value;
	__u8 type;
	__u8 number;
};
#endif

#include <stdio.h>

///Read joystick data being read in from the MFTech Receiver
class MFTechReceiver {
public:
	MFTechReceiver(void);
	int throttle();
	int steering();
#if LINUX
private:
	int joystick;
	int throttle_val, steering_val;
#endif
};
