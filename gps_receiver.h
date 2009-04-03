#pragma once

#include "stdafx.h"
#include "serial_port.h"
#include "ports.h"

struct GPStime {
	unsigned short int year;
	unsigned short int month;
	unsigned short int day;
	unsigned short int hours;
	unsigned short int minutes;
	unsigned short int seconds;
	unsigned short int milliseconds;
};

struct GPSpos {
	unsigned short int lat_degrees;
	double lat_minutes;
	char lat_direction;
	unsigned short int lon_degrees;
	double lon_minutes;
	char lon_direction;
};

class GPSReceiver {
public:
	GPSReceiver(void);
	void update();
	
	GPStime get_time();
	GPSpos get_pos();
	bool has_lock();
	double get_speed_knots();
	double get_speed_ms();
	double get_speed_mph();
	double get_track_angle();
private:
	unsigned int parse_until_comma(char* input, char* output, unsigned int position);
	
	SerialPort* serial;
	GPStime time;
	GPSpos pos;
	bool lock;
	double speed_knots;
	double track_angle;

};
