#include "pololu_serial.h"

/**
* Open the serial port handler and do basic error checking.
* The serial port is opened, currently at 9600 baud (hardcoded).
* In both Windows and Linux the port is setup for 9600 baud with
* 8 data bits, no stop bit, no parity bit and no flow control.
*/
PololuSerial::PololuSerial() {
    #if WINDOWS
    //Open the serial port
	LPCSTR portname = "COM8";
    serial_port = CreateFile(portname, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    //Check it opened successfully
    if( serial_port == INVALID_HANDLE_VALUE ) {
        serial_port = NULL;
		DWORD err = GetLastError();
		printf("Error opening serial port. Error %i.\n", err);
        return;
    }

    //Set timeouts (mostly, don't time out)
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadTotalTimeoutConstant    = 100;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = 0;
	if(SetCommTimeouts(serial_port, &timeouts) == FALSE) {
		printf("Error setting timeouts on serial port.\n");
        return;
	}

    //Set port options
    DCB dcb;
	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);
	BuildCommDCB((LPCSTR)"baud=9600 parity=N data=8 stop=1", &dcb);

    DWORD result = SetCommState(serial_port, &dcb);
	if( !result ) {
		printf("Error %i setting comm state.\n", GetLastError());
	}
    return;

    #elif LINUX

    //Open the serial port
    serial_port = open(POLOLUSERIAL_PORT, O_WRONLY | O_NOCTTY | O_NDELAY);
    printf("Serial port opened: %i\n", serial_port);
    if( serial_port == -1 ) {
        serial_port = NULL;
        return;
    }

    int serial_port_set = fcntl(serial_port, F_SETFL, 0);
    printf("Serial port set: %i\n", serial_port_set);

    //Yuck! Linux makes this a complete mess
    struct termios port_settings;
    cfsetispeed(&port_settings, B9600);
    cfsetospeed(&port_settings, B9600);
    port_settings.c_cflag &= ~PARENB;
    port_settings.c_cflag &= ~CSTOPB;
    port_settings.c_cflag &= ~CSIZE;
    port_settings.c_cflag |= CS8;
    port_settings.c_cflag &= ~CRTSCTS;
    port_settings.c_cflag |= CREAD | CLOCAL;
    port_settings.c_iflag &= ~( IXON | IXOFF | IXANY );
    port_settings.c_lflag &= ~( ICANON | ECHO | ECHOE | ISIG );
    port_settings.c_oflag &= ~OPOST;
    port_settings.c_cc[VMIN] = 0;
    port_settings.c_cc[VTIME] = 20;

    serial_port_set = tcsetattr(serial_port, TCSANOW, &port_settings);
    printf("Configured serial port: %i\n", serial_port_set);

    #endif
}

/**
* Close the serial port handler.
*/
PololuSerial::~PololuSerial() {
    #if WINDOWS
    //When we're all finished, clear up by closing the serial port
    CloseHandle(serial_port);
    #elif LINUX
    close(serial_port);
    #endif
}

/**
* Send data over the open serial port.
* \param data A char array of data to send. Not null-terminated.
* \param size The number of bytes to send from data.
*/
void PololuSerial::send_data(char* data, unsigned short int size) {
	if( !serial_port ) {
		printf("Error: Serial port not open.\n");
		return;
	}
    #if WINDOWS
	DWORD written;
    WriteFile(serial_port, data, (DWORD)size, &written, NULL);
    return;
    #elif LINUX
	if(write(serial_port, data, size) == -1) {
		printf("Error writing to serial port.\n");
		return;
	}
	return;
    #endif

    return;
}
