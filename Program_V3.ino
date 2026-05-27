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
	  //myservo.attach(servoPin, 500, 2250);
    myservo.attach(servoPin, 840, 1763);
    /*
    500 - 0
    2250 - 180
    x - 35   x=840
    y - 130  y=1763
    */
    myservo.write(90);
  }// servo
  {
    ENCL.begin();
    ENCR.begin();
    attachInterrupt(encoderLPinA, encoderL, CHANGE);
    attachInterrupt(encoderRPinA, encoderR, CHANGE);
  } // ENCODERS
  ps5.begin("e8:47:3a:74:aa:7b"); //replace with MAC address of your controller
  Wire.begin(0x67);
  Wire.setClock(i2c_Frequency);
  Wire.onRequest(i2c_request);
  Wire.onReceive(i2c_receive);
  Serial.println("Ready.");
}

void loop() {
  status = ps5.isConnected();
  digitalWrite(ledPin, !status);
  if (status){
    // print encoder positions
    //Serial.println(ENCL.getPos());
    //Serial.println(ENCR.getPos());
    // MAX: 135 MID: 90 MIN: 40
    //read data from controller
    vel = ps5.R2Value() - ps5.L2Value();
    LX = ps5.LStickX();
    // calculate servo position
    if(abs(LX) > 5){
      if(LX > 0){
        //myservo.write(map(LX, 5, 128, 90, 35));
        myservo.write(map(LX, 5, 128, 90, 0));
      }
      else{
        //myservo.write(map(LX, -5, -128, 90, 130));
        myservo.write(map(LX, -5, -128, 90, 180));
      }
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
  {
    //speed = (float)(((ENCL.getPos() - LLPos)+(ENCR.getPos() - LRPos)) / 2.0) / imp_per_rev * 2.0 * PI * wheel_Radius / (float)((millis() - Ltime) / 1000.0); // mm/s
    speed_L = (ENCL.getPos() - LLPos) / imp_per_rev * 2.0 * PI *wheel_Radius / (float)((millis()- Ltime) / 1000.0); // mm/s
    speed_R = (ENCR.getPos() - LRPos) / imp_per_rev * 2.0 * PI *wheel_Radius / (float)((millis()- Ltime) / 1000.0); // mm/s
    LLPos = ENCL.getPos();
    LRPos = ENCR.getPos();
    Ltime = millis();
  } // calculate speed
  delay(30);
}
