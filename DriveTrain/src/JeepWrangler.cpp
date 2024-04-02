/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */
#include "Particle.h"
#include "string.h"
//#include "ezButton.h" This is just a library that simplifies the use of buttons and handles debouncing for you and such. We can add it if need be
struct MotorControl {
  struct pins {
    String name;
    int forwardPin;
    int powerPin;
    int backwardPin;
  };
  MotorControl(String name, int forwardPin, int powerPin, int backwardPin);
  struct pins pins;
};
// Include Particle Device OS APIs


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);
const int switchPin = D6;
// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);
int setDriveControl(String inputString);
int driveValue[2]; //(x,y) values for the drive train
MotorControl Motors[2] = {
  MotorControl("Left", D0, D1, D2),
  MotorControl("Right", D3, D4, D5)
};
 MotorControl::MotorControl(String name, int forwardPin, int powerPin, int backwardPin){
    pins.name = name;
    pins.forwardPin = forwardPin;
    pins.powerPin = powerPin;
    pins.backwardPin = backwardPin;
  }
// setup() runs once, when the device is first turned on
void setDriveControl(const char *event, const char *data){
  String inputString = String(data);
  int xValue = inputString.substring(0, inputString.indexOf(",")).toInt();
  int yValue = inputString.substring(inputString.indexOf(",")+1).toInt();
  driveValue[0] = xValue;
  driveValue[1] = yValue;
}
void setup() {
  // Put initialization like pinMode and begin functions here
  for(int i = 0; i < 2; i++){
    pinMode(Motors[i].pins.forwardPin, OUTPUT);
    pinMode(Motors[i].pins.powerPin, OUTPUT);
    pinMode(Motors[i].pins.backwardPin, OUTPUT);
  }
  pinMode(switchPin, INPUT);
  Particle.subscribe("ControlValues(x,y)", setDriveControl,"2f0028001547313137363331");
}

void onlineControl(){
  //read the input from the cloud and set the output pins accordingly
  //if the input is high, set the output pin high
  //if the input is low, set the output pin low
}
void loop() {
  onlineControl();
}
// int setDriveControl(String inputString){
  
// }
