#pragma once
#include "stdafx.h"

/**
* Interfaces to a serial port. Holds a file descriptor/handler.
* Constructed with a port name, e.g. COM8 on Windows, or
* /dev/ttyUSB0 on Linux. Provides read and write functionality.
*/
class SerialPort {
    public:
        SerialPort(char* port, int baud);
        ~SerialPort();
        int send_data(char* data, unsigned int size);
		int read_line(char* buffer, unsigned int size);
    private:
        #if WINDOWS
        HANDLE serial_port;
        #elif LINUX
        int serial_port;
        #endif
};
