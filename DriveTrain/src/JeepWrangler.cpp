/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */
#include "Particle.h"
#include "string.h"
#include "Music_Speaker.h"
#define SpeakerPin A0  
#define forwardPin D4
#define backwardPin D3
#define speakerPin D2
//#include "ezButton.h" This is just a library that simplifies the use of buttons and handles debouncing for you and such. We can add it if need be
class MotorControl {
  public:
  struct pins {
    String name;
    int powerPin;
  };
  MotorControl(String name, int powerPin) {
    pins.name = name;
    pins.powerPin = powerPin;
  }
  struct pins pins;
  void setSpeed(int speed){
    this->speed = speed;
  }
  private:
  int speed = 0;
};
// Include Particle Device OS APIs


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);
// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);
int setDriveControl(String inputString);
int driveValue[2]; //(x,y) values for the drive train can be removed later, idk depends on how we want to do it
int driveControl = 0; //0 for remote control, 1 for website control
int* driveControlPtr = &driveControl; // a pointer to the driveControl variable, where it can switch between remote and website control
int selectedSong = 0; 
Music_Speaker speaker(speakerPin);
bool previvousState = speaker.tuneIsOn;
MotorControl Motors[2] = {
  MotorControl("Left", D0),
  MotorControl("Right", D1)
};
// setup() runs once, when the device is first turned on

void setDriveControlXY(const char *event, const char *data){
  String inputString = String(data);
  int xValue = inputString.substring(0, inputString.indexOf(",")).toInt(); //get the x value
  int yValue = inputString.substring(inputString.indexOf(",")+1).toInt(); //get the y value
  driveValue[0] = xValue;
  driveValue[1] = yValue;
}
void HornSwitch(const char *event, const char *data){
  String inputString = String(data);
  if(inputString.indexOf("ON") > -1){
    tone(speakerPin, 500,500);
  }
  else if(inputString.indexOf("OFF") > -1){
    noTone(speakerPin);
  } else if(inputString.indexOf("TUNE:") > -1){
    //Untested Code
    inputString = inputString.substring(inputString.indexOf(":")+1);
    selectedSong = inputString.toInt();
     Serial.println("Song Selected: " + inputString);
    Particle.publish("Song(O/F):","true"); //This event should lock out input from the buttons for the horn.
    speaker.tuneIsOn = !speaker.tuneIsOn;
  }
}
void setup() {
  // Put initialization like pinMode and begin functions here
  for(int i = 0; i < 2; i++){
    pinMode(Motors[i].pins.powerPin, OUTPUT);
  }
  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  Particle.subscribe("ControlValues(x,y)", setDriveControlXY,"2f0028001547313137363331");
  Particle.subscribe("HornSwitch:",HornSwitch,"2f0028001547313137363331");
  //configure for website control as well.
  Particle.function("setDriveControl", setDriveControl);
}

void onlineControl(){
  //read the input from the cloud and set the output pins accordingly
  //if the input is high, set the output pin high
  //if the input is low, set the output pin low
}

void loop() {
  // onlineControl();
  if(driveControl == 0){
    //remote control
    //read the input from the remote and set the output pins accordingly
    //if the input is high, set the output pin high
    //if the input is low, set the output pin low
  }
  else if(driveControl == 1){
    //website control
    //read the input from the cloud and set the output pins accordingly
    //if the input is high, set the output pin high
    //if the input is low, set the output pin low
  }
   //Untested Code
  if(speaker.tuneIsOn){
    speaker.playSong(selectedSong); 
    previvousState = !speaker.tuneIsOn;
  } else if(previvousState == true && speaker.tuneIsOn == false){
    //Untested Code
    noTone(speakerPin);
    Serial.println("Song is done");
    Particle.publish("Song(O/F):","false");
    previvousState = speaker.tuneIsOn;
  }
}
void beep(int frequency){
  tone(speakerPin, frequency,500);
}
int setDriveControl(String inputString){
  if(inputString.compareTo("remote") == 0){
    *driveControlPtr = 0;
    beep(1000);
    Particle.publish("DriveControl","Remote");
    return 0;
  }
  else if(inputString.compareTo("website") == 0){
    *driveControlPtr = 1;
    Particle.publish("DriveControl","Website");
    beep(2500);
    return 1;
  }
  return -1;
}
