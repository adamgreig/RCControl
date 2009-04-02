/**
* \file Defines the serial ports used for the various serial port items in the project.
*/

#if WINDOWS
#define POLOLU_PORT "COM8"
#define GPS_PORT "COM10"
#elif LINUX
#define POLOLU_PORT "/dev/ttyUSB0"
#define GPS_PORT "/dev/ttyUSB1"
#endif