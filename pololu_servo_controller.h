/**************************************
 * PololuServoController
 * Represents the servo controller
 * board and holds the serial port
 * to interface with it
 **************************************/

#pragma once

#include "pololu_serial.h"

///Control servos attached to a controller
class PololuServoController {
    public:
        PololuServoController();
        
        void config(unsigned short int servo, bool on=false, bool direction=false, unsigned short int range=15);
        void set_speed(unsigned short int servo, unsigned short int speed=0);
        void set_neutral(unsigned short int servo, unsigned short int position=3000);
        void set_position_7(unsigned short int servo, unsigned short int position);
        void set_position_8(unsigned short int servo, unsigned short int position);
        void set_position_abs(unsigned short int servo, unsigned short int position);
    private:
        PololuSerial serial;
};
