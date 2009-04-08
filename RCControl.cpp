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
#include "mftech_receiver.h"

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
	PololuServoController servos = PololuServoController();
	MFTechReceiver receiver = MFTechReceiver();
	sleep(2000);
	cout << "Active." << endl;
	
	//Store a vector list of time and the control state
	vector<TimeControl> time_controls;
	//Store the start time, to calculate offsets
	unsigned long int starttime = 0;

	for(;;) {
		
		//Modeselect is the right hand up/down stick
		//If it's not fully up, passthrough and store control
		int modeselect = receiver.modeselect();
		if( modeselect < 50000 ) {
			//Get the current throttle and steering position
			double throttle = receiver.throttle();
			double steering = receiver.steering();
			
			//Change from "0-65535" to "500-5500"
			throttle /= 65535.0;
			steering /= 65535.0;
			throttle *= 5000.0;
			steering *= 5000.0;
			throttle += 500.0;
			steering += 500.0;
			
			//Start the clock if it's not already
			if( starttime == 0 ) {
				starttime = get_timestamp();
			}
			
			//A new struct to hold the time and controls
			TimeControl tc;
			tc.offset = get_timestamp() - starttime;
			tc.throttle = (unsigned int)throttle;
			tc.steering = (unsigned int)steering;

			//Pass through current controls
			servos.set_position_abs(0, tc.steering);
			servos.set_position_abs(1, tc.throttle);

			//Store current controls
			time_controls.push_back(tc);

			printf("STORE throttle %.5d\tsteering %.5d\r\n", (int)throttle, (int)steering);
			sleep(50);


		} else {
			//Replay previous stored sequence

			//Iterator for the list of TimeControls
			vector<TimeControl>::iterator tc_it;
			
			//Wait a second before starting
			sleep(1000);
			
			//Take the current time, for offsets
			starttime = get_timestamp();
			
			//Run through the control list
			for( tc_it = time_controls.begin(); tc_it < time_controls.end(); tc_it++ ) {
				//If the mode switch changes position, stop quickly
				if( receiver.modeselect() < 50000 ) break;
				
				//Take the current struct
				TimeControl current_tc = *tc_it;

				//Wait for it to be the right time
				while( get_timestamp() - starttime < current_tc.offset );

				//Set the servos
				servos.set_position_abs(0, current_tc.steering);
				servos.set_position_abs(1, current_tc.throttle);

				printf("SEND throttle %.5d\tsteering %.5d\r\n", current_tc.throttle, current_tc.steering);
			}

			starttime = 0;
			time_controls.clear();
		}

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

Adam Greig

April 2009
*/
