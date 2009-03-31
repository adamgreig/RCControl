/**
* \file Code entry point and main application logic.
*/

#include "stdafx.h"

#include "pololu_servo_controller.h"
#include "mftech_receiver.h"
#include "file_logger.h"

//Windows apparently prefers the top form, while
// Linux doesn't really do _TCHAR or _tmain.
#if WINDOWS
int _tmain(int argc, _TCHAR* argv[]) {
#elif LINUX
int main(int argc, char* argv[]) {
#endif
	printf("Initialising...\n");

    //Make a controller & receiver
    PololuServoController controller = PololuServoController();
	MFTechReceiver receiver = MFTechReceiver();

	//Make a logger
	char logpath[32] = "log.txt";
	FileLogger log = FileLogger(logpath);

	//Wait a second for the serial device to initialise
	sleep(1000);

	//Start acting as a middleman
	printf("Now relaying data.\n");

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
		
		//Windows is a big fan of sprintf_s while Linux remains true to C and uses sprintf.
		#if WINDOWS
		sprintf_s(buf, "%d\t%d", (int)steering, (int)throttle);
		#elif LINUX
		sprintf(buf, "%d\t%d", (int)steering, (int)throttle);
		#endif

		log.log(buf);
		sleep(50);
	}
	return 0;
}
