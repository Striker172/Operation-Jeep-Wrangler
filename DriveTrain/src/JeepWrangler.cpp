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
    int inputPin;
    int outputPin;
    int enablePin;
  };
  MotorControl(String name, int inputPin, int outputPin, int enablePin);
  struct pins pins;
};
// Include Particle Device OS APIs


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);
static int controlType = 0;
const int switchPin = D6;
// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);
int setDriveControl(String inputString);
MotorControl Motors[2] = {
  MotorControl("Front", D0, D1, D2),
  MotorControl("Back", D3, D4, D5)
};
MotorControl::MotorControl(String name,int inputPin, int outputPin, int enablePin){
    pins.name = name;
    pins.inputPin = inputPin;
    pins.outputPin = outputPin;
    pins.enablePin = enablePin;
  }
// setup() runs once, when the device is first turned on
void setup() {
  // Put initialization like pinMode and begin functions here
  for(int i = 0; i < 2; i++){
    pinMode(Motors[i].pins.inputPin, INPUT);
    pinMode(Motors[i].pins.outputPin, OUTPUT);
    pinMode(Motors[i].pins.enablePin, OUTPUT);
  }
  pinMode(switchPin, INPUT);
  digitalWrite(Motors[0].pins.enablePin, HIGH);
  digitalWrite(Motors[1].pins.enablePin, HIGH);
  Particle.function("cF_setDriveControl", setDriveControl);
}

void onlineControl(){
  //read the input from the cloud and set the output pins accordingly
  //if the input is high, set the output pin high
  //if the input is low, set the output pin low
}
void loop() {
  onlineControl();
}
int setDriveControl(String inputString){
  
}
