/**************************************
 * RC Car Controller
 * Control servos connected to a
 * Pololu servo control board.
 **************************************/

#include "pololu_servo_controller.h"

int main(int argc, char** argv) {
    //Make a controller
    PololuServoController controller = PololuServoController();
    
    //Test: Set servo number 5's position to some absolute value
    controller.set_position_abs(5, 1337);
}
