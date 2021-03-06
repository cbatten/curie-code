//========================================================================
// Piezo
//========================================================================
// This class provides the playTone() and playNote() functions for
// playing sound with a piezo element.

#ifndef PIEZO_H
#define PIEZO_H

void playTone(int pin_piezo, int time_high, int duration);
void playNote(int pin_piezo, char note, int duration);

#endif /* PIEZO_H */

