/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#define xValPin A0
#define yValPin A1
 int xValRaw = 0;
 int controlX = 2047;
 int controlY = 2047;
 int yValRaw = 0;
#define hornSwitchPin D0
unsigned long int samplePositionTimer;
unsigned long int sampleHornSwitch;
// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);
void sendDatatoCloud(int controlX, int controlY);
// setup() runs once, when the device is first turned on
void setup() {
 pinMode(xValPin, INPUT);
 pinMode(yValPin, INPUT);
 pinMode(hornSwitchPin, INPUT_PULLDOWN);
 Serial.begin(9600);
 samplePositionTimer = millis()+500;
 sampleHornSwitch = millis()+1000;
}
// void sendDatatoCloud(int controlX, int controlY){
//     Particle.publish("ControlValues(x,y): ", String(controlX)+ String(",")+ String(controlY));
// }

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    unsigned long int currentTime = millis();
    unsigned long int currentTimeMicro = millis();
    if(currentTimeMicro > samplePositionTimer){
    xValRaw = (analogRead(xValPin)-2047);
    yValRaw = (analogRead(yValPin)-2047);
    controlX = map(xValRaw, -2047, 2047, -255, 255);
    controlY = map(yValRaw, -2047, 2047, -255, 255);
    // Serial.printlnf("X: %d, Y: %d, controlX: %d, controlY: %d", xValRaw, yValRaw, controlX, controlY);
    Particle.publish("ControlValues(x,y): ", String(controlX)+ String(",")+ String(controlY));
    samplePositionTimer += 500;
    }
    if(currentTime > sampleHornSwitch){
        if(digitalRead(hornSwitchPin) == HIGH){
            Particle.publish("HornSwitch: ", "ON");
        }
        else{
            Particle.publish("HornSwitch: ", "OFF");
        }
        sampleHornSwitch += 1000;
    }
}
