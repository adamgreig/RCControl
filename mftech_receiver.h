#pragma once
#include "stdafx.h"

#include <mmsystem.h>
#include <winbase.h>

/**
* Connect to an MFTech R/C receiver, represented on the system
* as a joystick, and read in the X and Y axis values to be
* used as throttle and steeling data. The implementation is
* significantly different under Linux and Windows, though the
* external interface is the same.
*/
class MFTechReceiver {
public:
	MFTechReceiver(void);
	bool joystick_available();
	int throttle();
	int steering();
	int modeselect();
};
