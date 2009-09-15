#include "wiimote_thread.h"

/**
* Initialise the class.
*/
Wiimote::Wiimote() {
	this->_updateMutex = CreateMutex(NULL, FALSE, NULL);
	this->_thread = _beginthread(wiimote_thread, 0, this);
	
	_connected = false;

	if( !this->_thread ) {
		cout << "Error starting Wiimote thread. Exiting." << endl;
		exit(1);
	}
}

/**
* Destroy the class.
*/
Wiimote::~Wiimote() {
}

/**
* The main thread
*/
void wiimote_thread(void* void_wiimote) {
	Wiimote * remote_t = (Wiimote*) void_wiimote;
	wiimote remote;
	while(!remote.Connect(wiimote::FIRST_AVAILABLE));
	remote_t->_connected = true;
	remote.SetLEDs(0x0F);
	remote.SetReportType(wiimote::IN_BUTTONS_ACCEL);

	do {

		while( remote.RefreshState() == NO_CHANGE ) {
			// If the remote state did not change, no need to do anything
			Sleep(1);
		}

		WaitForSingleObject(remote_t->_updateMutex, INFINITE);

		remote_t->_accel_X = remote.Acceleration.X;
		remote_t->_accel_Y = remote.Acceleration.Y;
		remote_t->_accel_Z = remote.Acceleration.Z;
		remote_t->_pitch = remote.Acceleration.Orientation.Pitch;
		remote_t->_roll = remote.Acceleration.Orientation.Roll;

		ReleaseMutex(remote_t->_updateMutex);

	} while(remote.IsConnected());

}

float Wiimote::accel_X() {
	WaitForSingleObject(this->_updateMutex, INFINITE);
	float result = this->_accel_X;
	ReleaseMutex(this->_updateMutex);
	return result;
}

float Wiimote::accel_Y() {
	WaitForSingleObject(this->_updateMutex, INFINITE);
	float result = this->_accel_Y;
	ReleaseMutex(this->_updateMutex);
	return result;
}

float Wiimote::accel_Z() {
	WaitForSingleObject(this->_updateMutex, INFINITE);
	float result = this->_accel_Z;
	ReleaseMutex(this->_updateMutex);
	return result;
}

float Wiimote::pitch() {
	WaitForSingleObject(this->_updateMutex, INFINITE);
	float result = this->_pitch;
	ReleaseMutex(this->_updateMutex);
	return result;
}

float Wiimote::roll() {
	WaitForSingleObject(this->_updateMutex, INFINITE);
	float result = this->_roll;
	ReleaseMutex(this->_updateMutex);
	return result;
}

bool Wiimote::connected() {
	// no need to acquire the mutex because this variable does
	// not change after connection
	return _connected;
}

wiimote_data_struct Wiimote::wiimote_data() {
	WaitForSingleObject(this->_updateMutex, INFINITE);
	wiimote_data_struct data;
	data.accel_X = this->_accel_X;
	data.accel_Y = this->_accel_Y;
	data.accel_Z = this->_accel_Z;
	data.pitch = this->_pitch;
	data.roll = this->_roll;
	ReleaseMutex(this->_updateMutex);
	return data;
}
