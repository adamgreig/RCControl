#include "serial_port.h"

/**
* Open the serial port and check it succeeded.
* The named port is opened at the given baud rate using 8N1
* with no hardware flow control.
* \param port The serial port to use, e.g. COM8 or /dev/ttyUSB0
* \param baud The baud rate to use, e.g. 9600 or 4800
*/
SerialPort::SerialPort(char* port, int baud) {
    #if WINDOWS
    //Open the serial port
    serial_port = CreateFile((LPCSTR)port, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    //Check it opened successfully
    if( serial_port == INVALID_HANDLE_VALUE ) {
		DWORD err = GetLastError();
		printf("Error opening serial port. Error %i.\n", err);
		exit(err);
    }

    //Set timeouts (mostly, don't time out)
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadTotalTimeoutConstant    = 100;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = 0;
	if(SetCommTimeouts(serial_port, &timeouts) == FALSE) {
		DWORD err = GetLastError();
		printf("Error setting timeouts on serial port. Error %i.\n", err);
        exit(err);
	}

    //Set port options
    DCB dcb;
	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);
	BuildCommDCB((LPCSTR)"parity=N data=8 stop=1", &dcb);
	dcb.BaudRate = (DWORD)baud;

    DWORD result = SetCommState(serial_port, &dcb);
	if( !result ) {
		DWORD err = GetLastError();
		printf("Error setting comm state. Error %i.\n", err);
		exit(err);
	}

    #elif LINUX

    //Open the serial port
    serial_port = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if( serial_port == -1 ) {
		printf("Error opening serial port.\n")
        exit(1);
    }

	//Initialise the port
    int serial_port_set = fcntl(serial_port, F_SETFL, 0);
	if( serial_port_set == -1 ) {
		printf("Error initialising serial port.\n");
		exit(1);
	}

	//Set all the weird arcane settings Linux demands (boils down to 8N1)
    struct termios port_settings;
    cfsetispeed(&port_settings, baud);
    cfsetospeed(&port_settings, baud);
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
	
	//Apply settings
    serial_port_set = tcsetattr(serial_port, TCSANOW, &port_settings);
	if( serial_port_set == -1 ) {
		printf("Error configuring serial port.\n");
		exit(1)
	}

    #endif
	printf("Serial port '%s' opened successfully.\n", port);
	return;
}

/**
* Close the serial port handler.
*/
SerialPort::~SerialPort() {
    #if WINDOWS
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
void SerialPort::send_data(char* data, unsigned int size) {
	if( !serial_port ) {
		printf("Error: Serial port not open.\n");
		return;
	}
    #if WINDOWS
	DWORD written;
	if( !WriteFile(serial_port, data, (DWORD)size, &written, NULL) ) {
		printf("Error writing to the serial port.\n");
	}
    #elif LINUX
	if(write(serial_port, data, size) == -1) {
		printf("Error writing to serial port.\n");
		return;
	}
    #endif

    return;
}

/**
* Read a LF terminated line of data from the serial port.
* \param buffer A pointer to a char array to be filled with data.
* \param size The maximum number of bytes to read.
*/
void SerialPort::read_line(char *buffer, unsigned int size) {
	if( !serial_port ) {
		printf("Error: Serial port not open.\n");
		return;
	}
	unsigned int i;
	char data;
	for( i=0; i<size; i++ ) {
		#if WINDOWS
		DWORD read;
		if( !ReadFile(serial_port, (LPVOID)data, 1, &read, NULL) ) {
			printf("Error reading from serial port.\n");
			return;
		}
		#elif LINUX
		if( read(serial_port, data, 1) == -1 ) {
			printf("Error reading from serial port.\n");
			return;
		}
		#endif
		buffer[i] = data;
		if( data == '\n' ) return;
	}
	return;
}