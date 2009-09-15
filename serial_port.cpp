#include "serial_port.h"

/**
* Open the serial port and check it succeeded.
* The named port is opened at the given baud rate using 8N1
* with no hardware flow control.
* \param port The serial port to use, e.g. COM8 or /dev/ttyUSB0
* \param baud The baud rate to use, e.g. 9600 or 4800. On linux this
* is restricted to a specific range of common values.
*/
SerialPort::SerialPort(char* port, int baud) {
    //Open the serial port
    serial_port = CreateFile((LPCSTR)port, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    //Check it opened successfully
    if( serial_port == INVALID_HANDLE_VALUE ) {
		DWORD err = GetLastError();
		cout << "Error opening serial port. Error " << err << endl;
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
		cout << "Error setting timeouts on serial port. Error " << err << endl;
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
		cout << "Error setting comm state. Error " << err << endl;
		exit(err);
	}
	cout << "Serial port '" << port << "' opened successfully.\n" << endl;
	return;
}

/**
* Close the serial port handler.
*/
SerialPort::~SerialPort() {
    CloseHandle(serial_port);
}

/**
* Send data over the open serial port.
* \param data A char array of data to send. Not null-terminated.
* \param size The number of bytes to send from data.
*/
int SerialPort::send_data(char* data, unsigned int size) {
	if( !serial_port ) {
		cout << "Error: Serial port not open." << endl;
		return -1;
	}

	DWORD written;
	if( !WriteFile(serial_port, data, (DWORD)size, &written, NULL) ) {
		cout << "Error writing to the serial port." << endl;
		return -1;
	} else {
		return written;
	}

    return 0;
}

/**
* Read a LF terminated line of data from the serial port.
* \param buffer A pointer to a char array to be filled with data.
* \param size The maximum number of bytes to read.
*/
int SerialPort::read_line(char *buffer, unsigned int size) {
	if( !serial_port ) {
		cout << "Error: Serial port not open." << endl;
		return -1;
	}

	unsigned int i;
	unsigned int total_read = 0;
	char data[1];
    bool end_of_line = false;

	DWORD data_read;
	
	for( i=0; i<size; i++ ) {
		if( !ReadFile(serial_port, (LPVOID)data, 1, &data_read, NULL) ) {
			cout <<"Error reading from serial port." << endl;
			return -1;
		} else if( data_read == 0 ) {
			return 0;
		} else {
			total_read++;
		}
        
		//For some reason the end of line is not consistently signalled
		// with \r\n. Instead sometimes \r\r, \n\r, \n\n is seen.
		// Therefore, just watch out for two of these control characters
		// before returning.
        if( data[0] == '\n' || data[0] == '\r' ) {
            total_read--;
            if( end_of_line ) {
                buffer[i - 1] = 0x00;
                return (int)total_read;
            } else {
                end_of_line = true;
            }
        } else {
            end_of_line = false;
            buffer[i] = data[0];
        }
        
	}
	return total_read;
}
