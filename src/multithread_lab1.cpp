#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
#include <math.h>
#include <iostream>
#include <string>
#include "../src/UTIL_SRC/FieldComputer.hpp"
#include <dispatch/dispatch.h>

#define PI 3.1415926535

using namespace std;

int thetaTransform(float thetaRead) 
{
    return ((thetaRead - 270.0) >= 0.0) ? thetaRead - 270 : thetaRead - 270 + 360;
}

float d2r(float d) {
    return d * PI / 180;
}

float r2d(float r) {
    return r * 180 / PI;
}

void execg2p(int robot, float x, float y) {
    std::string g2p = "./g2p "; // note trailing space
    std::string IP = "127.0.0.1 "; // note trailing space
    std::string command;
    command = g2p + IP;
    std::string robotNum = std::to_string(robot) + " ";
    command = command + robotNum + std::to_string(x)  + " " + std::to_string(y);
    cout << command.c_str() << endl;
    std::system(command.c_str());
}

int main(int argc, char *argv[])
{

    // Manage server connection
    string ip = string(argv[1]);
    FieldComputer fc(ip);
    fc.enableVerbose();
    FieldData data = fc.getFieldData();

    // Initialization of stuff.
    int robots = 2; 
    pid_t pid[robots];
    int robot;  
    int swarmState = 0;
    int offset = 90;
    float theta;
    int slopeSign;
    int targetX1, targetY1, targetX2, targetY2;




    // If the thetas are hovering about the 0/360 line
    if (abs(thetaTransform(data.entities[0].theta()) - thetaTransform(data.entities[1].theta())) > 90) // If given thetas are badly off, it's probs 0
        theta = 0.0; // This should be better
    else
        theta = (thetaTransform(data.entities[0].theta()) + thetaTransform(data.entities[1].theta())) / 2.0; // Approx theta of box

    theta = (theta > 180.0) ? -1.0*(theta - 360.0) : theta;
    // @TODO: This needs to actually deal with angles better





    targetX1 = (data.entities[0].x() < data.entities[1].x()) ? data.entities[0].x() : data.entities[1].x();
    targetY1 = (data.entities[0].x() < data.entities[1].x()) ? data.entities[0].y() : data.entities[1].y();



    targetX2 = (data.entities[0].x() > data.entities[1].x()) ? data.entities[0].x() : data.entities[1].x();
    targetY2 = (data.entities[0].x() > data.entities[1].x()) ? data.entities[0].y() : data.entities[1].y();

    cout << "Target1: " << targetX1 << " " << targetY1 << endl;
    cout << "Target2: " << targetX2 << " " << targetY2 << endl;

    slopeSign = (targetY1 > targetY2) ? 1: -1;
    if (theta == 0)
        slopeSign = 1;

    cout << "Slope: " << slopeSign << endl;


    //This is supposed to be for the triangulation/isolation of x and y offsets, not working too well though.

    int target1_x0 = (offset * sin(d2r(theta)) * slopeSign);
    int target1_y0 = (offset * cos(d2r(theta)) * slopeSign);

    int target2_x0 = (offset * sin(d2r(theta)) * slopeSign);
    int target2_y0 = (offset * cos(d2r(theta)) * slopeSign);

    if (slopeSign == -1) {
        target1_y0 = - target1_y0;
        target2_y0 = - target2_y0;
    }

    cout << "Offsets:" << endl;
    cout << "target1_x0 " << target1_x0 << endl;
    cout << "target1_y0 " << target1_y0 << endl;
    cout << "target2_x0 " << target2_x0 << endl;
    cout << "target2_y0 " << target2_y0 << endl;

    targetX1 = targetX1 - target1_x0;
    targetY1 = targetY1 - target1_y0;

    targetX2 = targetX2 - target2_x0;
    targetY2 = targetY2 - target2_y0;

    cout << "Recalculating..." << endl;
    cout << "Theta: " << theta << endl;
    cout << "Target1: " << targetX1 << " " << targetY1 << endl;
    cout << "Target2: " << targetX2 << " " << targetY2 << endl;

    //exit(0);

    /////////
    // Create initial threads
    /////////
    for (robot=0; robot < robots; ++robot) {
        if ((pid[robot] = fork()) == -1)
           return (1); // Who needs error handling, this is robotics
        if (pid[robot] == 0) {
            break;
      }
    }

    /////////////////////
    //State machine logic
    /////////////////////  
    switch (swarmState) {

        //SWARM STATE 0
        case 0:// Initial state of the swarm.
          switch (robot) { // The child does it's case task.

              //ROBOT 0 STATE 0
              case 0: // Thread 0
                  cout << "   Robot 0 moving... \n";
                  execg2p(2, targetX1, targetY1);
                  cout << "   Robot 0 moved. \n";
                  exit(0);
              break;

              //ROBOT 1 STATE 0
              case 1: // Thread 1
                  cout << "   Robot 1 moving... \n";
                  execg2p(3, targetX2, targetY2);
                  cout << "   Robot 1 moved. \n";
                  exit(0);
              break;

              // MAIN THREAD
              default: // Parent thread
                  //WAIT FOR ALL ROBOTS TO FINISH WORK
                  for (robot=0; robot < robots; ++robot) {
                      int status;
                      waitpid(pid[robot], &status, 0);
                      cout << "A robot finished!" << endl;
                  }
                  for(unsigned i = 0; i < data.entities.size(); i++){
                      cout << " - Entity id: " << data.entities[i].id() << endl;
                      cout << "x: " << data.entities[i].x() << endl;
                      cout << "y: " << data.entities[i].y() << endl;
                      cout << "theta: " << data.entities[i].theta() << endl;
                      cout << "width: " << data.entities[i].width() << endl;
                      cout << "height: " << data.entities[i].height() << endl;
                  }
                  for(unsigned i = 0; i < data.robots.size(); i++){
                      cout << " - Robot id: " << data.robots[i].id() << endl;
                      cout << "x: " << data.robots[i].x() << endl;
                      cout << "y: " << data.robots[i].y() << endl;
                      cout << "theta: " << data.robots[i].theta() << endl;
                      cout << "width: " << data.robots[i].width() << endl;
                      cout << "height: " << data.robots[i].height() << endl;
                  }

                  targetX1 = targetX1 + 2.0 * target1_x0;
                  targetY1 = targetY1 + 2.0 * target1_y0;

                  targetX2 = targetX2 + 2.0 * target2_x0;
                  targetY2 = targetY2 + 2.0 * target2_y0;

                  swarmState++;
                  for (robot=0; robot < robots; ++robot) {
                      if ((pid[robot] = fork()) == -1)
                          return (1); // Who needs error handling, this is robotics
                      if (pid[robot] == 0)  // If we are the child then don't make children
                          break;
                  }
        break;
        } // END SWARM STATE 1

        // SWARM STATE 1
        case 1: // Next swarm state
            switch (robot) {
                case 0:
                    cout << "   Robot 0 moving... \n";
                    execg2p(2, targetX1, targetY1);
                    cout << "   Robot 0 moved. \n";
                    exit(0);
                break;
                case 1:
                    cout << "   Robot 1 moving... \n";
                    execg2p(3, targetX2, targetY2);
                    cout << "   Robot 1 moved. \n";
                    exit(0);
                break;
                default:
                    cout << " Swarm has completed state 1" << endl;
          break;
        }
    }
}
