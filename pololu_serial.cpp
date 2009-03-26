/**************************************
 * PololuSerial
 * Open and send data over a serial port
 **************************************/

#include "pololu_serial.h"

///Open the serial port
PololuSerial::PololuSerial() {
    #ifdef WINDOWS
    //Open the serial port
    serial_port = CreateFile(POLOLUSERIAL_PORT, GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    
    //Check it opened successfully
    if( serial_port == INVALID_HANDLE_VALUE ) {
        serial_port = NULL;
        return false;
    }
    
    //Set timeouts (mostly, don't time out)
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadTotalTimeoutConstant    = 100;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = 0;
    if(SetCommTimeouts(serial_port, &timeouts) == FALSE)
        return FALSE;
    
    //Set port options
    DCB dcb;
    BuildCommDCB("baud=28800 parity=N data=8 stop=1", &dcb)
    
    return SetCommState(serial_port, &dcb);
    
    #elifdef LINUX
    
    //Open the serial port
    serial_port = open(POLOLUSERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if( serial_port == -1 ) {
        serial_port = NULL;
        return false;
    }
    
    fcntl(serial_port, F_SETFL, 0);
    
    //Yuck! Linux makes this a complete mess
    struct termios port_settings;
    cfsetispeed(port_settings, 28800);
    cfsetospeed(port_settings, 28800);
    port_settings.c_cflag &= ~PARENB;
    port_settings.c_cflag &= ~CSTOPB;
    port_settings.c_cflag &= ~CSIZE;
    port_settings.c_cflag |= CS8;
    tcsetattr(serial_port, TCSANOW, &port_settings);
    
    #endif
}

///Close the serial port
PololuSerial::~PololuSerial() {
    #ifdef WINDOWS
    //When we're all finished, clear up by closing the serial port
    CloseHandle(serial_port);
    #elifdef LINUX
    close(serial_port);
    #endif
}

///Send data over the serial port
void PololuSerial::send_data(char* data, unsigned short int size) {
    #ifdef WINDOWS
    return WriteFile(serial_port, data, (DWORD)size);
    #elifdef LINUX
    return write(serial_port, data, size);
    #endif
    
    //To test, since I'm not currently on Windows, instead printf the
    // data that would be sent so it can be redirected to a file and
    // checked for correctness.
    unsigned short int i;
    for(i=0; i<size; i++) {
        printf("%c", data[i]);
    }
    //The 0xFF acts as a separator if multiple commands are being outputted
    printf("%c", 0xff);
    return;
}
