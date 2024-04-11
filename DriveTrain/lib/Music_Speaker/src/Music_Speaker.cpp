/* Music_Speaker library by Striker
 */

#include "Music_Speaker.h"
extern int pinkPantherShort[];
extern int superMarioShort[];
extern int darthVaderShort[];
extern int pinkPantherFull[];
extern int superMarioFull[];
extern int darthVaderFull[];
extern int happybirthday[];
extern int odetojoy[];
extern int lionsleeps[];
extern int brahmslullaby[];
extern int rickRoll[];
int Music_Speaker::thisNote = 0;
int Music_Speaker::noteDuration = 0;
/**
 * @brief Constructs a Music_Speaker object with the specified speaker pin.
 * 
 * @param speakerPin The pin number to which the speaker is connected.
 */
//This is the constructor for the Music_Speaker object
Music_Speaker::Music_Speaker(int speakerPin)
{
    this->speakerPin = speakerPin;
    pinMode(speakerPin, OUTPUT);
    SongInfo songIndex[] = {
        {0, sizeof(superMarioShort) / sizeof(superMarioShort[0]) / 2, 190, (60000 * 4) / 190,superMarioShort},
        {1, sizeof(darthVaderShort) / sizeof(darthVaderShort[0]) / 2, 170, (60000 * 4) / 170,darthVaderShort},
        {2, sizeof(pinkPantherShort) / sizeof(pinkPantherShort[0]) / 2, 170, (60000 * 4) / 170,pinkPantherShort},
        {3, sizeof(pinkPantherFull) / sizeof(pinkPantherFull[0]) / 2, 190, (60000 * 4) / 190,pinkPantherFull},
        {4, sizeof(superMarioFull) / sizeof(superMarioFull[0]) / 2, 190, (60000 * 4) / 190,superMarioFull},
        {5, sizeof(darthVaderFull) / sizeof(darthVaderFull[0]) / 2, 170, (60000 * 4) / 170,darthVaderFull},
        {6, sizeof(happybirthday) / sizeof(happybirthday[0]) / 2, 140, (60000 * 4) / 140,happybirthday},
        {7, sizeof(odetojoy) / sizeof(odetojoy[0]) / 2, 114, (60000 * 4) / 114,odetojoy},
        {8, sizeof(lionsleeps) / sizeof(lionsleeps[0]) / 2, 122, (60000 * 4) / 122,lionsleeps},
        {9, sizeof(brahmslullaby) / sizeof(brahmslullaby[0]) / 2, 76, (60000 * 4) / 76,brahmslullaby},
        {10, sizeof(rickRoll) / sizeof(rickRoll[0]) / 2, 114, (60000 * 4) / 114,rickRoll}
    };
    for(int i = 0; i < 11; i++){
        this->songIndex[i] = songIndex[i];
    }
    thisNote = 0;
     tuneIsOn = false;
    noteIsOn = false;
}

//Not my code, but I have modified it to work with my project. Original code can be found at https://github.com/robsoncouto/arduino-songs/issues/33
void Music_Speaker::playSong(int song){
    if (thisNote < songIndex[song].notes * 2) { //  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    if (!noteIsOn) { // calculates the duration of each note
       //array is twice the number of notes (notes + durations)
       divider = songIndex[song].songArray[thisNote + 1];
      if (divider > 0) {
        noteDuration = (songIndex[song].wholenote) / divider; 
      }else if (divider < 0) { // dotted notes are represented with negative durations
          noteDuration = (songIndex[song].wholenote) / abs(divider);
          noteDuration *= 1.5; // increases the duration in half for dotted notes
        }
      noteIsOn = true;
      elapsedNote = 0;
    } //end of calculation of note duration

    if (elapsedNote == 0) { //start to play note
    tone(speakerPin, songIndex[song].songArray[thisNote]);
      elapsedNote = millis();
    }else{ // Wait for the specified duration before playing the next note
      if (millis() - elapsedNote >= noteDuration) { 
        noTone(speakerPin); // stop the waveform generation before the next note
        noteIsOn = false;
        thisNote += 2;
      }
    }
  }else{ // tune has ended because thisNote > notes * 2
    tuneIsOn = false; 
    thisNote = 0;
    noTone(speakerPin);
}
}

