#pragma once
#include "stdafx.h"
#include "wiimote.h"

/**
* Store all wiimote acceleration values in one place
*/
struct wiimote_data_struct {
	float accel_X;
	float accel_Y;
	float accel_Z;
	float pitch;
	float roll;
};

/**
* Runs a thread that constantly polls the wiimote for
* acceleration data and makes this data available.
*/
class Wiimote {
    public:
        Wiimote();
		~Wiimote();
        
		float accel_X();
		float accel_Y();
		float accel_Z();
		float pitch();
		float roll();
		bool connected();
		void disconnect();
		wiimote_data_struct wiimote_data();

	private:
		uintptr_t _thread;
		HANDLE _updateMutex;
		float _accel_X;
		float _accel_Y;
		float _accel_Z;
		float _pitch;
		float _roll;
		bool _connected;

	friend void wiimote_thread(void*);
};

void wiimote_thread(void* Wiimote);