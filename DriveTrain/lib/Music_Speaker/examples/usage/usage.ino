// Example usage for Music_Speaker library by Striker.

#include "Music_Speaker.h"

// Initialize objects from the lib
Music_Speaker music_Speaker;

void setup() {
    // Call functions on initialized library objects that require hardware
    music_Speaker.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    music_Speaker.process();
}
