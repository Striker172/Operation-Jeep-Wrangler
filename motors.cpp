#include "Particle.h"
SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);
using namespace std;

SerialLogHandler logHandler(LOG_LEVEL_INFO);

//Motor A
const int motorPin1  = 1;  // Pin 14 of L293
const int motorPin2  = 2;  // Pin 10 of L293

//Motor B
const int motorPin3  = 3; // Pin  7 of L293
const int motorPin4  = 4;  // Pin  2 of L293

//input from serial monitor. CHANGE TO CLOUD VARIABLE
char t;

void setup() {
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
    Serial.begin(9600);
}

void loop() {
if(Serial.available()){
  t = Serial.read();
  Serial.println(t);
}
 
if(t == 'F'){            //move  forward(all motors rotate in forward direction)
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}
 
else if(t=='B'){
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
}
 
else if(t=='L'){          //turn right (left side motors rotate in forward direction, right side motors doesn't rotate)
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}
 
else if(t=='R'){          //turn left (right side motors rotate in forward direction, left side motors doesn't rotate)
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
}
 
else {          //Stop
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
}
delay(100);
}
