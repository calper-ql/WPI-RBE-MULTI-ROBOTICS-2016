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

int main(int argc, char *argv[])
{
  int swarmState = 0;

  string ip = string(argv[1]);
  FieldComputer fc(ip);
  fc.enableVerbose();
  FieldData data = fc.getFieldData();

  int robots = 2; 
  pid_t pid[robots];
  int robot;



  int offset = 90;

  // float theta;

  // if ((data.entities[0].theta() - data.entities[1].theta()) > 10) 
  //   theta = 0;
  // else 
  //   theta = data.entities[0].theta() + data.entities[1].theta() / 2; // Approx theta of box
  float theta;

  if (abs(thetaTransform(data.entities[0].theta()) - thetaTransform(data.entities[1].theta())) > 90) // If given thetas are badly off, it's probs 0
    theta = 0.0; // This should be better
  else
    theta = (thetaTransform(data.entities[0].theta()) + thetaTransform(data.entities[1].theta())) / 2.0; // Approx theta of box

  theta = (theta > 180.0) ? -1.0*(theta - 360.0) : theta;
  // @TODO: This needs to actually deal with angles better

  int targetX1, targetY1, targetX2, targetY2;


  std::string g2p = "./g2p "; // note trailing space
  std::string IP = "127.0.0.1 "; // note trailing space
  std::string command1;
  std::string command2;

  targetX1 = (data.entities[0].x() < data.entities[1].x()) ? data.entities[0].x() : data.entities[1].x();
  targetY1 = (data.entities[0].x() < data.entities[1].x()) ? data.entities[0].y() : data.entities[1].y();



  targetX2 = (data.entities[0].x() > data.entities[1].x()) ? data.entities[0].x() : data.entities[1].x();
  targetY2 = (data.entities[0].x() > data.entities[1].x()) ? data.entities[0].y() : data.entities[1].y();

  cout << "Target1: " << targetX1 << " " << targetY1 << endl;
  cout << "Target2: " << targetX2 << " " << targetY2 << endl;

  int slopeSign = (targetY1 > targetY2) ? 1: -1;
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

  case 0:// Initial state of the swarm.
    switch (robot) { // The child does it's case task.
    case 0: // Thread 0
      cout << "   Robot 0 moving... \n";
      //system("sleep 3");
      command1 = g2p + IP;
      command1 = command1 + "3 " + std::to_string(targetX1)  + " " + std::to_string(targetY1);
      cout << command1.c_str() << endl;
      std::system(command1.c_str());
      cout << "   Robot 0 moved. \n";
      exit(0);
    break;

    case 1: // Thread 1
      cout << "   Robot 1 moving... \n";
      command2 = g2p + IP;
      command2 = command2 + "2 " + std::to_string(targetX2)  + " " + std::to_string(targetY2);
      cout << command2.c_str() << endl;
      std::system(command2.c_str());
      //system(strcat("./g2p 127.0.0.1 2 ", strcat(std::to_string(targetX2 - offset), std::to_string(targetY2))));
      cout << "   Robot 1 moved. \n";
      exit(0);
    break;

    default: // Parent thread
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
      // Update TODO
      // targetX1 = targetX1 + offset * sin(theta) * slopeSign;
      // targetY1 = targetY1 + offset * cos(theta) * slopeSign;

      // targetX2 = targetX2 + offset * sin(theta) * slopeSign;
      // targetY2 = targetY2 + offset * cos(theta) * slopeSign;

      swarmState++;
      for (robot=0; robot < robots; ++robot) {
        if ((pid[robot] = fork()) == -1)
          return (1); // Who needs error handling, this is robotics
        if (pid[robot] == 0) { // If we are the child then don't make children
          break;
        }
      }
    break;
    } // Closes robot switch on case 0s
  case 1: // Next swarm state
    switch (robot) {
      case 0:
        cout << "   Robot 0 moving... \n";
        //system("sleep 3");
        command1 = g2p + IP;
        command1 = command1 + "3 " + std::to_string(targetX1)  + " " + std::to_string(targetY1);
        cout << command1.c_str() << endl;
        std::system(command1.c_str());
        cout << "   Robot 0 moved. \n";
        exit(0);
      break;
      case 1:
        cout << "   Robot 1 moving... \n";
        command2 = g2p + IP;
        command2 = command2 + "2 " + std::to_string(targetX2)  + " " + std::to_string(targetY2);
        cout << command2.c_str() << endl;
        std::system(command2.c_str());
        //system(strcat("./g2p 127.0.0.1 2 ", strcat(std::to_string(targetX2 - offset), std::to_string(targetY2))));
        cout << "   Robot 1 moved. \n";
        exit(0);
      break;

      default:
        cout << " Master says good." << endl;
      break;
    }
    //cout << "NYEH" << endl;
  }
}
