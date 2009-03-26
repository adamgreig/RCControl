/**************************************
 * PololuSerial
 * Open and send data over a serial port
 **************************************/

#pragma once

#define LINUX

#ifdef WINDOWS
#include <windows.h>
#define POLOLUSERIAL_PORT "COM1"
#elifdef LINUX
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#define POLOLUSERIAL_PORT "/dev/ttyS0"
#endif

#include <stdio.h>

#define POLOLUSERIAL_PORT "COM1"

///Interface to a serial port. Re-implement for Linux when required.
class PololuSerial {
    public:
        PololuSerial();
        ~PololuSerial();
        void send_data(char* data, unsigned short int size);
    private:
        #ifdef WINDOWS
        HANDLE serial_port;
        #elifdef LINUX
        int serial_port;
        #endif
};
