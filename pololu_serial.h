/**************************************
 * PololuSerial
 * Open and send data over a serial port
 **************************************/

#pragma once

#if WINDOWS
#include <windows.h>
#define POLOLUSERIAL_PORT "COM8"
#elif LINUX
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#define POLOLUSERIAL_PORT "/dev/ttyUSB0"
#endif

#include <stdio.h>

///Interface to a serial port. Re-implement for Linux when required.
class PololuSerial {
    public:
        PololuSerial();
        ~PololuSerial();
        void send_data(char* data, unsigned short int size);
    private:
        #if WINDOWS
        HANDLE serial_port;
        #elif LINUX
        int serial_port;
        #endif
};
