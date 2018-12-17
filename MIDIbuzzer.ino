#include <MIDI.h>
#include "pitches.h"
#define BUZZER 12

int pitches[36] = {
  NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3,
  NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
  NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5
  };
  
int keysPushed[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int polyphony = 0;
int frequency = 10;
bool shiftElements = false;
bool noteOffReceived = false;

MIDI_CREATE_DEFAULT_INSTANCE();

void playNote(byte pitch)
{
  if (pitch <= 83 and pitch >= 48) {
    tone(BUZZER, pitches[pitch - 48]);
  }
}

void arpeggio(int frequency, int keysPushed[]) {
  while (true){
    for (int i = 0; i < polyphony; i++) {
      playNote(keysPushed[i]);
      delay(1000/frequency);
      noTone(BUZZER);
    }
    MIDI.read();
    if (noteOffReceived) {
      break;
    }
  }
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  polyphony += 1;
  noteOffReceived = false;
  keysPushed[polyphony - 1] = pitch;
  
  if (polyphony == 1) {
    playNote(pitch);
  }
  else if (polyphony > 1) {
    arpeggio(frequency, keysPushed);
  }
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  polyphony -= 1;
  shiftElements = false;
  noteOffReceived = true;
  
  if (polyphony == 0) {
    noTone(BUZZER);
  }
  else {
    for (int i = 0; i <= polyphony; i++) {
      if (shiftElements) {
        keysPushed[i-1] = keysPushed[i];
      }
      if (keysPushed[i] == pitch) {
        keysPushed[i] = 0;
        shiftElements = true;
      }
    }
    arpeggio(frequency, keysPushed);
  }
}

// -----------------------------------------------------------------------------

void setup()
{
  // Connect the handleNoteOn function to the library,
  // so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function

  // Do the same for NoteOffs
  MIDI.setHandleNoteOff(handleNoteOff);

  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  pinMode(BUZZER, OUTPUT);

  // device ready indicator
  for (int i = 0; i < 3; i++) {
    tone(BUZZER, pitches[12], 200);
    delay(400);
  }
}

void loop()
{
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();

  // There is no need to check if there are messages incoming
  // if they are bound to a Callback function.
  // The attached method will be called automatically
  // when the corresponding message has been received.
}
