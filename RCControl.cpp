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
		controller.set_position_abs(0, (int)steering);
		controller.set_position_abs(1, (int)throttle);

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

/**
\mainpage RCControl Documentation

At the time of writing, the code consists of four classes.

\section pololu Pololu Controller
Two are related to a Pololu Servo Controller: http://www.pololu.com/catalog/product/390

This device simulates a serial port to the computer over USB, and has a straightforward
protocol defined in http://www.pololu.com/file/0J35/usc01a_guide.pdf

It controls up to 16 servos connected directly to it, and requires an external power supply.

PololuSerial is responsible for communicating with it over the serial port. It is not
particularly protocol aware, but does set the port up with the correct settings for
communication (8 data bits, 1 stop bit, no parity bit, no hardware flow control).
PololuServoController defines the Pololu API. Each method takes a servo number and
the data to send. The class also instances PololuSerial for actually sending data.

\section mftech MFTech Receiver
The third class, MFTechReceiver, is related to the MFTech R/C Receiver:
http://www.mftech.de/usb-interface_en.htm#RC_USB-Interface_III

This device reads in data from a remote control transmitter (controller). It either
connects to the transmitter by a buddy cable directly, or by connecting to an R/C
receiver via the servo cables. The latter method is to be used on the car as this
allows the transmitter to transmit normally over radio, then the receiver picks the
signals up and sends the data on to the MFTech interface which the computer reads.
It simulates a joystick to the computer, using the normal USB HID protocol. This is
read as a joystick using some special Windows commands, or as a normal file handler
on Linux which generates events whenever the joystick does anything. In both cases
the class interface is the same.

\section logger Logger
The final class, FileLogger, logs this data to a file. This is to be used with other sensors for
recording how a human driver controlled the car, with the aim of having the onboard
computer eventually do the driving itself. Other data would also need to be logged but
the same class should be useful, perhaps with some extension. Image data would be more
complicated, while GPS/acceleration data should be straightforward to log.

Adam Greig

March 2009
*/
