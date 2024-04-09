#pragma once

/* Music_Speaker library by Striker
 */

// This will load the definition for common Particle variable types
#ifndef Music_Speaker_h
#define Music_Speaker_h
#include "Particle.h"
#include "tunes.h"
// This is your main class that users will import into their application
class Music_Speaker
{
public:
static int thisNote, noteDuration;
  static byte activeTune;
  bool tuneIsOn, noteIsOn;
  Music_Speaker(int speakerPin);
  void playSong(int song);
private:
ulong elapsedNote;
struct SongInfo {
        int songNumber;
        int notes;
        int tempo;
        int wholenote;
        int* songArray;
    };
SongInfo songIndex[11];
int divider = 0;
int speakerPin;
};
#endif
