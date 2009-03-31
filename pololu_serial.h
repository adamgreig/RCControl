#pragma once
#include "stdafx.h"

//Edit these values as required.
#if WINDOWS
#define POLOLUSERIAL_PORT "COM8"
#elif LINUX
#define POLOLUSERIAL_PORT "/dev/ttyUSB0"
#endif

/**
* Interfaces to a serial port. Holds a file descriptor/handler.
*/
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
