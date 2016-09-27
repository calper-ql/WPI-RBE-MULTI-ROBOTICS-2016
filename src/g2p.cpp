#include <iostream>
#include <string>
#include <cmath>
#include "UTIL_SRC/FieldComputer.hpp"

#define PI 3.1415926535
#define ALLOWED_ANGLE_ERROR 1

// Allowed distance from target is in pixels.
#define ALLOWED_DISTANCE 10

using namespace std;

class PID {
public:
    // Integrated Error is updated each time class gets an update and error entered is
    // is added to Integrated Error.
    float I_error;
    float error;
    float KP;
    float KI;
    float KD;
    float delta_pos;

    // Initializes the desired value and K values from the arguements.
    PID(float KP, float KI, float KD){
        this->KP = KP;
        this->KI = KI;
        this->KD = KD;
        I_error = 0;
    }

    // Updates the PID with a new error and calculates the pid output
    float update(float error){
        float delta_error = this->error - error;
        this->error = error;
        I_error += error;
        delta_pos = (KP * error) + (KI * I_error) + (KD * delta_error);
        if(delta_pos < 0) return -delta_pos;
        return delta_pos;
    }

    // Sets the I_error to 0
    void reset(){
        I_error = 0;
    }
};

Robot getRobot(int id, FieldComputer fc){
    // Request update from the field computer.
    FieldData data = fc.getFieldData();
    Robot robot(id);
    bool found = false;
    for(unsigned i = 0; i < data.robots.size(); i++){
        if(data.robots[i].id() == robot.id()) {
            robot = data.robots[i];
            return robot;
        }
    }
    return Robot(-1);
}

double angleError(double diff){
    double difference = diff;
    while (difference < -180) difference += 360;
    while (difference > 180) difference -= 360;
    return difference;
}

int main(int argc, char *argv[])
{
    // Print usage.
    if(argc < 5){
        printf("\n Usage: %s <ip of server> <id> <targetx> <targety>\n",argv[0]);
        return 1;
    }

    // Gether required fields.
    string ip = string(argv[1]);
    int id = atoi(argv[2]);
    int targetx = atoi(argv[3]);
    int targety = atoi(argv[4]);

    // Initialize the field computer.
    FieldComputer fc(ip);
    // Enable verbose for debugging.
    //fc.enableVerbose();

    /*
    target_reached is set to false to indicate
    that the robot has not reached the target coordinates.
    */
    bool target_reached = false;

    while(!target_reached){
        Robot robot = getRobot(id, fc);
        if(robot.id() == id){
            PID pid(0.01, 0.005, 0.00005);
            while(true){
                robot = getRobot(id, fc);
                if(robot.id() == id){
                    float target_angle = 180.0 - atan2(robot.y() - targety, robot.x() - targetx) * 180.0 / PI;
                    float angle_diffrence = target_angle - robot.theta();
                    float angle_error = angleError(angle_diffrence);
                    if(angle_error < ALLOWED_ANGLE_ERROR && angle_error > -ALLOWED_ANGLE_ERROR) break;
                    if(angle_error < 0) fc.arcadeDrive(id, pid.update(angle_error), 1.0);
                    else fc.arcadeDrive(id, pid.update(angle_error), -1.0);
                    usleep(50000);
                }
            }
            fc.arcadeDrive(id, 1.0, 0.0);
            if(sqrt(pow(robot.x() - targetx,2.0) + pow(robot.y() - targety,2.0)) < ALLOWED_DISTANCE) target_reached = true;
        }
        usleep(50000);
    }
    fc.arcadeDrive(id, 0.0, 0.0);
    return 0;
}
