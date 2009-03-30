#pragma once

#define WINDOWS 1

#if WINDOWS
#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include <winbase.h>
#elif LINUX
#define JOYSTICK "/dev/input/js0"
struct js_event {
	u32 time;
	s16 value;
	u8 type;
	u8 number;
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
	int throttle, steering;
#endif
};
