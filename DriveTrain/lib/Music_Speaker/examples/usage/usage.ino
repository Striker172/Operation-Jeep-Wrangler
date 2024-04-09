// Example usage for Music_Speaker library by Striker.

#include "Music_Speaker.h"
#include "Particle.h"
#define speakerPin D2
// Initialize objects from the lib
Music_Speaker music_Speaker(speakerPin);

void setup() {
}

void loop() {
    // Use the library's initialized objects and functions
   if(music_Speaker.tuneIsOn){
       music_Speaker.playSong(0);
   }
}
