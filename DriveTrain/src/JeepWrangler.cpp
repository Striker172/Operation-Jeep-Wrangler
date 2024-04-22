//Nessesary Libraries for the code 
#include "neopixel.h"
#include "Particle.h"
#include "Music_Speaker.h"
//The pin layout for the Jeep Wrangler 
#define PixelPin D0
#define SpeakerPin A1
#define speakerPin D1
#define backwardPin D5
#define forwardPin D4
#define RightSideMotor D2
#define LeftSideMotor D3
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
//Global variables
int maxPower = 100;
int driveValue[2]; //(x,y) values for the drive train can be removed later, idk depends on how we want to do it
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
  for(int i = 0; i < 2; i++){
    pinMode(Motors[i].pins.powerPin, OUTPUT);
  }
  Serial.begin(9600);
  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  Particle.subscribe("ControlValues(x,y)", setDriveControlXY,"2f0028001547313137363331");
  Particle.subscribe("HornSwitch:",HornSwitch,"2f0028001547313137363331");
  Particle.subscribe("LightLevels:",setLEDBrightness,"2f0028001547313137363331");
  //configure for website control as well.
  Particle.function("setDriveControl(R/W)", setDriveControl);
  Particle.function("SetDriveSpeed",setDriveSpeed);
  Particle.function("SetLEDColor(R,G,B,Bright)",setLEDColor);
  Particle.publish("DriveControl:","Remote");
  strip.begin();
  RGBValues[0] = 255;
  RGBValues[1] = 255;
  RGBValues[2] = 255;
  RGBValues[3] = 100;
}


void Drive(){
  //Stop
  driveValue[0] = driveValue[0] * maxPower/255;
  driveValue[1] = driveValue[1] * maxPower/255;
  if(driveValue[0] == 0 && driveValue[1] == 0){
    analogWrite(Motors[0].pins.powerPin, 0);
    analogWrite(Motors[1].pins.powerPin, 0);
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, LOW);
  }
  else if(driveValue[1] >= 0){
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, LOW);
    if(driveValue[0] < 0){
      //Right 
      analogWrite(Motors[0].pins.powerPin, driveValue[0]);
      analogWrite(Motors[1].pins.powerPin, abs(driveValue[1]));
    } else if(driveValue[0] > 0){
      //Left
      analogWrite(Motors[0].pins.powerPin, driveValue[1]);
      analogWrite(Motors[1].pins.powerPin, driveValue[0]);
    }else if (driveValue[0] == 0){
      //Forward
      analogWrite(Motors[0].pins.powerPin, driveValue[1]);
      analogWrite(Motors[1].pins.powerPin, driveValue[1]);
    }
    //Backward
  } else if(driveValue[1] <= 0){
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
    if(driveValue[0] <= 0){
      //Right
      analogWrite(Motors[0].pins.powerPin, abs(driveValue[0]));
      analogWrite(Motors[1].pins.powerPin, abs(driveValue[1]));
    } else if(driveValue[0] > 0){
      analogWrite(Motors[0].pins.powerPin, driveValue[1]);
      analogWrite(Motors[1].pins.powerPin, abs(driveValue[0]));
    } else if (driveValue[0] == 0){
      analogWrite(Motors[0].pins.powerPin, abs(driveValue[1]));
      analogWrite(Motors[1].pins.powerPin, abs(driveValue[1]));
    }
  }

}
unsigned long PosTimer = millis() + 1000;
void loop() {
  // onlineControl();
  if(driveControl == 0){
    if(millis() > PosTimer){
      Drive();
      PosTimer = millis() + 1000;
    }
  }
  else if(driveControl == 1){
    Drive();
  }
  //The code for the player to work, it will only activiate if the somebody presses the button.
  if(speaker.tuneIsOn){
    speaker.playSong(selectedSong); 
    previvousState = !speaker.tuneIsOn;
  } else if(previvousState == true && speaker.tuneIsOn == false){
    if(selectedSong == 12){
      RGBValues[0] = 0;
      RGBValues[1] = 50;
      RGBValues[2] = 255;
      RGBValues[3] = 100;
    }
    noTone(speakerPin);
    //This will publish for disabling input, however, it will still allow for interrupts to happen unless we change the code.
    Particle.publish("Song(O/F):","false");
    previvousState = speaker.tuneIsOn;
  }


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
void HornSwitch(const char *event, const char *data){
  String inputString = String(data);
  if(inputString.indexOf("ON") > -1){
    beep(1000);
    speaker.thisNote = 0;
    speaker.tuneIsOn= false;
  }
  else if(inputString.indexOf("OFF") > -1){
    noTone(speakerPin);
    speaker.tuneIsOn = false;
    speaker.thisNote = 0;
  } else if(inputString.indexOf("TUNE:") > -1){
    speaker.thisNote = 0;
    inputString = inputString.substring(inputString.indexOf(":")+1); // This gets the number between the interrupt pin and :
    selectedSong = inputString.toInt()-1;
    Serial.println(selectedSong);
    Particle.publish("Song(O/F):","true"); //This allows for interrupts to happen
    speaker.tuneIsOn = !speaker.tuneIsOn;
  }
}
//Untested Code
int setDriveSpeed(String inputString){
 int input = inputString.toInt();
  if(input > 100 || input < 0){
    //if the person goes over 100 or under 0, it will return -1 and set the maxPower to half speed
    maxPower = 125;
    return -1;
  }else{
   maxPower = map(input, 0, 100, 0, 255); //map the input to the range of the motor speed
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
