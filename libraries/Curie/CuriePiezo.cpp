//========================================================================
// CuriePiezo
//========================================================================

#include <CuriePiezo.h>
#include <Arduino.h>

//------------------------------------------------------------------------
// playTone
//------------------------------------------------------------------------
// Play a tone by using a digital pin to generate a square wave.

void playTone(int pin_piezo, int time_high, int duration) {
  for (long i = 0; i < duration * 1000L; i += time_high * 2) {
    digitalWrite( pin_piezo, HIGH );
    delayMicroseconds(time_high);
    digitalWrite( pin_piezo, LOW );
    delayMicroseconds(time_high);
  }
}

//------------------------------------------------------------------------
// playNote
//------------------------------------------------------------------------
// Play a note by looking up its time_high (half the wave period)
// and playing the tone.

void playNote(int pin_piezo, char note, int duration) {
  char names[] = { 'C', 'D', 'E', 'F', 'G', 'A', 'B', 'c', 'd', 'e', 'f', 'g', 'a', 'b' };
  int time_high[] = { 3817, 3401, 3030, 2857, 2551, 2273, 2024, 1915, 1700, 1519, 1432, 1275, 1136, 1014 };

  // Play the tone corresponding to the note name.

  for (int i = 0; i < 16; i++) {
    if (names[i] == note) {
      playTone(pin_piezo, time_high[i], duration);
    }
  }

  // If the note is a rest, send no data.

  if (note == ' ') {
    delay( duration );
  }

}


