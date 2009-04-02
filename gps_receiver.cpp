#include "gps_receiver.h"

#if LINUX
char * strtok_s( char* str, const char* deliminaters, char** context ) {
	return strtok( str, deliminaters );
}
#endif

GPSReceiver::GPSReceiver(void) {
	serial = new SerialPort(GPS_PORT, 4800);
}

void GPSReceiver::update() {
	char buffer[128];
	unsigned int i, buffer_length;
	//Process each available line on the buffer
	while( serial->read_line(buffer, 128) != 0 ) {
		buffer_length = (unsigned int)strlen(buffer);

		//The first character should always be a $
		if( buffer[0] != '$' )
			continue;
		
		//All valid GPS related sentences start GP
		if( buffer[1] != 'G' || buffer[2] != 'P' )
			continue;
		
		//We are only interested in the RMC sentences
		if( buffer[3] != 'R' || buffer[4] != 'M' || buffer[5] != 'C' )
			continue;

		//After the message type there should be a ','
		if( buffer[6] != ',' )
			continue;

		//If everything so far is good, check the checksum
		//Find the original checksum
		char msg_checksum[2];
		msg_checksum[0] = buffer[buffer_length - 4];
		msg_checksum[1] = buffer[buffer_length - 3];
		//Calculate the message checksum (XOR all characters)
		char calc_checksum = buffer[1];
		for( i=2; i<(buffer_length - 5); i++ ) {
			calc_checksum ^= buffer[i];
		}
		//Store the new checksum as two hex characters
		char calc_checksum_str[3];
		#if WINDOWS
		sprintf_s(calc_checksum_str, 3, "%.2X", calc_checksum);
		#elif LINUX
		sprintf(calc_checksum_str, "%.2X", calc_checksum);
		#endif
		//Compare the two
		if( strncmp( msg_checksum, calc_checksum_str, 2 ) != 0 ) {
			printf("Checksum incorrect, skipping line.\n");
			continue;
		}
		
		//Store the fields as they are parsed
		char *field, *next_field;
		char delims[] = ",*";

		//Get the sentence type
		field = strtok_s( buffer, ",", &next_field );
		
		//Get the time
		field = strtok_s( NULL, ",", &next_field );
		if( strlen(field) > 0 ) {
			time.hours = 10*((int)(field[0]) - 48) + ((int)field[1] - 48);
			time.minutes = 10*((int)(field[2]) - 48) + ((int)field[3] - 48);
			time.seconds = 10*((int)(field[4]) - 48) + ((int)field[5] - 48);
			time.milliseconds = 100*((int)field[7] - 48) + 10*((int)field[8] - 48);
			time.milliseconds += ((int)field[9] - 48);

			printf("Time: %.2i:%.2i:%.2i.%.3i UTC\n", time.hours, time.minutes, time.seconds, time.milliseconds);
		}

		//Get the lock status
		field = strtok_s( NULL, ",", &next_field );

		//Get the latitude
		field = strtok_s( NULL, ",", &next_field );

		//Get latitude direction
		field = strtok_s( NULL, ",", &next_field );

		//Get the longitude
		field = strtok_s( NULL, ",", &next_field );

		//Get longitude direction
		field = strtok_s( NULL, ",", &next_field );

		//Get groundspeed in knots
		field = strtok_s( NULL, ",", &next_field );

		//Get track angle
		field = strtok_s( NULL, ",", &next_field );

		//Get the date
		field = strtok_s( NULL, ",", &next_field );

		//Get the magnetic variation
		field = strtok_s( NULL, ",", &next_field );

		//Get magnetic variation direction
		field = strtok_s( NULL, ",", &next_field );
		
	}
}

GPStime GPSReceiver::get_time() {
	return time;
}

GPSpos GPSReceiver::get_pos() {
	return pos;
}

bool GPSReceiver::has_lock() {
	return lock;
}

double GPSReceiver::get_speed_knots() {
	return speed_knots;
}

double GPSReceiver::get_speed_ms() {
	return speed_knots * 0.51444;
}

double GPSReceiver::get_speed_mph() {
	return speed_knots * 1.15078;
}

double GPSReceiver::get_track_angle() {
	return track_angle;
}