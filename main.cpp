/**************************************
 * RC Car Controller
 * Control servos connected to a
 * Pololu servo control board.
 **************************************/

#include "pololu_servo_controller.h"

int main(int argc, char** argv) {
    //Make a controller
    PololuServoController controller = PololuServoController();
    printf("Sleeping\n");
    sleep(5);
    printf("Sending command...\n");
    //Test: Set servo number 5's position to some absolute value
    controller.config(3,true,false,15);
    printf("Sent.\n");
    sleep(1);
    controller.set_position_abs(14,4096);
    sleep(1);
    printf("Send.\n");
}
