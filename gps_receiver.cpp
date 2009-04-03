#include "gps_receiver.h"

#if LINUX
//Linux doesn't have the _s functions so fake them when compiling for Linux.
char * strtok_s( char* str, const char* deliminaters, char** context ) {
	return strtok( str, deliminaters );
}
char * strncpy_s( char* dest, size_t size, const char* source, size_t num ) {
	return strncpy( dest, source, num );
}
#endif

/**
* Initialise the serial port used to talk to the GPS unit.
*/
GPSReceiver::GPSReceiver(void) {
	serial = new SerialPort(GPS_PORT, 4800);
	memset(&time, sizeof(time), 0x00);
	memset(&pos, sizeof(pos), 0x00);
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
		char field[128];
		unsigned int buffer_index = 0;
		
		printf("Sentence: %s\n", buffer);

		//Get the sentence type
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		printf("Sentence type: %s\n", field);
		
		//Get the time
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		printf("Sentence time: %s\n", field);
		if( strlen(field) > 0 ) {
			time.hours = 10*((int)(field[0]) - 48) + ((int)field[1] - 48);
			time.minutes = 10*((int)(field[2]) - 48) + ((int)field[3] - 48);
			time.seconds = 10*((int)(field[4]) - 48) + ((int)field[5] - 48);
			time.milliseconds = 100*((int)field[7] - 48) + 10*((int)field[8] - 48);
			time.milliseconds += ((int)field[9] - 48);
		}
		printf("Time: %.2i:%.2i:%.2i.%.3i UTC\n", time.hours, time.minutes, time.seconds, time.milliseconds);

		//Get the lock status
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		if( strlen(field) > 0 && field[0] == 'A' ) {
			lock = true;
			printf("GPS position lock obtained.\n");
		} else {
			//If there is no lock, there's no point parsing anything else.
			//The only other field available is the date, everything else
			// is empty.
			lock = false;
			printf("No position lock.\n");
			return;
		}

		//Get the latitude
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		if( strlen(field) > 0 ) {
			pos.lat_degrees = 10*(field[0] - 48) + (field[1] - 48);
			char minutes[16];
			strncpy_s(minutes, &field[2], sizeof(field) - 3);
			pos.lat_minutes = atof(minutes);
		}

		//Get latitude direction
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		if( strlen(field) > 0 ) {
			pos.lat_direction = field[0];
		}

		printf("Lat: %i degrees %d minutes %s\n", pos.lat_degrees, pos.lat_minutes, pos.lat_direction);

		//Get the longitude
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		if( strlen(field) > 0 ) {
			pos.lon_degrees = 10*(field[0] - 48) + (field[1] - 48);
			char minutes[16];
			strncpy_s(minutes, &field[2], sizeof(field) - 3);
			pos.lon_minutes = atof(minutes);
		}

		//Get latitude longitude
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		if( strlen(field) > 0 ) {
			pos.lon_direction = field[0];
		}

		printf("Lon: %i degrees %d minutes %s\n", pos.lat_degrees, pos.lat_minutes, pos.lat_direction);

		//Get groundspeed in knots
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		if( strlen(field) > 0 ) {
			speed_knots = atof(field);
		}

		printf("Speed:\n\t%d knots\n\t%d m/s\n\t%d mph\n", get_speed_knots(), get_speed_ms(), get_speed_mph());

		//Get track angle
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		if( strlen(field) > 0 ) {
			speed_knots = atof(field);
		}

		//Get the date
		buffer_index = parse_until_comma(buffer, field, buffer_index);
		if( strlen(field) > 0 ) {
			time.day = 10*(field[0] - 48) + (field[1] - 48);
			time.month = 10*(field[2] - 48) + (field[3] - 48);
			time.year = 10*(field[4] - 48) + (field[5] - 48);
		}
		printf("Date: %i/%i/%i\n", time.day, time.month, time.year);

		//We could get the magnetic field variation but there's not much
		// point. Instead, we stop here. If you did want this data, just
		// repeat the same procedure as for everything else. Note that
		// the final field, the direction of the variation, is terminated
		// with a * and not a ,.
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