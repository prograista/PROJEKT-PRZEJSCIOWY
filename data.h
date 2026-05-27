#include <ps5Controller.h>
#include <ESP32Servo.h>
#include <HBRIDGE.h>
#include <ENCODER.h>
#include "Wire.h"

const uint8_t RF = 12; //R - right F - front
const uint8_t RB = 13; //R - right B - back
const uint8_t LF = 27; //L - left F - front
const uint8_t LB = 14; //L - left B - back
const uint8_t RS = 26; //R - right S - speed
const uint8_t LS = 25; //L - left S - speed
const uint8_t servoPin = 15;
const uint8_t ledPin = 2;
const uint8_t encoderRPinA = 33; //right encoder signal A
const uint8_t encoderRPinB = 32; //right encoder signal B
const uint8_t encoderLPinA = 35; //left encoder signal A
const uint8_t encoderLPinB = 34; //left encoder signal B

const float imp_per_rev = 1024.0;
const float wheel_Diameter = 71.6;
const float wheel_Radius = wheel_Diameter/2.0;
float speed_R = 0.0;
float speed_L = 0.0;
uint64_t Ltime = 0;
int64_t LLPos = 0;
int64_t LRPos = 0;

const uint32_t i2c_Frequency = 100000;
const uint8_t i2c_register[10] = {0x00, 0x01, 0x02, 0x03, 0x04};
// 0x00 - nothing
// 0x01 - speed_L
// 0x02 - speed_R
// 0x03 - pos_L
// 0x04 - pos_R
uint8_t i2c_request_register = i2c_register[0];


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

void i2c_request(){
  uint8_t data[8] = {0,0,0,0,0,0,0,0};
  if(i2c_request_register == i2c_register[1]){
    int32_t Speed = (int32_t)(speed_L * 1000);
    data[0] = (Speed >> 24) & 0xFF;
    data[1] = (Speed >> 16) & 0xFF;
    data[2] = (Speed >> 8) & 0xFF;
    data[3] = Speed & 0xFF;
    Wire.write(data, 4);
  }else if(i2c_request_register == i2c_register[2]){
    int32_t Speed = (int32_t)(speed_R * 1000);
    data[0] = (Speed >> 24) & 0xFF;
    data[1] = (Speed >> 16) & 0xFF;
    data[2] = (Speed >> 8) & 0xFF;
    data[3] = Speed & 0xFF;
    Wire.write(data, 4);
  }else if(i2c_request_register == i2c_register[2]){
    int64_t POS = ENCL.getPos();
    for(int i = 7; i >= 0; i--){
      data[7-i] = (POS >> (i * 8)) & 0xFF;
    }
    Wire.write(data, 8);
  }else if(i2c_request_register == i2c_register[2]){
    int64_t POS = ENCR.getPos();
    for(int i = 7; i >= 0; i--){
      data[7-i] = (POS >> (i * 8)) & 0xFF;
    }
    Wire.write(data, 8);
  }
  i2c_request_register = i2c_register[0];
  /*Serial.print("Request, speed: ");
  Serial.print((uint32_t) speed);
  Serial.print(", data: ");
  Serial.print((uint8_t) data[0]);
  Serial.print("; ");
  Serial.print((uint8_t) data[1]);
  Serial.print("; ");
  Serial.print((uint8_t) data[2]);
  Serial.print("; ");
  Serial.println((uint8_t) data[3]);*/
}

void i2c_receive(int numBytes){
  if(numBytes == 1){
    if(Wire.available()){
      i2c_request_register = Wire.read();
    }
  }else{
    i2c_request_register = i2c_register[0];
  }
  Serial.print("Receive: ");
  Serial.println((uint8_t) i2c_request_register);
}