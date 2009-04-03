/**
* \file Defines the serial ports used for the various serial port items in the project.
*/
#pragma once

#if WINDOWS
#define POLOLU_PORT "COM8"
#define GPS_PORT "COM3"
#elif LINUX
#define POLOLU_PORT "/dev/ttyUSB1"
#define GPS_PORT "/dev/ttyUSB0"
#endif
