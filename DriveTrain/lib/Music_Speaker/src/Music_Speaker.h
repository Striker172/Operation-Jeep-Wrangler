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
 int divider = 0;
  static byte activeTune;
  int speakerPin;
  bool tuneIsOn, noteIsOn;
  ulong elapsedNote;
 struct SongInfo {
        int songNumber;
        int notes;
        int tempo;
        int wholenote;
        int* songArray;
    };
  Music_Speaker(int speakerPin);
  void playSong(int song);
  SongInfo songIndex[11];
private:
};
#endif
