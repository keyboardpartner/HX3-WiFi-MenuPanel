/*
// #############################################################################
//       __ ________  _____  ____  ___   ___  ___
//      / //_/ __/\ \/ / _ )/ __ \/ _ | / _ \/ _ \
//     / ,< / _/   \  / _  / /_/ / __ |/ , _/ // /
//    /_/|_/___/_  /_/____/\____/_/_|_/_/|_/____/
//      / _ \/ _ | / _ \/_  __/ |/ / __/ _ \
//     / ___/ __ |/ , _/ / / /    / _// , _/
//    /_/  /_/ |_/_/|_| /_/ /_/|_/___/_/|_|
//
// #############################################################################
*/
// Class Lib for KBP Encoder, C. Meyer 1/2026
// Based on work from DFROBOT

#ifndef HX3_ENCODER_H
#define HX3_ENCODER_H

#include <Arduino.h>
#include <inttypes.h>

class Encoder {
public:

  Encoder(uint8_t pin_enter, uint8_t pin_up, uint8_t pin_down, uint8_t pin_enc_a, uint8_t pin_enc_b);

  typedef void (*actionEncCallback)(int16_t delta);
  void setEncoderCallback(actionEncCallback action) { _encoderAction = action; } // Callback for wait loops, e.g. for button press handling, to avoid blocking calls
  
  typedef void (*actionBtnCallback)(uint8_t button);
  void setButtonCallback(actionBtnCallback action)  { _buttonAction = action; } // Callback for wait loops, e.g. for button press handling, to avoid blocking calls
  
  // #############################################################################
  
  // Encoder-Buttons und Encoder-Drucktaster lesen, 
  // Rückgabe als Bitmaske, 0 = keine Taste gedrückt, 1 = Enter gedrückt, 2 = Up gedrückt, 4 = Down gedrückt:
  uint8_t checkButtons(); 
  uint8_t getButtons(); 
  uint8_t waitReleased(uint32_t timeout_ms); // as above, wait for release of all buttons
  uint8_t waitPressed(uint32_t timeout_ms); // as above, wait for press of any button
  void checkButtonsAutorepeat(uint32_t initial_timeout_ms, uint32_t repeat_timeout_ms, uint32_t enter_timeout_ms);
 
  // absolute Position des Encoders, in der Regel ist getEncoderDelta für Menühandling ausreichend:
  int32_t getEncoderPosition(); // ohne Callback
  int16_t getEncoderDelta(); // Delta seit letzem Aufruf, ohne Callback
  void checkEncoder(int16_t accell); // ruft bei Encoder-Änderungen Callback auf, sonst wie getEncoderDelta
  
  // WICHTIG: regelmäßig oder in einem Timer-Interrupt aufrufen, um Encoder-Position zu aktualisieren, z.B. alle 2 ms
  void encoderISR(); 
  
  // #############################################################################

private:
  static void dummyEncoderAction(int16_t delta) { };
  static void dummyButtonAction(uint8_t button) { };
  actionEncCallback _encoderAction; // Callback for encoder movement handling, to avoid blocking calls
  actionBtnCallback _buttonAction;  // Callback for button press handling, to avoid blocking calls
  uint8_t _pin_enter, _pin_up, _pin_down, _pin_enc_a, _pin_enc_b;
  uint8_t _btnState_enter = 0;
  uint8_t _btnState_up = 0;
  uint8_t _btnState_down = 0;
  uint8_t _btnState_old = 0; // vorheriger Zustand der Buttons, um Änderungen zu erkennen
  uint32_t _autorepeat_timeout = 800; // Zeit in ms, nach der bei gedrückter Taste eine Wiederholung ausgelöst wird
  bool _was_autorepeat_timeout = false; // Flag, um zu verfolgen, ob der Autorepeat-Timeout erreicht wurde
  // Encolder-Handling: Position und Zeit der letzten Änderung, um Beschleunigungseffekt zu ermöglichen
  volatile uint8_t _lastState = 3; // Initialer Zustand der Encoder-Bits
  volatile int32_t _encoderPosition = 0;
  volatile uint32_t _encoderMillis = 0;
  int32_t _encoderPosition_old = 0;
  int16_t _accell = 1;
  int16_t _accell_half = 1;
};

#endif
