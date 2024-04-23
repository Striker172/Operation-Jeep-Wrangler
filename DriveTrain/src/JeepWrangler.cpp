//Nessesary Libraries for the code 
#include "neopixel.h"
#include "Particle.h"
#include "Music_Speaker.h"
//The pin layout for the Jeep Wrangler
#define SpeakerPin A0  
#define PixelPin D0
#define speakerPin D1
#define backwardPin D2
#define forwardPin D3
#define RightSideMotor D4
#define LeftSideMotor D5
//Variables for the ILed
#define PIXEL_COUNT 2
#define PIXEL_TYPE WS2812

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
  private:
};


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);
//Objects for the libraries
SerialLogHandler logHandler(LOG_LEVEL_INFO);
Music_Speaker speaker(speakerPin);
Adafruit_NeoPixel strip(PIXEL_COUNT, PixelPin, PIXEL_TYPE);
//Functions
//Set the max speed of the motors, it's based on percentage
int setDriveSpeed(String inputString);
//Sets the drive control aka gets the x and y values from the controller or website?
void setDriveControlXY(const char *event, const char *data);
//This gets the input from the horn switch and sets the speaker to beep, and sets the song to play
void HornSwitch(const char *event, const char *data);
//This will set the drive control to either remote or website and will make a beep sound
int setDriveControl(String inputString);
//This will beep the speaker
void beep(int frequency);
//Sets the color of the lights on the car *Only for the website!
int setLEDColor(String inputString);
//Sets the brightness of the LED lights *Only for the controller
void setLEDBrightness(const char *event, const char *data);
//This will show the color of the LED lights
void showColor(int brightnessValue);
int setCloudDriveControlXY(String inputString);
//How the car will get horn input from the website
int HornSwitchWeb(String inputString);
//Global variables
int maxPower = 100;
int static driveValue[2]; //(x,y) values for the drive train can be removed later, idk depends on how we want to do it
int driveControl = 0; //0 for remote control, 1 for website control
int* driveControlPtr = &driveControl; // a pointer to the driveControl variable, where it can switch between remote and website control
int selectedSong = 0; //This is the song that is selected to play
bool previvousState = speaker.tuneIsOn;
int RGBValues[4]; //The varibles for the RGB values and the brightness
//Objects for the motors
MotorControl Motors[2] = {
  MotorControl("Left", LeftSideMotor),
  MotorControl("Right", RightSideMotor)
}; //Use analogWrite to output the values to the pins, and use the speedCap to limit the speed of the motors

void setup() {
  // Put initialization like pinMode and begin functions here
  driveValue[0] = 0;
  driveValue[1] = 0;

  Serial.begin(9600);
  pinMode(RightSideMotor, OUTPUT);
  pinMode(LeftSideMotor, OUTPUT);
  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  Particle.subscribe("ControlValues(x,y):", setDriveControlXY,"2f0028001547313137363331");
  Particle.subscribe("HornSwitch:",HornSwitch,"2f0028001547313137363331");
  Particle.subscribe("LightLevels:",setLEDBrightness,"2f0028001547313137363331");
  //configure for website control as well.
  Particle.function("SetCloudDriveControl", setCloudDriveControlXY);
  Particle.function("setDriveControl(R/W)", setDriveControl);
  Particle.function("SetDriveSpeed",setDriveSpeed);
  Particle.function("SetLEDColor",setLEDColor);
  Particle.function("HornSwitchWeb",HornSwitchWeb);
  Particle.publish("DriveControl:","Remote");
  strip.begin();

  RGBValues[0] = 255;
  RGBValues[1] = 255;
  RGBValues[2] = 255;
  RGBValues[3] = 100;
}



unsigned long PosTimer = millis() + 500;
void loop() {
  // onlineControl();
  if(driveControl == 0){
    if(millis() > PosTimer){
       //Stop
  int driveDirection = 0;
  driveValue[0] = driveValue[0] * maxPower / 255;
  driveValue[1] = driveValue[1] * maxPower / 255;
if(driveValue[0] == 0 && driveValue[1] == 0){
  driveDirection = 100;
   digitalWrite(backwardPin, HIGH);
    digitalWrite(forwardPin, HIGH);
}
else if(driveValue[1] >= 0){
  digitalWrite(backwardPin, LOW);
  digitalWrite(forwardPin, HIGH);
  if ( driveValue[0] == 0 || driveValue[0] > 0){
    driveDirection = 0;
  }
  else if(driveValue[0] < 0){
    // Right 
    driveDirection = 3;
  } else if(driveValue[0] > 0){
    // Left
   driveDirection = 2;
  }
} else if(driveValue[1] < 0){
  digitalWrite(backwardPin, HIGH);
  digitalWrite(forwardPin, LOW);
  if (driveValue[0] ==0  || driveValue[0] < 0){
    // Backward
    driveDirection = 0;
  }
  else if(driveValue[0] <= 0){
    // Right
   driveDirection = 3;
  } else if(driveValue[0] > 0){
    driveDirection = 2;
  }
}
Serial.println(driveDirection);
switch(driveDirection){
  case 0:
  //forward
  analogWrite(Motors[0].pins.powerPin, abs(driveValue[1]));
  analogWrite(Motors[1].pins.powerPin, abs(driveValue[1]));
  break;
  case 2:
  //left
  analogWrite(Motors[0].pins.powerPin, abs(driveValue[1]));
  analogWrite(Motors[1].pins.powerPin, abs(driveValue[0]));
  break;
  case 3:
  //right
  analogWrite(Motors[0].pins.powerPin, abs(driveValue[0]));
  analogWrite(Motors[1].pins.powerPin, abs(driveValue[1]));
  break;
  default:
  analogWrite(Motors[0].pins.powerPin, 0);
  analogWrite(Motors[1].pins.powerPin, 0);
  break;
}
      PosTimer = millis() + 500;
    }
      
    }
  else if(driveControl == 1){
  }
  //The code for the player to work, it will only activiate if the somebody presses the button.
  if(speaker.tuneIsOn){
    speaker.playSong(selectedSong); 
    previvousState = !speaker.tuneIsOn;
  } else if(previvousState == true && speaker.tuneIsOn == false){
    noTone(speakerPin);
    //This will publish for disabling input, however, it will still allow for interrupts to happen unless we change the code.
    Particle.publish("Song(O/F):","false");
    previvousState = speaker.tuneIsOn;
  }

  //Motor Control
  if(driveValue[0] > 0){
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, LOW);
  } else if(driveValue[0] < 0){
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
  } else {
    analogWrite(Motors[0].pins.powerPin, 0);
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, LOW);
  }

  if(driveValue[1] > 0){
    digitalWrite(LeftSideMotor, HIGH);
    digitalWrite(RightSideMotor, LOW);
  } else if(driveValue[1] < 0){
    digitalWrite(RightSideMotor, HIGH);
    digitalWrite(LeftSideMotor, LOW);
  } else {
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, LOW);
  }
  delay(100);

}
//Untested code
void beep(int frequency){
  //The frequency does nothing, it's just a placeholder for the frequency of the beep if the event that the car isn't moving
  if(driveValue[0] != 0){
     frequency = map(driveValue[0], -255, 255, 0, 10000); //map the frequency to the range of the speaker with how hard the joystick is pushed
  }
  tone(speakerPin, frequency, 500);
}
int setDriveControl(String inputString){
  if(inputString.toLowerCase().compareTo("remote") == 0|| inputString.toLowerCase().compareTo("r") == 0){
    *driveControlPtr = 0;
   tone(speakerPin, 1000, 500);
    Particle.publish("DriveControl(R/W):","Remote");
    return 0;
  }
  else if(inputString.toLowerCase().compareTo("website") == 0|| inputString.toLowerCase().compareTo("w") == 0){
    *driveControlPtr = 1;
    Particle.publish("DriveControl(R/W):","Website");
    tone(speakerPin, 2500, 500);
    return 1;
  }
  return -1;
}
void setDriveControlXY(const char *event, const char *data){
  if(strlen(data) == 0){
    return;
  }else{
  driveValue[0] = String(data).substring(0, String(data).indexOf(",")).toInt(); //get the x value
  driveValue[1] = String(data).substring(String(data).indexOf(",")+1).toInt(); //get the y value
  if(driveValue[0] > 255){
    driveValue[0] = 255;
  } else if(driveValue[0] < -255){
    driveValue[0] = -255;
  }
  if(driveValue[1] > 255){
   driveValue[1]= 255;
  } else if(driveValue[1] < -255){
    driveValue[1] = -255;
  }
}
}
void HornSwitch(const char *event, const char *data){
  String inputString = String(data);
  if(inputString.indexOf("ON") > -1){
    beep(1000);
  }
  else if(inputString.indexOf("OFF") > -1){
    noTone(speakerPin);
    speaker.tuneIsOn = false;
    speaker.thisNote = 0;
  } else if(inputString.indexOf("TUNE:") > -1){
    inputString = inputString.substring(inputString.indexOf(":")+1); // This gets the number between the interrupt pin and :
    selectedSong = inputString.toInt()-1;
    Particle.publish("Song(O/F):","true"); //This allows for interrupts to happen
    speaker.tuneIsOn = !speaker.tuneIsOn;
  }
}
int HornSwitchWeb(String inputString){
  HornSwitch(0, inputString);
  return 1;
  }
//Untested Code
int setDriveSpeed(String inputString){
 int input = inputString.toInt();
  if(input > 100 || input < 0){
    //if the person goes over 100 or under 0, it will return -1 and set the maxPower to half speed
    maxPower = 125;
    return -1;
  }else{
    maxPower = map(input, 0, 100, 0, 255);
    return 2;
  }
}

int setLEDColor(String inputString){
  //The cloud function only for the website
  //The input string will be in the format of "R,G,B,Brightness"
  for(int i =0; i< 4;i++){
    RGBValues[i] = inputString.substring(0,inputString.indexOf(",")).toInt();
    inputString.remove(0,inputString.indexOf(",")+1);
  }
  if(RGBValues[3] > 100){
    RGBValues[3] %= 100;
  } else if(RGBValues[3] < 0){
    RGBValues[3] = abs(RGBValues[3]);
  }
  //100% is max brigthness, 0% is no brightness
  showColor(RGBValues[3]);
  return 1;
}

void setLEDBrightness(const char *event, const char *data){
  //Only for the controller, honestly we don't have to use this for the controller, it's kind of stupid
  RGBValues[3] = String(data).toInt();
 showColor(RGBValues[3]);
}
void showColor(int brightnessValue){
  //Map the brightness value to the range of the LED lights
  int brightness = map(RGBValues[3], 0, 100, 0,255);
  int color = strip.Color((brightness*RGBValues[1]/255), (brightness*RGBValues[0]/255), (brightness*RGBValues[2]/255));
  strip.setPixelColor(0,color);
  strip.setPixelColor(1,color);
  strip.show();
}
int setCloudDriveControlXY(String inputString){
  int xValue = inputString.substring(0, inputString.indexOf(",")).toInt(); //get the x value
  int yValue = inputString.substring(inputString.indexOf(",")+1).toInt(); //get the y value
  driveValue[0] = xValue;
  driveValue[1] = yValue;
  return 0;
}