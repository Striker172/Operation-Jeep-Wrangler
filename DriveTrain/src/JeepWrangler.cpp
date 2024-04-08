
#include "Particle.h"
#include "string.h"
#include "Music_Speaker.h"
#define SpeakerPin A0  
#define forwardPin D4
#define backwardPin D3
#define speakerPin D2
class MotorControl {
  public:
  struct pins {
    String name;
    int powerPin;
    int speedCap;
  };
  MotorControl(String name, int powerPin) {
    pins.name = name;
    pins.powerPin = powerPin;
    pins.speedCap = 50; // based on percentage so a range from 0-100%
    //Zero being no speed, 100 being full speed, 50 being half speed
    //Or the otherway around, depends on the person making the drive code.
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
int selectedSong = 0; //This is the song that is selected to play
Music_Speaker speaker(speakerPin);
bool previvousState = speaker.tuneIsOn;
MotorControl Motors[2] = {
  MotorControl("Left", D0),
  MotorControl("Right", D1)
}; //Use analogWrite to output the values to the pins, and use the speedCap to limit the speed of the motors
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
    //I may have this vary on the position controls, idk yet
    tone(speakerPin, 500,500);
  }
  else if(inputString.indexOf("OFF") > -1){
    //Untested Code
    noTone(speakerPin);
    speaker.tuneIsOn = false;
    speaker.thisNote = 0;
  } else if(inputString.indexOf("TUNE:") > -1){
    inputString = inputString.substring(inputString.indexOf(":")+1); // This gets the number between the interrupt pin and :
    selectedSong = inputString.toInt();
    Particle.publish("Song(O/F):","true"); //This allows for interrupts to happen
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
  Particle.publish("DriveControl:","Remote");
  Particle.function("Set_Drive_Speed",setDriveSpeed);
}

int setDriveSpeed(String inputString){
 //This will set the cap for the speed of the motors, based on percentage
 int input = inputString.toInt();
  if(input > 100 || input < 0){
    return -1;
  }else{
    for(int i = 0; i < 2; i++){
      Motors[i].pins.speedCap = input;
    }
    return 2;
  }
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
  if(speaker.tuneIsOn){
    speaker.playSong(selectedSong); 
    previvousState = !speaker.tuneIsOn;
  } else if(previvousState == true && speaker.tuneIsOn == false){
    noTone(speakerPin);
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
