/**************************************
 * PololuSerial
 * Open and send data over a serial port
 **************************************/

#include "pololu_serial.h"

///Open the serial port
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

///Close the serial port
PololuSerial::~PololuSerial() {
    #if WINDOWS
    //When we're all finished, clear up by closing the serial port
    CloseHandle(serial_port);
    #endif
}

///Send data over the serial port
void PololuSerial::send_data(char* data, unsigned short int size) {
    #if WINDOWS
    return WriteFile(serial_port, data, (DWORD)size);
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
