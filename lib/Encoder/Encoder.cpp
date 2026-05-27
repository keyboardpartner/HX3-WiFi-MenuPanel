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
// Class Lib for KBP Encoder with rotary encoder and 3 buttons, C. Meyer 1/2026

#include "Encoder.h"
#include <inttypes.h>
#include "Arduino.h"


Encoder::Encoder(uint8_t pin_enter, uint8_t pin_up, uint8_t pin_down, uint8_t pin_enc_a, uint8_t pin_enc_b) {
  _pin_enter = pin_enter;
  _pin_up = pin_up;
  _pin_down = pin_down;
  _pin_enc_a = pin_enc_a;
  _pin_enc_b = pin_enc_b;
  pinMode(_pin_enter, INPUT);
  pinMode(_pin_up, INPUT);
  pinMode(_pin_down, INPUT);
  pinMode(_pin_enc_a, INPUT);
  pinMode(_pin_enc_b, INPUT);
  _encoderAction = dummyEncoderAction; // Moved action callback auf default setzen, damit er nicht ins Leere läuft, falls er nicht definiert ist
  _buttonAction = dummyButtonAction;
  _btnState_old = (digitalRead(_pin_enter) << 0) | (digitalRead(_pin_up) << 1) | (digitalRead(_pin_down) << 2);
  _lastState = (digitalRead(_pin_enc_b) << 1) | digitalRead(_pin_enc_a);
  _encoderMillis = millis();
}


/********** high level commands, for the user! */

// #############################################################################
//
//     ######  #     # ####### ####### ####### #     #  #####
//     #     # #     #    #       #    #     # ##    # #     #
//     #     # #     #    #       #    #     # # #   # #
//     ######  #     #    #       #    #     # #  #  #  #####
//     #     # #     #    #       #    #     # #   # #       #
//     #     # #     #    #       #    #     # #    ## #     #
//     ######   #####     #       #    ####### #     #  #####
//
// #############################################################################

// read a byte from PCA9555 port 1 (buttons)
// benötigt etwa 130 µs (inkl. I2C Overhead) bei 400 kHz
uint8_t Encoder::checkButtons() {
  uint8_t buttons;
  buttons = (digitalRead(_pin_enter) << 0) | (digitalRead(_pin_up) << 1) | (digitalRead(_pin_down) << 2);
  buttons = ~buttons & 0b00000111; // Invertieren, da 0 = gedrückt, und nur die unteren 3 Bits sind Tasten
  if (_btnState_old != buttons) {
    _btnState_old = buttons;
    if (_was_autorepeat_timeout) {
      _buttonAction(buttons | 0x80); // Nur 3 Bits sind Tasten, Invertieren, da 0 = gedrückt
    } else {
      _buttonAction(buttons); // Nur 3 Bits sind Tasten, Invertieren, da 0 = gedrückt
    }
    delay(20); // Kurze Entprell-Verzögerung, um Mehrfachauslösungen zu vermeiden
  }
  return buttons; // Nur 3 Bits sind Tasten
}

uint8_t Encoder::getButtons() {
  uint8_t buttons;
  buttons = (digitalRead(_pin_enter) << 0) | (digitalRead(_pin_up) << 1) | (digitalRead(_pin_down) << 2);
  buttons = ~buttons & 0b00000111; // Invertieren, da 0 = gedrückt, und nur die unteren 3 Bits sind Tasten
  return buttons; // Nur 3 Bits sind Tasten
}

uint8_t Encoder::waitReleased(uint32_t timeout_ms) {
  uint8_t buttons;
  uint32_t now_ms = millis();
  uint32_t timeout_time = now_ms + timeout_ms;
  buttons = getButtons();
  while ((buttons != 0) && ((now_ms < timeout_time) || (timeout_ms == 0))) {
    buttons = getButtons();
    now_ms = millis();
    delay(20); 
  }; // Warten bis alle Tasten losgelassen sind oder Timeout
  _btnState_old = 0;
  _was_autorepeat_timeout = (now_ms >= timeout_time); // Wenn noch Tasten gedrückt sind, wurde der Timeout erreicht
  return buttons; // enthält bei Timeout die noch gedrückten Tasten, ansonsten 0
}

uint8_t Encoder::waitPressed(uint32_t timeout_ms) {
  uint8_t buttons;
  uint32_t now_ms = millis();
  uint32_t timeout_time = now_ms + timeout_ms;
  do {
    buttons = getButtons();
    now_ms = millis();
    delay(20); 
  } while ((buttons == 0) && ((now_ms < timeout_time) || (timeout_ms == 0))); // Warten bis alle Tasten losgelassen sind oder Timeout
  _btnState_old = buttons;
  _was_autorepeat_timeout = (now_ms >= timeout_time); // Wenn noch Tasten gedrückt sind, wurde der Timeout erreicht
  return buttons; // enthält bei Timeout die noch gedrückten Tasten, ansonsten 0
}

void Encoder::checkButtonsAutorepeat(uint32_t initial_timeout_ms, uint32_t repeat_timeout_ms, uint32_t enter_timeout_ms) {
  uint8_t buttons = checkButtons();
  _was_autorepeat_timeout = true;
  if(buttons == 0) {
    _autorepeat_timeout = initial_timeout_ms;
    _was_autorepeat_timeout = false;
  } else if (buttons == 1) {
    // Enter-Taste ist gedrückt
    waitReleased(enter_timeout_ms); 
  } else {
    waitReleased(_autorepeat_timeout); // Warten, bis die Taste losgelassen wird, um Mehrfachaktionen zu vermeiden
    _autorepeat_timeout = repeat_timeout_ms; // Verkürzung des Timeouts, um schneller auf erneute Aktionen reagieren zu können
  }
}

// #############################################################################
//
//     ####### #     #  #####  ####### ######  ####### ######
//     #       ##    # #     # #     # #     # #       #     #
//     #       # #   # #       #     # #     # #       #     #
//     #####   #  #  # #       #     # #     # #####   ######
//     #       #   # # #       #     # #     # #       #   #
//     #       #    ## #     # #     # #     # #       #    #
//     ####### #     #  #####  ####### ######  ####### #     #
//
// #############################################################################

/*
// https://github.com/lazlyhu/RotaryEncoderAccel/tree/master

const int8_t c_knobdir[] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
 -1,  0,  0,  1,
  0,  1, -1,  0 };

#define LATCHSTATE 3
*/

int32_t Encoder::getEncoderPosition() {
  int32_t position;
  noInterrupts(); // Interrupts deaktivieren, um konsistenten Zugriff auf die Encoder-Position zu gewährleisten
  position = _encoderPosition;
  interrupts(); // Interrupts wieder aktivieren
  return position;
}

void Encoder::encoderISR() {
  // muss regelmäßig oder über Timer-Interrupt (max. 2ms) aufgerufen werden, 
  // um die Encoder-Position zu aktualisieren
  int16_t delta = 0;
  uint8_t currentState = (digitalRead(_pin_enc_b) << 1) | digitalRead(_pin_enc_a); // Nur die beiden relevanten Bits lesen und nach rechts verschieben
  if (currentState != _lastState) {
    // Zustandsänderung erkannt, nur ganze Schritte zählen
    if ((_lastState == 0b00 && currentState == 0b10)) {
      delta = 1; // Vorwärts
    } else if ((_lastState == 0b00  && currentState == 0b01)) {
      delta = -1; // Rückwärts
    }
    _lastState = currentState;
  }
  // eine einfache Beschleunigungsfunktion, die den Delta-Wert erhöht,
  // wenn die Encoderänderungen schnell hintereinander auftreten.
  if (delta != 0) {
    uint32_t now = millis();
    uint32_t diff = now - _encoderMillis;
    if (diff < 80) { // Wenn die letzte Änderung weniger als 80 ms her ist, erhöhen wir den Delta-Wert
      if (diff < 40) { // Wenn die letzte Änderung weniger als 40 ms her ist, erhöhen wir den Delta-Wert weiter
        delta *= _accell; // Beschleunigungseffekt: Verfünffache den Delta-Wert
      } else {
        delta *= _accell_half; // Beschleunigungseffekt: Verdopple den Delta-Wert
      }
    }
    _encoderPosition += delta; 
    _encoderMillis = now;
  }
}

int16_t Encoder::getEncoderDelta() {
  int16_t delta = 0;
  if (_encoderPosition != _encoderPosition_old) {
    delta = _encoderPosition - _encoderPosition_old;
    _encoderPosition_old = _encoderPosition;
  }
  return delta;
}

void Encoder::checkEncoder(int16_t accell) {
  int16_t delta = 0;
  _accell = accell; // Beschleunigungsfaktor aktualisieren, z.B. basierend auf der aktuellen Menüebene oder anderen Kriterien
  _accell_half = _accell / 2;
   if (_accell_half < 1) _accell_half = 1; // Sicherstellen, dass der halbe Beschleunigungsfaktor mindestens 1 ist, um eine Verlangsamung zu vermeiden
   if (_accell < 1) _accell = 1; // Sicherstellen, dass der Beschleunigungsfaktor mindestens 1 ist, um eine Verlangsamung zu vermeiden
   if (_encoderPosition != _encoderPosition_old) {
    delta = _encoderPosition - _encoderPosition_old;
    _encoderPosition_old = _encoderPosition;
    _encoderAction(delta); // Callback mit Delta-Wert aufrufen
  }
  if (_encoderPosition != _encoderPosition_old) {
    delta = _encoderPosition - _encoderPosition_old;
    _encoderPosition_old = _encoderPosition;
    _encoderAction(delta); // Callback mit Delta-Wert aufrufen
  }
}

