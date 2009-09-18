/**
* \file Code entry point and main application logic.
*/

#include <vector>
#include <ctime>

#include "stdafx.h"
#include "pololu_servo_controller.h"
#include "mftech_receiver.h"
#include "wiimote_thread.h"
#include "gps_receiver.h"
#include "camera.h"
#include "gui.h"

int _tmain(int argc, _TCHAR* argv[]) {
	cout << "Opening servo controller serial port... ";
	PololuServoController servos = PololuServoController();
	cout << " Done" << endl;
	cout << "Opening MFTech Receiver interface... ";
	MFTechReceiver mftech = MFTechReceiver();
	cout << " Done" << endl;
	cout << "Starting up Wiimote thread... ";
	Wiimote remote = Wiimote();
	cout << " Done" << endl;
	cout << "Starting up GPS thread... ";
	GPSReceiver gps = GPSReceiver();
	cout << " Done" << endl;
	cout << "Starting camera reading thread... ";
	Camera camera = Camera();
	cout << " Done" << endl;
	cout << "Starting GUI... ";
	GUI gui = GUI();
	cout << "Done" << endl;

	for(;;) {
		unsigned int the_time = clock();
		cout << "Time " << the_time << endl;
		if( remote.connected() ) {
			gui.set_wiimote(true);
			cout.precision(3);
			cout.setf(ios::fixed,ios::floatfield);
			cout << "X  " << remote.accel_X() << "\tY  " << remote.accel_Y() << "\tZ  " << remote.accel_Z() << endl;
		} else {
			cout << "No wiimote connected" << endl;
		}
		if( gps.has_lock() ) {
			gui.set_gps(true);
			cout.precision(5);
			cout.setf(ios::fixed,ios::floatfield);
			GPSpos pos = gps.get_pos();
			GPStime time = gps.get_time();
			cout << "Lat " << pos.lat_degrees << "° " << pos.lat_minutes << "' " << pos.lat_direction << endl;
			cout << "Lon " << pos.lon_degrees << "° " << pos.lon_minutes << "' " << pos.lon_direction << endl;
			cout << gps.get_speed_mph() << "mph" << endl;
			cout << time.year << "-" << time.month << "-" << time.day << " " << time.hours << ":" << time.minutes << ":" << time.seconds << "." << time.milliseconds << endl;
		} else {
			cout << endl << endl << "No GPS lock" << endl << endl;
		}
		if( mftech.joystick_available() ) {
			gui.set_mftech(true);
			cout.precision(3);
			cout.setf(ios::fixed,ios::floatfield);
			cout << "Throttle " << mftech.throttle() << endl;
			cout << "Steering " << mftech.steering() << endl;
			cout << "Mode Select " << mftech.modeselect() << endl;
		} else {
			cout << endl << "No controller found" << endl << endl;
		}
		if( servos.connected() ) {
			gui.set_servo(true);
			if(mftech.joystick_available()) {
				double throttle = mftech.throttle();
				double steering = mftech.steering();
				throttle /= 65535.0;
				steering /= 65535.0;
				throttle *= 5000.0;
				steering *= 5000.0;
				throttle += 500.0;
				steering += 500.0;
				servos.set_position_abs(0, (unsigned short)steering);
				servos.set_position_abs(1, (unsigned short)throttle);
				cout << "Sent new servo positions to controller" << endl;
			}
		} else {
			cout << "Pololu servo controller not connected" << endl;
		}
		if( camera.connected() ) {
			gui.set_camera(true);
		} else {
			cout << "No camera found" << endl;
		}
		
		cout << endl;
		
		if( gui.close ) {
			camera.release();
			Sleep(100);
			exit(0);
		}

		Sleep(30);
	}
}
