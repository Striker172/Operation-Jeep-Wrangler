// Include Particle Device OS APIs
#include "Particle.h"
#define xValPin A0
#define yValPin A1
#define Remote 0
#define Website 1
#define hornSelectorPin1 D0
#define hornSelectorPin2 D1
#define hornSelectorPin3 D2
#define ControlSwitch D3
#define hornSwitchPin D4
#define lightSwitch D5
//Structs
struct HornSwitch{
    int indentify;
    int value;
};
//Variables
int centerX = 0;
int centerY = 0;
int controlX = 2047;
int controlY = 2047;
int driveControl = 0;
int previvousButtonState = LOW;
int brightness = 0;
bool Lockout = false;
int hornSelector;
bool disableHorn = false;

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);
//Arrays 
unsigned long int Timers[3] = {0,0,0};//PostionTimer, HornTimer, LightTimer
//1: RickRoll, 2: Doom,3: DarthVader Short,4: Pink Panther Short,5: Pink Panther Long,6: Super Mario Long,7: Lion sleeps tonight
String hornSongs[] = {"ON","Tune:11","Tune:11","Tune:1","Tune:2","Tune:3","Tune:4","Tune:8"};
int previvousxyValues[2];
HornSwitch hornSelectorPins[] = {{hornSelectorPin1,1},{hornSelectorPin2,2},{hornSelectorPin3,4}};
void HornInput(const char *event, const char *data){
    String input  = String(data);
    if(input.indexOf("ON") > -1){
        disableHorn = true;
    }
    else {
        disableHorn = false;
    }
}
//Sets the control of the car to either remote or website
//With the addition of a lockout that doesn't allow the controller to flip between the two
//This is to prevent the controller from flipping between the two when the user is trying to control the car. The decision is controlled via the particle dashboard.
void DriveControl(const char *event, const char *data){
    String Line = String(data).toLowerCase();
    if(Line.indexOf("remote") > -1 || Line.indexOf("r") > -1){
        driveControl = Remote;
        Particle.publish("Control: ", "Remote");
        Lockout = false;
    }
    else if(Line.indexOf("website") > -1 || Line.indexOf("w") > -1){
        driveControl = Website;
        Particle.publish("Control: ", "Website");
        Particle.unsubscribe(); // unsubscribes from all the events but add one to make sure it knows to switch back to remote control
        Particle.subscribe("DriveControl(R/W):",DriveControl,"24001f001147313134383335");
        Lockout = true;
    }
}
void findCenter(){
    for(int i = 0; i < 100; i++){
        centerX += analogRead(xValPin);
        centerY += analogRead(yValPin);
    }
    centerX = centerX/100;
    centerY = centerY/100;
}
void setup() {
 pinMode(xValPin, INPUT);
 pinMode(yValPin, INPUT);
 pinMode(hornSwitchPin, INPUT_PULLDOWN);
 pinMode(lightSwitch, INPUT_PULLDOWN);
 pinMode(ControlSwitch, INPUT_PULLDOWN);
 pinMode(D7, OUTPUT);
 for(int i = 0; i < 3; i++){
    pinMode(hornSelectorPins[i].indentify, INPUT_PULLDOWN);
    Timers[i] = millis()+250;
 }
 Serial.begin(9600);
 driveControl = Remote;
 Particle.subscribe("Song(O/F):",HornInput,"24001f001147313134383335");
 Particle.subscribe("DriveControl(R/W):",DriveControl,"24001f001147313134383335"); //This is the event that will be used to switch between remote and website control
 //Gets the center of the joystick
 findCenter();
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    unsigned long int currentTime = millis();
        //Splits the joystick into 4 quadrants, and maps the values to -255 to 255
        // Serial.println("SamplePositionTimer: "+ String(samplePositionTimer) + " CurrentTime: "+ String(currentTime));
    if(Lockout == false){
        if(digitalRead(ControlSwitch) == LOW){
            driveControl = Remote;
        } else if(digitalRead(ControlSwitch) == HIGH){
            driveControl = Website;
        }
        }
    if(driveControl == 0){
        digitalWrite(D7, HIGH);
        driveControl = Remote;
        
        if(currentTime >Timers[0]){ 
        //Splits the joystick into 4 quadrants, and maps the values to -255 to 255
        controlX = map(analogRead(xValPin), -centerX, centerX, -255, 255)-255;
            controlY = map(analogRead(yValPin), -centerY, centerY, -255, 255) -255;
            
                Particle.publish("ControlValues(x,y): ", String(controlX)+ String(",")+ String(controlY));
                previvousxyValues[0] = controlX;
                previvousxyValues[1] = controlY;
    Timers[0] += 500;
    }
    if(currentTime > Timers[1]){
        hornSelector = 0;
        String statement = "";
        //Untested code 
        if(digitalRead(hornSwitchPin)== HIGH && disableHorn == true){
            statement = "OFF"; //This turns off the horn
            Particle.publish("Song(O/F):", statement);
        }
        if(digitalRead(hornSwitchPin) == HIGH && disableHorn == false){
            for(int i =0; i < 3;i++){
                 if(digitalRead(hornSelectorPins[i].indentify) == HIGH){
                    hornSelector += hornSelectorPins[i].value;
                 }
            }
            //I could change it into an array with a struct, but I don't think it's necessary
            statement = hornSongs[hornSelector];
            Particle.publish("Song(O/F):", statement);
        }
        Timers[1] += 250;
        //Maybe add a button that will switch between remote and website control on the actual controller.
    }
    //Untested code 
    if(currentTime > Timers[2]){
        int buttonState = digitalRead(lightSwitch);
        if(buttonState == HIGH && previvousButtonState == LOW){
            if(brightness > 100){
                brightness = 0;
            }
            Particle.publish("LightLevels:", String(brightness));
            previvousButtonState = HIGH;
            brightness += 25;
        }
        else if(buttonState == LOW){
           previvousButtonState = LOW;
        }
        Timers[2] += 250;
    }
    //I will add a button that switchs from off, half lights, and full lights for the car.
    //To change the color of the lights, that will be controlled via the website for obvisous reasons.
}else if(driveControl == 1){
    digitalWrite(D7, LOW);
    //This is the code that will be used to control the car via the website
} else{
    if(millis() > Timers[0]){
        digitalWrite(D7, LOW);
        Timers[0] += 250;
    }else{
        digitalWrite(D7, HIGH);
    }
}
}
