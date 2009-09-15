/**
* \file Code entry point and main application logic.
*/

#include <vector>

#if WINDOWS
//Get the number of system ticks since execution started.
//This is a consistent way to time the replay, on windows
#include <ctime>
unsigned long int get_timestamp() {
	return (unsigned long int)clock();
}
#elif LINUX
//Linux does clock() completely differently and uselessly.
//Instead, gettimeofday, take away a lot from the seconds
// so they still fit in a long int, multiply by 1k to make
// space for milliseconds, divide microseconds by 1k to
// make them into milliseconds, then add the two and return
#include <sys/time.h>
unsigned long int get_timestamp() {
	struct timeval thetime;
	gettimeofday(&thetime, NULL);
	thetime.tv_sec -= 1234567890;
	thetime.tv_sec *= 1000;
	thetime.tv_usec /= 1000;
	return (unsigned long int)(thetime.tv_sec + thetime.tv_usec);
}
#endif

#include "stdafx.h"
#include "pololu_servo_controller.h"
//#include "mftech_receiver.h"
#include "wiimote.h"

/**
* TimeControl stores some form of timestamp offset and the
* position of the throttle and steering at that time.
*/
struct TimeControl {
	unsigned long int offset;
	unsigned int throttle;
	unsigned int steering;
};

//Windows apparently prefers the top form, while
// Linux doesn't really do _TCHAR or _tmain.
#if WINDOWS
int _tmain(int argc, _TCHAR* argv[]) {
#elif LINUX
int main(int argc, char* argv[]) {
#endif
	
	//Create a new servo controller and rc receiver
	cout << "Initialising..." << endl;
	wiimote remote;
	PololuServoController servos = PololuServoController();
	remote.Connect(wiimote::FIRST_AVAILABLE);
	remote.SetLEDs(0x0f);
	remote.SetReportType(wiimote::IN_BUTTONS_ACCEL);
	cout << "Active." << endl;

	for(;;) {
			
			remote.RefreshState();
			int roll, pitch;

			if( remote.Button.A() ) {

				roll = remote.Acceleration.Orientation.Roll;
				// roll: -90 to 90
				roll = -roll; // roll: 90 to -90
				roll += 90;   // roll: 180 to 0
				roll *= 28;   // roll: 5040 to 0
				roll += 500;  // roll: 5540 to 500
				if( roll > 5500 ) roll = 5500;
				if( roll < 500 ) roll = 500;

				pitch = remote.Acceleration.Orientation.Pitch;
				// pitch: -90 to 0
				pitch = -pitch; // pitch: 90 to 0
				pitch *= 20;   // pitch: 1800 to 0
				pitch += 500;  // pitch: 2300 to 500
				if( pitch > 2300 ) pitch = 2300;
				if( pitch < 500 ) pitch = 500;

			} else {
				roll = 500;
				pitch = 500;
			}

			//Pass through current controls
			servos.set_position_abs(0, roll);
			servos.set_position_abs(1, pitch);

			printf("throttle %.5d\tsteering %.5d\r\n", pitch, roll);
			sleep(50);

	}
	

}

/**
\mainpage RCControl Documentation

At the time of writing, the code consists of five classes.

\section serial Serial Interface
The SerialPort class connects to a serial port given in the constructor and allows
reading or writing to it. This is used by the Pololu controller and the GPS receiver.

\section pololu Pololu Controller
Pololu Servo Controller: http://www.pololu.com/catalog/product/390

This device simulates a serial port to the computer over USB, and has a straightforward
protocol defined in http://www.pololu.com/file/0J35/usc01a_guide.pdf

It controls up to 16 servos connected directly to it, and requires an external power supply.

PololuServoController defines the Pololu API. Each method takes a servo number and
the data to send. The class also instances SerialPort for actually sending data.

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

\section gps GPSReceiver
The GPSReceiver class reads in lines from the GPS over the serial port, then parses
them as NMEA strings to find GPRMC sentences which it extracts position and time data
from, making them available as class members.

\section logger Logger
The final class, FileLogger, logs this data to a file. This is to be used with other sensors for
recording how a human driver controlled the car, with the aim of having the onboard
computer eventually do the driving itself. Other data would also need to be logged but
the same class should be useful, perhaps with some extension. Image data would be more
complicated, while GPS/acceleration data should be straightforward to log.

\section todo To-Do
The main thing that needs to be improved at the moment is the GPS receiver class, which
currently hangs waiting for a full line at the serial port. This significantly slows down
program execution. A non-blocking read on the serial port, or using Window's OVERLAPPING
read functions to generate events may help resolve this issue.

Side note: Window's OVERLAPPING file reading is horrible! You may be better off just using
a thread to constantly read the GPS port synchronously and have the main thread check that.

Adam Greig

April 2009
*/
