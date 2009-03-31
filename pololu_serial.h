#pragma once
#include "stdafx.h"

#if WINDOWS
#define POLOLUSERIAL_PORT "COM8"
#elif LINUX
#define POLOLUSERIAL_PORT "/dev/ttyUSB0"
#endif

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
