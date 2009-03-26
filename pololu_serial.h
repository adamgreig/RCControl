/**************************************
 * PololuSerial
 * Open and send data over a serial port
 **************************************/

#pragma once

#ifdef WINDOWS
#include <windows.h>
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
        #endif
};
