/**************************************
 * PololuSerial
 * Open and send data over a serial port
 **************************************/

#include "pololu_serial.h"

PololuSerial::PololuSerial() {
    #if WINDOWS
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
    #endif
}

PololuSerial::~PololuSerial() {
    #if WINDOWS
    CloseHandle(serial_port);
    #endif
}

void PololuSerial::send_data(char* data, unsigned short int size) {
    #if WINDOWS
    return WriteFile(serial_port, data, (DWORD)size);
    #endif
    unsigned short int i;
    for(i=0; i<size; i++) {
        printf("%c", data[i]);
    }
    printf("%c", 0xff);
    return;
}
