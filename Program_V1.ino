#include "data.h"

void setup() {
  Serial.begin(115200);
  {
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, 1);
  } // GPIO
  {
    ESP32PWM::allocateTimer(3);
	  myservo.setPeriodHertz(50);
	  myservo.attach(servoPin, 500, 2250);
    myservo.write(90);
  }// servo
  {
    ENCL.begin();
    ENCR.begin();
    attachInterrupt(encoderLPinA, encoderL, CHANGE);
    attachInterrupt(encoderRPinA, encoderR, CHANGE);
  } // ENCODERS
  ps5.begin("e8:47:3a:74:aa:7b"); //replace with MAC address of your controller
  Serial.println("Ready.");
}

void loop() {
  status = ps5.isConnected();
  digitalWrite(ledPin, !status);
  if (status){
    // print encoder positions
    Serial.println(ENCL.getPos());
    Serial.println(ENCR.getPos());
    // MAX: 135 MID: 90 MIN: 40
    //read data from controller
    vel = ps5.R2Value() - ps5.L2Value();
    LX = ps5.LStickX();
    // calculate servo position
    if(abs(LX) > 5){
      if(LX > 0) myservo.write(map(LX, 5, 128, 90, 35));
      else myservo.write(map(LX, -5, -128, 90, 130));
    }else myservo.write(90);
    //set motor velocity
    MotorR.Move(vel);
    MotorL.Move(vel);
  }else{
    //STOP
    myservo.write(90);
    MotorR.FastStop();
    MotorL.FastStop();
  }
  delay(30);
}
