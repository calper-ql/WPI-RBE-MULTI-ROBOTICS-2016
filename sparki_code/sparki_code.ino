#include <Sparki.h>  // include the sparki library

#define ID 2
#define DATA_LENGTH 300

bool started = false;
unsigned long start_time;
unsigned list_end = 0;
char inc_data[DATA_LENGTH];

void setup() {
  Serial1.begin(115200);
}

void driveMotor(bool left, float speed) {
  if (left) {
    if (speed < 0.0) {
      sparki.motorRotate(MOTOR_LEFT, DIR_CW, -100.0 * speed);
    } else {
      sparki.motorRotate(MOTOR_LEFT, DIR_CCW, 100.0 * speed);
    }
  } else {
    if (speed < 0.0) {
      sparki.motorRotate(MOTOR_RIGHT, DIR_CCW, -100.0 * speed);
    } else {
      sparki.motorRotate(MOTOR_RIGHT, DIR_CW, 100.0 * speed);
    }
  }
}

void arcadeDrive(float speed, float sway) {
  Serial.println("Works");
  Serial.println(speed);
  Serial.println(sway);
  if (speed > 1.0) speed = 1.0;
  if (speed < -1.0) speed = -1.0;
  if (sway > 1.0) sway = 1.0;
  if (sway < -1.0) sway = -1.0;

  if (speed > 0.0) {
    if (sway < 0) {
      driveMotor(true, speed + 2.0 * (sway*speed));
      driveMotor(false, speed);
    } else {
      driveMotor(true, speed);
      driveMotor(false, speed - 2.0 * (sway*speed));
    }
  } else {
    // bug here ...
    if (sway < 0) {
      driveMotor(true, speed - 2.0 * (sway*speed));
      driveMotor(false, speed);
    } else {
      driveMotor(true, speed);
      driveMotor(false, speed + 2.0 * (sway*speed));
    }
  }

}

void loop() {
  if (Serial1.available()) {
    inc_data[list_end] = Serial1.read();
    list_end++;
    started = true;
    start_time = millis();
  }

  if (started && (start_time + 1000) <= millis()) {
    list_end = 0;
    started = false;
  }

  if (started && inc_data[list_end - 1] == '\n') {
    for (unsigned i = list_end - 1; i < DATA_LENGTH; i++) inc_data[i] = 0;
    int id;
    char speedbuf[32];
    char swaybuf[32];
    sscanf(inc_data, "%i %s %s", &id, &speedbuf, &swaybuf);

    if(speedbuf[0] == '-'){
      float open = atof(swaybuf);
      if(open > 0.5){
        sparki.gripperOpen();
      }else{
        sparki.gripperClose();
      }
      
    }else{
      float speed = atof(speedbuf);
      float sway = atof(swaybuf);
      if (id == ID) arcadeDrive(speed, sway);
    }
    list_end = 0;
    started = false;
  }

}
