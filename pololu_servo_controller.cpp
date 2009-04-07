#include "pololu_servo_controller.h"

/**
* Initialise the servo controller. Currently does nothing.
*/
PololuServoController::PololuServoController() {
	serial = new SerialPort(POLOLU_PORT, 9600);
}

/**
* On destruction, set all servos to OFF
*/
PololuServoController::~PololuServoController() {
	unsigned short int i;
	for(i=0; i<16; i++) {
		config(i, false);
	}
}

/**
* Set the servo configuration, and send out the values over the serial port.
* \param servo The servo number to configure.
* \param on whether The servo should be turned on.
* \param direction What direction large positions send the servo.
* \param range what The position value is multiplied by.
*
* From the Pololu manual:
*
*  *  Bit 6 specifies whether a servo  is on or not; a 1  turns  the 
*  servo on, and a 0  (default)  turns  it off.
*
*  *  Bit 5  sets  the direction  the  servo moves, which only 
*  applies  to 7- and 8-bit position commands.   If  the bit  is 0 
*  (default), a  larger position number causes  the output 
*  pulse  to  get  bigger;  if  the  bit  is  1,  a  larger  position 
*  number will make  the output pulse  shorter.
*
*  *  Bits 0-4 set  the range  through which  the servo moves  in 
*  7-  and  8-bit  commands.   A  larger  value will  give  a 
*  larger range, and setting the range to 0 will make the 
*  servo always stay at neutral.  Given the same range setting, an 8-bit command will 
*  move the servo through twice the range of a 7-bit command.   The default range 
*  setting  is 15, which will give approximately 180 degrees  in 8-bit commands and 90 
*  degrees  in 7-bit commands.
*/
void PololuServoController::config(unsigned short int servo, bool on, bool direction, unsigned short int range) {

    //Send the servo configuration line
    char data[5];
    data[0] = (char)0x80;     //START
    data[1] = 0x01;     //Device ID: Pololu 16-Servo Controller
    data[2] = 0x00;     //Command: Set-Parameters
    data[3] = (char)servo;//Servo_Num: Current servo number
    data[4] = 0x00;     //Data

    if(on)
        data[4] |= 0x40;

    if(direction)
        data[4] |= 0x20;

    data[4] |= 0x1F & (char)range;

    serial->send_data(data, 5);

}

/**
* Set the speed this servo will go to a position with
* \param speed How fast the servo rotates to that value, 0(fast) to 127(slow)
*
* From the Pololu manual:
*
* This command allows you  to set  the speed at which  the servo moves.  If  the speed  is set 
*  to 0 (default), the output pulse will instantly change to the set position.   If the speed 
*  value  is nonzero,  the pulse changes gradually from  the old position  to  the new position.  
*  With a speed of 1, the pulse width changes at 50 microseconds per second, up to a 
*  maximum  speed of 6.35 ms per  second with a  speed  setting of 127.
*/
void PololuServoController::set_speed(unsigned short int servo, unsigned short int speed) {
    char data[5];
    data[0] = (char)0x80;
    data[1] = 0x01;
    data[2] = 0x01;
    data[3] = (char)servo;
    data[4] = (char)speed;

    serial->send_data(data, 5);
}

/**
* Set the neutral position of a servo.
* \param position The neutral position of the servo, default 3000ms
*
* From the Pololu manual:
*
* Setting neutral only applies  to 7- and 8-bit commands.   The neutral value sets  the 
*  middle of a range, and corresponds to a 7-bit position value of 63.5 or an 8-bit position 
*  value of 127.5. The neutral position is an absolute position just like command 4, and 
*  setting the neutral position will move the servo to that position.   The default value is 
*  3000.   It may be useful to change neutral if you change servos and need to calibrate 
*  your  system, or  if you cannot get your mechanical  linkages  to  just  the  right  lengths.
*/
void PololuServoController::set_neutral(unsigned short int servo, unsigned short int position) {
    char data[6];
    data[0] = (char)0x80;
    data[1] = 0x01;
    data[2] = 0x05;
    data[3] = (char)servo;
    data[4] = 0x7F & (char)(position >> 7);
    data[5] = 0x7F & (char)position;

    serial->send_data(data, 6);
}

/**
* Set servo position, 7bit.
* Only takes 5 bytes but has less range or precision.
* \param position Multiplied by range and adjusted for neutral, 7-bit, at range 15 this gives 90 degrees of movement.
*
* From the Pololu manual:
*
* When this command is sent, the data value is multiplied by the range setting for the 
*  corresponding servo and adjusted for the neutral setting.  This command can be useful 
*  in  speeding up communications  since only 5  total bytes are  sent  to  set a position.   
*  Setting a  servo position will automatically  turn  it on.
*/
void PololuServoController::set_position_7(unsigned short int servo, unsigned short int position) {
    char data[5];
    data[0] = (char)0x80;
    data[1] = 0x01;
    data[2] = 0x02;
    data[3] = (char)servo;
    data[4] = 0x7F & (char)position;

    serial->send_data(data, 5);
}

/**
* Set servo position, 8bit.
* Sends an extra byte of data but gives you more range or precision.
* \param position Multiplied by range and adjusted for neutral, 8-bit, at range 15 this gives 180 degrees of movement
* From the Pololu manual:
*
* This command is just like the 7-bit version, except that two data bytes must be sent to 
*  transfer 8 bits.   Bit 0 of data 1 contains the most significant bit (bit 7 of your position 
*  byte), and the lower bits of data 2 contain the lower seven bits of your position byte.   
*  (Bit 7  in data bytes  sent over  the  serial  line must always be 0.)
*/
void PololuServoController::set_position_8(unsigned short int servo, unsigned short int position) {
    char data[6];
    data[0] = (char)0x80;
    data[1] = 0x01;
    data[2] = 0x03;
    data[3] = (char)servo;
    data[4] = 0x01 & (char)(position >> 7);
    data[5] = 0x7F & (char)(position);

    serial->send_data(data, 6);
}

/**
* Set the servo's position absolutely, without using range or neutral values.
* \param position The absolute position to go to, range 500 to 5500, corresponds to 250ms to 2750ms
*
* From the Pololu manual:
*
* This command allows direct control of the internal servo position variable.   Neutral, 
*  range, and direction settings do not affect this command.   Data 2 contains the lower 7 
*  bits, and Data 1 contains  the upper bits.  The range of valid values  is 500  through 5500.  
*  Setting a  servo position will automatically  turn  it on.
*/
void PololuServoController::set_position_abs(unsigned short int servo, unsigned short int position) {
    char data[6];
    data[0] = (char)0x80;
    data[1] = 0x01;
    data[2] = 0x04;
    data[3] = (char)servo;
    data[4] = 0x7F & (char)(position >> 7);
    data[5] = 0x7F & (char)position;

    serial->send_data(data, 6);
}
