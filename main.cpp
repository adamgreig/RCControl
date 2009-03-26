/**************************************
 * RC Car Controller
 * Control servos connected to a
 * Pololu servo control board.
 **************************************/

#include "pololu_servo_controller.h"

int main(int argc, char** argv) {
    PololuServoController controller = PololuServoController();
    controller.set_position_abs(5, 1337);
}
