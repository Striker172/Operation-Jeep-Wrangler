// Include Particle Device OS APIs
#include "Particle.h"
#define xValPin A1
#define yValPin A0
int centerX = 0;
int centerY = 0;
 int xValRaw = 0;
 int controlX = 2047;
 int controlY = 2047;
 int yValRaw = 0;
 #define Remote 1;
 #define Website 0;
#define hornSwitchPin D0
#define hornSelectorPin1 D1
#define hornSelectorPin2 D2
#define hornSelectorPin3 D3
int driveControl = 0;
unsigned long int samplePositionTimer;
unsigned long int sampleHornSwitch;
struct HornSwitch
{
    int indentify;
    int value;
};

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);
// void sendDatatoCloud(int controlX, int controlY); //This method probably doesn't matter
// setup() runs once, when the device is first turned on
int hornSelector; HornSwitch hornSelectorPins[] = {{hornSelectorPin1,1},{hornSelectorPin2,2},{hornSelectorPin3,4}};
bool disableHorn = false;
void HornInput(const char *event, const char *data){
    String input  = String(data);
    if(input.indexOf("ON") > -1){
        disableHorn = true;
    }
    else {
        disableHorn = false;
    }
}
void DriveControl(const char *event, const char *data){
    String Line = String(data);
    if(Line.indexOf("Remote") > -1){
        driveControl = Remote;
    }
    else if(Line.indexOf("Website") > -1){
        driveControl = Website;
        Particle.unsubscribe(); // unsubscribes from all the events but add one to make sure it knows to switch back to remote control
        Particle.subscribe("DriveControl(R/W):",DriveControl,"24001f001147313134383335");
    }
}
void setup() {
 pinMode(xValPin, INPUT);
 pinMode(yValPin, INPUT);
 pinMode(hornSwitchPin, INPUT_PULLDOWN);
 for(int i = 0; i < 3; i++){
    pinMode(hornSelectorPins[i].indentify, INPUT_PULLDOWN);
 }
 Serial.begin(9600);
 samplePositionTimer = millis()+500;
 sampleHornSwitch = millis()+750;
 Particle.subscribe("Song(O/F):",HornInput,"24001f001147313134383335");
 Particle.subscribe("DriveControl(R/W):",DriveControl,"24001f001147313134383335"); //This is the event that will be used to switch between remote and website control
 //Gets the center of the joystick
 for(int i = 0; i < 50; i++){
     centerX += analogRead(xValPin);
     centerY += analogRead(yValPin);
 }
    centerX = centerX/50;
    centerY = centerY/50;
}
// void sendDatatoCloud(int controlX, int controlY){
//     Particle.publish("ControlValues(x,y): ", String(controlX)+ String(",")+ String(controlY));
// }

// loop() runs over and over again, as quickly as it can execute.
void loop() {
    unsigned long int currentTime = millis();
    if(driveControl == 1) {
        //Splits the joystick into 4 quadrants, and maps the values to -255 to 255
    if(currentTime > samplePositionTimer){ 
        //Splits the joystick into 4 quadrants, and maps the values to -255 to 255
    xValRaw = (analogRead(xValPin)-centerX);
    yValRaw = (analogRead(yValPin)-centerY);
    controlX = map(xValRaw, -centerX, centerX, -255, 255);
    controlY = map(yValRaw, -centerY, centerY, -255, 255);
    // Serial.printlnf("X: %d, Y: %d, controlX: %d, controlY: %d", xValRaw, yValRaw, controlX, controlY);
    Particle.publish("ControlValues(x,y): ", String(controlX)+ String(",")+ String(controlY));
    samplePositionTimer += 500;
    }
    if(currentTime > sampleHornSwitch){
        hornSelector = 0;
        String statement = "";
        if(digitalRead(hornSwitchPin)== HIGH && disableHorn == true){
            statement = "OFF"; //This turns off the horn
        }
        if(digitalRead(hornSwitchPin) == HIGH && disableHorn == false){
            for(int i =0; i < 3;i++){
                 if(digitalRead(hornSelectorPins[i].indentify) == HIGH){
                    hornSelector += hornSelectorPins[i].value;
                 }
            }
            switch(hornSelector){
                case 0:
                    statement = "ON"; //Normal horn sound
                    break;
                case 1:
                    statement = "TUNE:11"; //RickRoll
                    break;
                case 2:
                    statement = "TUNE:0"; //Mario short
                    break;
                case 3:
                    statement = "TUNE:1"; //Star wars short
                    break;
                case 4:
                    statement = "TUNE:2"; //pink panther short
                    break;
                case 5:
                    statement = "TUNE:3"; // Pink Panther Long
                    break;
                case 6:
                    statement = "TUNE:4"; //Star Wars
                    break;
                case 7:
                    statement = "TUNE:8"; //Lion sleeps tonight
                    break;
            }
        }
        Particle.publish("Song(O/F):", statement);
        sampleHornSwitch += 750;
        //Maybe add a button that will switch between remote and website control on the actual controller.
    }
    }
    //I will add a button that switchs from off, half lights, and full lights for the car.
    //To change the color of the lights, that will be controlled via the website for obvisous reasons.
}
