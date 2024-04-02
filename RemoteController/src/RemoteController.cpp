/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
const int xValPin = A0;
const int yValPin = A1;
 int xValRaw = 0;
 int controlX = 2047;
 int controlY = 2047;
 int yValRaw = 0;
 static unsigned long int Timer;

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
 Serial.begin(9600);
 Timer = millis()+2000;
}
// void sendDatatoCloud(int controlX, int controlY){
//     Particle.publish("ControlValues(x,y): ", String(controlX)+ String(",")+ String(controlY));
// }

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    unsigned long int currentTime = millis();
    if(currentTime > Timer){
    xValRaw = (analogRead(xValPin)-2047);
    yValRaw = (analogRead(yValPin)-2047);
    controlX = map(xValRaw, -2047, 2047, -255, 255);
    controlY = map(yValRaw, -2047, 2047, -255, 255);
    // Serial.printlnf("X: %d, Y: %d, controlX: %d, controlY: %d", xValRaw, yValRaw, controlX, controlY);
    Particle.publish("ControlValues(x,y): ", String(controlX)+ String(",")+ String(controlY));
    Timer += 500;
    }
}
