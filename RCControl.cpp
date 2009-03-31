// RCControl.cpp : Defines the entry point for the console application.
//

#if WINDOWS
#include "stdafx.h"
#include <windows.h>
#endif
#include "pololu_servo_controller.h"
#include "mftech_receiver.h"
#include "file_logger.h"

#if WINDOWS
int _tmain(int argc, _TCHAR* argv[]) {
#elif LINUX
int main(int argc, char* argv[]) {
#endif
	printf("Initialising...\n");
    //Make a controller & receiver
    PololuServoController controller = PololuServoController();
	MFTechReceiver receiver = MFTechReceiver();
	char logpath[32] = "log.txt";
	FileLogger log = FileLogger(logpath);

	//Wait a second for the serial device to initialise
	#if WINDOWS
	Sleep(1000);
	#elif LINUX
	usleep(1000000);
	#endif
	printf("Now relaying data.\n");
	//Start acting as a middleman
	for(;;) {
		//Read the throttle and steering as a double
		double throttle = (double)receiver.throttle();
		double steering = (double)receiver.steering();
		//Convert from 0 to 0xFFFF to 500 to 5500 (servo range)
		steering /= 65535.0;
		steering *= 5000.0;
		steering += 500.0;
		throttle /= 65535.0;
		throttle *= 5000.0;
		throttle += 500.0;
		//Set position
		controller.set_position_abs(1, (int)steering);
		controller.set_position_abs(2, (int)throttle);
		//Display and log data
		char buf[256];
		printf("%.5d\t%.5d\r", (int)steering, (int)throttle);
		#if WINDOWS
		sprintf_s(buf, "%d\t%d", (int)steering, (int)throttle);
		#elif LINUX
		sprintf(buf, "%d\t%d", (int)steering, (int)throttle);
		#endif
		log.log(buf);
		//Wait 50ms
		#if WINDOWS
		Sleep(50);
		#elif LINUX
		usleep(50000);
		#endif
	}
	return 0;
}
