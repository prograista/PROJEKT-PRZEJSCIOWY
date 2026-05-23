#include <ps5Controller.h>
#include <ESP32Servo.h>
#include <HBRIDGE.h>
#include <ENCODER.h>

#define RF 12 //R - right F - front
#define RB 13 //R - right B - back
#define LF 27 //L - left F - front
#define LB 14 //L - left B - back
#define RS 26 //R - right S - speed
#define LS 25 //L - left S - speed
#define servoPin 15
#define ledPin 2
#define encoderRPinA 33 //right encoder signal A
#define encoderRPinB 32 //right encoder signal B
#define encoderLPinA 35 //left encoder signal A
#define encoderLPinB 34 //left encoder signal B

bool status = false; //status of connection
int LX = 0; //left joystick X
int vel = 0; // velocity

Servo myservo;

HBRIDGE MotorR(RF, RB, RS);
HBRIDGE MotorL(LF, LB, LS);

ENCODER ENCL(encoderLPinA, encoderLPinB);
ENCODER ENCR(encoderRPinA, encoderRPinB);

void ARDUINO_ISR_ATTR encoderL() {
  ENCL.count();
}

void ARDUINO_ISR_ATTR encoderR() {
  ENCR.count();
}