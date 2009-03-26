/**************************************
 * PololuServoController
 * Represents the servo controller
 * board and holds the serial port
 * to interface with it
 **************************************/

#include "pololu_servo_controller.h"
#include "pololu_serial.h"

///Constructor initialises the serial port
PololuServoController::PololuServoController() {
    serial = PololuSerial();
}

///Set the servo configuration
///\param servo the servo number to configure
///\param on whether the servo should be turned on
///\param direction what direction large positions send the servo
///\param range what the position value is multiplied by
void PololuServoController::config(unsigned short int servo, bool on, bool direction, unsigned short int range) {
    
    //Send the servo configuration line
    char data[5];
    data[0] = 0x80;     //START
    data[1] = 0x01;     //Device ID: Pololu 16-Servo Controller
    data[2] = 0x00;     //Command: Set-Parameters
    data[3] = servo;    //Servo_Num: Current servo number
    data[4] = 0x00;     //Data
    
    if(on)
        data[4] |= 0x40;
    
    if(direction)
        data[4] |= 0x20;
    
    data[4] |= 0x1F & range;
    
    serial.send_data(data, 5);
    
}

///Set the speed this servo will go to a position with
///\param speed How fast the servo rotates to that value, 0(fast) to 127(slow)
void PololuServoController::set_speed(unsigned short int servo, unsigned short int speed) {
    char data[5];
    data[0] = 0x80;
    data[1] = 0x01;
    data[2] = 0x01;
    data[3] = servo;
    data[4] = speed;
    
    serial.send_data(data, 5);
}

///Set the neutral position
///\param position The neutral position of the servo, default 3000ms
void PololuServoController::set_neutral(unsigned short int servo, unsigned short int position) {
    char data[6];
    data[0] = 0x80;
    data[1] = 0x01;
    data[2] = 0x05;
    data[3] = servo;
    data[4] = 0x7F & (position >> 7);
    data[5] = 0x7F & position;
    
    serial.send_data(data, 6);
}

///Set position, 7bit
///\param position Multiplied by range and adjusted for neutral, 7-bit, at range=15 90deg
void PololuServoController::set_position_7(unsigned short int servo, unsigned short int position) {
    char data[5];
    data[0] = 0x80;
    data[1] = 0x01;
    data[2] = 0x02;
    data[3] = servo;
    data[4] = 0x7F & position;
    
    serial.send_data(data, 5);
}

///Set position, 8bit
///Sends an extra byte of data but gives you more range or precision
///\param position Multiplied by range and adjusted for neutral, 8-bit, range=15 180deg
void PololuServoController::set_position_8(unsigned short int servo, unsigned short int position) {
    char data[6];
    data[0] = 0x80;
    data[1] = 0x01;
    data[2] = 0x03;
    data[3] = servo;
    data[4] = 0x01 & (position >> 7);
    data[5] = 0x7F & (position);
    
    serial.send_data(data, 6);
}

///Set the absolute position
///\param position The absolute position to go to, range 500 to 5500, corresponds to 250ms to 2750ms
void PololuServoController::set_position_abs(unsigned short int servo, unsigned short int position) {
    char data[6];
    data[0] = 0x80;
    data[1] = 0x01;
    data[2] = 0x04;
    data[3] = servo;
    data[4] = 0x7F & (position >> 7);
    data[5] = 0x7F & position;
    
    serial.send_data(data, 6);
}
