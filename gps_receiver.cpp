#include "gps_receiver.h"

/**
* Initialise the serial port used to talk to the GPS unit.
*/
GPSReceiver::GPSReceiver(void) {
	serial = new SerialPort(GPS_PORT, 9600);
	_connected = serial->connected();

	memset(&time, sizeof(time), 0x00);
	memset(&pos, sizeof(pos), 0x00);
	
	lock = false;

	_updateMutex = CreateMutex(NULL, FALSE, NULL);
	this->_thread = _beginthread(gps_thread, 0, this);

	if( !this->_thread ) {
		cout << "Error starting GPS thread. Exiting." << endl;
		exit(1);
	}
}

GPSReceiver::~GPSReceiver(void) {
}

bool GPSReceiver::connected() {
	return _connected;
}

/**
* Read input until a comma occurs, storing each characater into output.
* Sets output to be a null terminated string of everything up to the comma.
* Returns the new position for next time the function is called.
* \param input A char* pointing to a null terminated string
* \param output A char* pointing to a buffer that must be as big as input
* \param position The previous position in the string, i.e. the starting point
*/
unsigned int GPSReceiver::parse_until_comma(char* input, char* output, unsigned int position) {
	unsigned int output_index = 0;
	unsigned int size = (unsigned int)strlen(input);
	
	output[output_index] = 0x00;

	while( position < size ) {
		if( input[position] != ',' ) {
			output[output_index++] = input[position];
			output[output_index] = 0x00;
		} else {
			return position + 1;
		}
		position++;
	}

	return position;

}

/**
* Check the serial port handler for new lines and parse any GGRMC sentences.
* This function updates the class's internal state.
* You should always call update() before accessing class members.
*/
void GPSReceiver::update() {
	char buffer[128];
	unsigned int i, buffer_length;
	//Process the next line in the buffer
    serial->read_line(buffer, 128);
	buffer_length = (unsigned int)strlen(buffer);
    
    fflush(NULL);
	
    //The first character should always be a $
	if( buffer[0] != '$' )
		return;
	
	//All valid GPS related sentences start GP
	if( buffer[1] != 'G' || buffer[2] != 'P' )
		return;
    
	//We are only interested in the RMC sentences
	if( buffer[3] != 'R' || buffer[4] != 'M' || buffer[5] != 'C' )
		return;
    
	//After the message type there should be a ','
	if( buffer[6] != ',' )
		return;
    
	//If everything so far is good, check the checksum
	//Find the original checksum
	char msg_checksum[2];
	msg_checksum[0] = buffer[buffer_length - 2];
	msg_checksum[1] = buffer[buffer_length - 1];
    
	//Calculate the message checksum (XOR all characters)
	char calc_checksum = buffer[1];
    
	for( i=2; i<(buffer_length - 3); i++ ) {
		calc_checksum ^= buffer[i];
	}
    
	//Store the new checksum as two hex characters
	char calc_checksum_str[3];
	sprintf_s(calc_checksum_str, 3, "%.2X", calc_checksum);
	//Compare the two
	if( strncmp( msg_checksum, calc_checksum_str, 2 ) != 0 ) {
		return;
	}
	// Now acquire the mutex lock as we are going to go through with the update
	WaitForSingleObject(_updateMutex, INFINITE);
	//Store the fields as they are parsed
	char field[128];
	unsigned int buffer_index = 0;

	//Get the sentence type
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	
	//Get the time
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	if( strlen(field) > 0 ) {
		time.hours = 10*((int)(field[0]) - 48) + ((int)field[1] - 48);
		time.minutes = 10*((int)(field[2]) - 48) + ((int)field[3] - 48);
		time.seconds = 10*((int)(field[4]) - 48) + ((int)field[5] - 48);
		time.milliseconds = 100*((int)field[7] - 48) + 10*((int)field[8] - 48);
		time.milliseconds += ((int)field[9] - 48);
	}
	//printf("Time: %.2i:%.2i:%.2i.%.3i UTC\n", time.hours, time.minutes, time.seconds, time.milliseconds);

	//Get the lock status
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	if( strlen(field) > 0 && field[0] == 'A' ) {
		lock = true;
		//cout << "GPS position lock obtained." << endl;
	} else {
		//If there is no lock, there's no point parsing anything else.
		//The only other field available is the date, everything else
		// is empty.
		lock = false;
		//cout << "No GPS position lock." << endl;
		ReleaseMutex(_updateMutex);
		return;
	}

	//Get the latitude
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	if( strlen(field) > 0 ) {
		char minutes[16];
		strncpy_s(minutes, 16, &field[2], strlen(field) - 3);
		pos.lat_minutes = atof(minutes);
        pos.lat_degrees = 10*(field[0] - 48) + (field[1] - 48);
	}

	//Get latitude direction
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	if( strlen(field) > 0 ) {
		pos.lat_direction = field[0];
	}

	//printf("Lat: %i degrees %f minutes %c\n", pos.lat_degrees, pos.lat_minutes, pos.lat_direction);

	//Get the longitude
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	if( strlen(field) > 0 ) {
		char minutes[16];
		strncpy_s(minutes, 16, &field[3], strlen(field) - 4);
		pos.lon_minutes = atof(minutes);
        pos.lon_degrees = 100*(field[0] - 48);
        pos.lon_degrees += 10*(field[1] - 48);
        pos.lon_degrees += (field[1] - 48);
	}

	//Get latitude direction
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	if( strlen(field) > 0 ) {
		pos.lon_direction = field[0];
	}

	//printf("Lon: %d degrees %f minutes %c\n", pos.lon_degrees, pos.lon_minutes, pos.lon_direction);

	//Get groundspeed in knots
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	if( strlen(field) > 0 ) {
		speed_knots = atof(field);
	}

	//printf("Speed:\n\t%f knots\n\t%f m/s\n\t%f mph\n", get_speed_knots(), get_speed_ms(), get_speed_mph());

	//Get track angle
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	if( strlen(field) > 0 ) {
		track_angle = atof(field);
	}
    //printf("Heading: %f degrees\n", track_angle);

	//Get the date
	buffer_index = parse_until_comma(buffer, field, buffer_index);
	if( strlen(field) > 0 ) {
		time.day = 10*(field[0] - 48) + (field[1] - 48);
		time.month = 10*(field[2] - 48) + (field[3] - 48);
		time.year = 10*(field[4] - 48) + (field[5] - 48);
	}
	//printf("Date: %.2d/%.2d/%.2d\n", time.day, time.month, time.year);

	//We could get the magnetic field variation but there's not much
	// point. Instead, we stop here. If you did want this data, just
	// repeat the same procedure as for everything else. Note that
	// the final field, the direction of the variation, is terminated
	// with a * and not a ,.

	ReleaseMutex(_updateMutex);
	printf("update() released mutex\r\n");

}

///Return the GPStime struct
GPStime GPSReceiver::get_time() {
	WaitForSingleObject(_updateMutex, INFINITE);
	GPStime result = time;
	ReleaseMutex(_updateMutex);
	return result;
}

///Return the GPSpos struct
GPSpos GPSReceiver::get_pos() {
	WaitForSingleObject(_updateMutex, INFINITE);
	GPSpos result = pos;
	ReleaseMutex(_updateMutex);
	return result;
}

///Return the lock status
bool GPSReceiver::has_lock() {
	WaitForSingleObject(_updateMutex, INFINITE);
	bool result = lock;
	ReleaseMutex(_updateMutex);
	return result;
}

///Return current speed in knots
double GPSReceiver::get_speed_knots() {
	WaitForSingleObject(_updateMutex, INFINITE);
	double result = speed_knots;
	ReleaseMutex(_updateMutex);
	return result;
}

///Return current speed in metres per sec
double GPSReceiver::get_speed_ms() {
	WaitForSingleObject(_updateMutex, INFINITE);
	double result = speed_knots * 0.51444;
	ReleaseMutex(_updateMutex);
	return result;
}

///Return current speed in miles per hour
double GPSReceiver::get_speed_mph() {
	WaitForSingleObject(_updateMutex, INFINITE);
	double result = speed_knots * 1.15078;
	ReleaseMutex(_updateMutex);
	return result;
}

///Return current heading
double GPSReceiver::get_track_angle() {
	WaitForSingleObject(_updateMutex, INFINITE);
	double result = track_angle;
	ReleaseMutex(_updateMutex);
	return result;
}

///Constantly read the serial port to check for updates
void gps_thread(void* void_gps_receiver) {
	GPSReceiver* gps_receiver = (GPSReceiver*) void_gps_receiver;
	for(;;) {
		gps_receiver->update();
		Sleep(5);
	}
}