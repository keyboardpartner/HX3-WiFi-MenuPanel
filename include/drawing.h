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

// https://doc-tft-espi.readthedocs.io/tft_espi/methods/fillrectgradient/
// https://rgbcolorpicker.com/565


#ifndef Drawing_h
#define Drawing_h

#include <Arduino.h>
#include "global_vars.h"
#include "Free_Fonts.h"
#include <LittleFS.h>

// TODO: make these constants settable
#define LISTBOX_TOP 5
#define LISTBOX_SUB_TOP 43
#define LISTBOX_LEFT 5
#define LISTBOX_W 160
#define LISTBOX_H 160

#define LISTENTRY_COUNT 4
#define LISTENTRY_W 150 // 10 Pixel padding on each Seite
#define LISTENTRY_H 28
#define LISTFIELD_W 150 // 10 Pixel padding on each side
#define LISTFIELD_H (LISTENTRY_COUNT * LISTENTRY_H) // Höhe aller Listenfelder zusammen
#define LISTENTRY_PADDING 5

#define VALUEBOX_TOP 5
#define VALUEBOX_LEFT 180

#define VALUEBOX_W 130
#define VALUEBOX_H 70
#define VALUEBOX_CENTER_X (VALUEBOX_W/2)
#define VALUEBOX_CENTER_Y (VALUEBOX_H/2)

#define ORGANBOX_TOP 85
#define ORGANBOX_LEFT 180
#define ORGANBOX_W 130
#define ORGANBOX_H 80


// msgType =0/16 "i" in blau, =1/17 "?" in blau, =2/18 "!" in rot
enum DialogBoxType {
	DB_INFO = 0,
	DB_REQUEST = 1,
	DB_ERROR = 2,

  DB_INFO_OK = 8,
  DB_REQUEST_OK = 9,
  DB_ERROR_OK = 10,
};
                                                      
#define MSG_WIDTH 250
#define MSG_HEIGHT 70
#define MSG_WIDTH_2 MSG_WIDTH/2 // halbe Breite
#define MSG_HEIGHT_2 MSG_HEIGHT/2 // halbe Höhe

#define WIDEMSG_WIDTH 280
#define WIDEMSG_HEIGHT 80
#define WIDEMSG_WIDTH_2 WIDEMSG_WIDTH/2 // halbe Breite
#define WIDEMSG_HEIGHT_2 WIDEMSG_HEIGHT/2 // halbe Höhe

typedef struct {
  char str1[12];
  char str2[12];
} textValueStringType;

const textValueStringType organModelStrings[] PROGMEM = {
  {"B3", "Std"},  // 0
  {"B3", "Old"},  // 1
  {"B3", "Recpd"},  // 2
  {"M100", "M3"},  // 3
  {"H100", "12 Drb"},  // 4
  {"Boehm ", "2000"},  // 5
  {"Boehm", "CnT/L"},  // 6
  {"Wersi", "Space"},  // 7
  {"Wersi", "Sacral"},  // 8
  {"Farfisa", "Combo"},  // 9
  {"Vox", "Conti"},  // 10
  {"Conn", "Church"},  // 11
  {"Custom", "Organ 1"},  // 12
  {"Custom", "Organ 2"},  // 13
  {"Custom", "Organ 3"},  // 14
  {"Custom", "Organ 4"}   // 15
};

const textValueStringType speakerModelStrings[] PROGMEM = {
  {"122 Std", "Sm Room"},  // 0
  {"122 Std", "Lg Room"},  // 1
  {"122 Old", "Sm Room"},  // 2
  {"122 Old", "Lg Room"},  // 3
  {"147 New", "Sm Room"},  // 4
  {"147 New", "Lg Room"},  // 5
  {"760 Std", "Sm Room"},  // 6
  {"760 Std", "Lg Room"},  // 7
  {"Space", "Sound"},  // 8
  {"Sharma", "2001"},  // 9
  {"Fender", "Vibratone"},  // 10
  {"Dynacord", "D-100"},  // 11
  {"Dynacord", "CLS222"},  // 12
  {"Custom", "Spkr 1"},  // 13
  {"Custom", "Spkr 2"},  // 14
  {"Custom", "Spkr 3"},  // 15
  {"Custom", "Spkr 4"}   // 16
}; 

const textValueStringType taperingStrings[] PROGMEM = {
  {"Year", "1955"},    // 0 Cap Sets, Tapering B3
  {"Year", "1961"},    // 1
  {"Year", "1972"},    // 2
  {"Recapped", "Generator"},    // 3 B3 aggressiv
  {"Straight", "Linear"},    // 4 linear, kein Tapering
  {"Twangy", "Combo"}   // 5 aggressiv, kein Tapering
};

const textValueStringType wavesetStrings[] PROGMEM = {
  {"B3/M100", "25% k2"},    // 0 neue B3
  {"B3/M100", "28% k2"},    // 1
  {"B3/M100", "32% k2"},    // 2
  {"B3/M100", "38% k2"},    // 3 alte B3
  {"Pure", "Sine"},    // 4 Reiner Sinus, Böhm mit Sinus-Zusatz
  {"Sawtooth", "Filtered"},   // 5 Sägezahn gefiltert
  {"LC Gen", "Sine"},   // 6 Sinus LC-Generator
  {"TOS Gen", "SquSine"},  // 7 Sinus aus Rechteck-Filterung
};

const char vibknobLettering[6][3] = {
  "V1", "C1", "V2", "C2", "V3", "C3"
};

const textValueStringType gatingModeStrings[] PROGMEM = {
  {"9", "Contacts"},    // 0 B3
  {"12", "Contacts"},    // 1 H100
  {"Env Gen", "ADSR"},    // 2
  {"Env Gen", "Perc DBs"},  
  {"Env Gen", "Timebend"},  
};

const textValueStringType wifiModeStrings[] PROGMEM = {
  {"OFF", ""},    // no Wifi
  {"Router", "STA Mode"},    // STA mode
  {"Access Pt", "AP Mode"},    // AP mode
};

const textValueStringType midiCCstrings[] PROGMEM = {
  {"NI B4", ""},         // 0
  {"Hammond", "XK Series"},    // 1 
  {"Hammond", "SK Series"},    // 2
  {"Versatile", "or Boehm"},   // 3
  {"Nord", "C1/C2"},    // 4
  {"Voce", "Drawbar"},  // 5
  {"DLQ", "KeyB Duo"},  // 6
  {"Crumar", "Hamichord"},    // 7
  {"HX3", "Native"},    // 8
  {"Custom", "Set 1"},     // 9
  {"Custom", "Set 2"},     // 10
};

const textValueStringType phrModeStrings[] PROGMEM = {
  {"Phasing", "Vib 1"},    // 0
  {"Phasing", "Vib 2"},    // 1 
  {"Celeste", "Normal"},   // 2
  {"Celeste", "Weak"},     // 3
  {"Ensemble", "Normal"},  // 4
  {"Ensemble", "Weak"},    // 5
  {"Rotor", "Normal"},     // 6
  {"Rotor", "Weak"},       // 7
};

void getMenuEntry(menuEntryType* entry, uint16_t index);

bool dimmedMainWindow = false; // global variable to track if the main window is dimmed
uint16_t gradientStartColor = TFT_DIALOGGREY;
uint16_t gradientEndColor = 0x39a7;
uint16_t borderColor = TFT_BORDER;

void setMainDimmedState(bool dimmed) {
  dimmedMainWindow = dimmed;
  gradientStartColor = TFT_DIALOGGREY;
  gradientEndColor = 0x39a7;
  borderColor = TFT_BORDER;
  if (dimmedMainWindow) {
    gradientStartColor = tft.alphaBlend(160, gradientStartColor, TFT_BLACK); // Apply a dimming effect to the entire screen
    gradientEndColor = tft.alphaBlend(160, gradientEndColor, TFT_BLACK); // Apply a dimming effect to the entire screen
    borderColor = tft.alphaBlend(160, borderColor, TFT_BLACK); // Apply a dimming effect to the entire screen
  }
}


// #############################################################################
//
//     ######  ######     #    #     #    ######  #     # ######  
//     #     # #     #   # #   #  #  #    #     # ##   ## #     # 
//     #     # #     #  #   #  #  #  #    #     # # # # # #     # 
//     #     # ######  #     # #  #  #    ######  #  #  # ######  
//     #     # #   #   ####### #  #  #    #     # #     # #       
//     #     # #    #  #     # #  #  #    #     # #     # #       
//     ######  #     # #     #  ## ##     ######  #     # #                                                                     
//                                                                
// #############################################################################

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Bodmer's BMP image rendering function

void drawBMP(const char *filename, int16_t x, int16_t y) {
  if ((x >= tft.width()) || (y >= tft.height())) return;

  fs::File bmpFS;
  // Open requested file on SD card
  bmpFS = LittleFS.open(filename, "r");
  if (!bmpFS) {
    Serial.print("File not found");
    return;
  }
  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;
  uint32_t startTime = millis();
  if (read16(bmpFS) == 0x4D42) {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);
    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) {
      y += h - 1;
      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);
      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];
      for (row = 0; row < h; row++) {
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16-bit colours
        for (uint16_t col = 0; col < w; col++)  {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }
        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
      Serial.print("BMP loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

// #############################################################################
//
//     ######  ######     #    #     #    #     #    #    #       
//     #     # #     #   # #   #  #  #    #     #   # #   #       
//     #     # #     #  #   #  #  #  #    #     #  #   #  #       
//     #     # ######  #     # #  #  #    #     # #     # #       
//     #     # #   #   ####### #  #  #     #   #  ####### #       
//     #     # #    #  #     # #  #  #      # #   #     # #       
//     ######  #     # #     #  ## ##        #    #     # ####### 
//                                                                
// #############################################################################


// Zeige einen Text in der Valuebox an
// canvas must be assigned and created before calling this function
void drawTextValue(const char* text) {
  canvas.drawString(text, VALUEBOX_CENTER_X, VALUEBOX_CENTER_Y - 2);
}

// Zeige einen zweizeiligen Text in der Valuebox an
// canvas must be assigned and created before calling this function
void drawTextValue2(const char* text1, const char* text2) {
  if (strlen(text2) == 0) {
    drawTextValue(text1); // Nur 1 Zeile
    return;
  }
  canvas.setFreeFont(FF22);
  canvas.drawString(text1, VALUEBOX_CENTER_X, VALUEBOX_CENTER_Y - 16);
  canvas.drawString(text2, VALUEBOX_CENTER_X, VALUEBOX_CENTER_Y + 14);
}

// Zeige Preset-Nummer und -Text in der Valuebox an
// canvas must be assigned and created before calling this function
void drawPreset(const uint16_t preset, const char* text) {
  canvas.drawNumber(preset, VALUEBOX_CENTER_X, VALUEBOX_CENTER_Y - 15);
  canvas.setFreeFont(FF21);
  canvas.drawString(text, VALUEBOX_CENTER_X, VALUEBOX_CENTER_Y + 15);
}

// Zeige einen numerischen Wert in der Valuebox an
// canvas must be assigned and created before calling this function
void drawNumeric(const int16_t value) {
  uint32_t text_width = canvas.textWidth("888") + 2; // Breite von 3 Ziffern in der Schriftart FF24 
  canvas.drawNumber(value, VALUEBOX_CENTER_X, VALUEBOX_CENTER_Y - 2);
}

// Zeige einen binären Wert (ON/OFF) in der Valuebox an
// canvas must be assigned and created before calling this function
void drawBinary(const int16_t value) {
  if (value == 0) {
    drawTextValue("OFF");
  } else {
    drawTextValue("ON");
  }
}

// Zeige den Status des Rotary Speed in der Valuebox an (SLOW, STOP, FAST)
// canvas must be assigned and created before calling this function
void drawRotarySpeed(const int16_t value) {
  switch (value) {
  case 0:
    drawTextValue("SLOW");
    break;
  case 1:
    drawTextValue("STOP");
    break;
  case 2:
    drawTextValue("FAST");
    break;
  }
}

// Zeige einen Wert als Potentiometer-Arc in der Valuebox an
// canvas must be assigned and created before calling this function
void drawPot(const int16_t value, const int16_t max, uint16_t arc_color) {
  uint32_t text_width = canvas.textWidth("888") + 2; // Breite von 3 Ziffern in der Schriftart FF23 
  int32_t endAngle = map(value, 0, max, 45, 315);
  canvas.drawNumber(value, VALUEBOX_CENTER_X - 33, VALUEBOX_CENTER_Y - 2);
  canvas.drawArc(VALUEBOX_CENTER_X + 28, VALUEBOX_CENTER_Y, 28, 23, endAngle, 315, TFT_CHARCOAL, TFT_CHARCOAL); // Draw the arc
  canvas.drawArc(VALUEBOX_CENTER_X + 28, VALUEBOX_CENTER_Y, 28, 23, 45, endAngle, arc_color, TFT_CHARCOAL); // Draw the arc
}

// Zeige einen Wert als VibKnob in der Valuebox an
// canvas must be assigned and created before calling this function
void drawVibKnob(const int16_t value, const int16_t max, uint16_t arc_color) {
  uint32_t text_width = canvas.textWidth("C3") + 2; // Breite von 2 Buchstaben in der Schriftart FF23 
  canvas.drawString(vibknobLettering[value], VALUEBOX_CENTER_X - 33, VALUEBOX_CENTER_Y - 2);
  int32_t angle_step = 360 / (max + 1); // Winkel pro Markierung
  // draw arc segment as knob pointer
  for (int i = 0; i <= max; i++) {
    int32_t markAngle = (288 + i * angle_step) % 360; // Winkel der aktuellen Markierung
    if (value == i) {
      canvas.drawArc(VALUEBOX_CENTER_X + 28, VALUEBOX_CENTER_Y, 28, 23, markAngle, (markAngle + 24) % 360, arc_color, TFT_CHARCOAL); // Draw the active mark
    } else {
      canvas.drawArc(VALUEBOX_CENTER_X + 28, VALUEBOX_CENTER_Y, 27, 23, markAngle, (markAngle + 24) % 360, TFT_CHARCOAL, TFT_CHARCOAL); // Draw the inactive mark
    }
  }
  canvas.drawCircle(VALUEBOX_CENTER_X + 28, VALUEBOX_CENTER_Y, 28, TFT_CHARCOAL); // Draw the outer circle of the knob
  canvas.drawCircle(VALUEBOX_CENTER_X + 28, VALUEBOX_CENTER_Y, 29, TFT_CHARCOAL); // Draw the outer circle of the knob
}

// Zeige einen Wert als horizontalen Fortschrittsbalken in der Valuebox an
// canvas must be assigned and created before calling this function
void drawProgress(const int16_t value, const int16_t max, uint16_t bar_color) {
  int32_t bar_length = map(value, 0, max, 0, VALUEBOX_W - 22);
  uint16_t text_width = canvas.textWidth("888") + 2; // Breite von 3 Ziffern in der Schriftart FF23 
  uint16_t bar_gradient = canvas.alphaBlend(140, bar_color, TFT_BLACK);
  canvas.fillRectVGradient(11, VALUEBOX_CENTER_Y + 10, bar_length, 18, bar_color, bar_gradient);
  canvas.fillRectVGradient(11 + bar_length, VALUEBOX_CENTER_Y + 10, VALUEBOX_W - 22 - bar_length, 18, TFT_CHARCOAL, TFT_BLACK);
  canvas.drawRect(10, VALUEBOX_CENTER_Y + 9, VALUEBOX_W - 20, 20, borderColor);
  canvas.drawNumber(value, VALUEBOX_CENTER_X, VALUEBOX_CENTER_Y - 15);
}


// #############################################################################

// Draws a value from currentMenuEntry in the value box. If in_valuechange is true, the value is highlighted to indicate it can be edited.
void drawValue(menuEntryType* entry, bool in_valuechange) {
  uint16_t text_color = TFT_BLACK;
  if (in_valuechange && (!dimmedMainWindow)) {
    text_color = TFT_EDITCOLOR;
  }
  canvas.createSprite(VALUEBOX_W, VALUEBOX_H);
  canvas.drawRect(0, 0, VALUEBOX_W, VALUEBOX_H, borderColor);
  // canvas.fillRectVGradient(VALUEBOX_LEFT + 1, VALUEBOX_TOP + 1, VALUEBOX_W - 2, VALUEBOX_H - 2, gradientStartColor, gradientEndColor);
  canvas.fillRectVGradient(1, 1, VALUEBOX_W - 2, VALUEBOX_H - 2, gradientStartColor, gradientEndColor);
  canvas.setTextDatum(MC_DATUM); // middle center text datum
  canvas.setTextColor(text_color);
  canvas.setFreeFont(FF23);

  int32_t value = hx3EditArray[entry->editArrayIdx];
  char buffer1[16], buffer2[12]; // 23 chars + null terminator
  switch (entry->displayType) {
  case tm_numeric:
    drawNumeric(value);
    break;
  case tm_pot:
    // als Bogen
    drawPot(value, entry->menuValueMax, text_color);
    break;
  case tm_vibknob:
    // als runder Drehschalter
    drawVibKnob(value, entry->menuValueMax, text_color);
    break;
  case tm_drawbar:
    // als Balken
    drawProgress(value, entry->menuValueMax, in_valuechange ? text_color : TFT_LIGHTGREY);
    break;
  case tm_tab:
    // als Ein/Aus
    drawBinary(value);
    break;
  case tm_button:
    // auch möglich, wenn keine Variable angegeben ist
    canvas.drawString("OK", VALUEBOX_CENTER_X, VALUEBOX_CENTER_Y - 15);
    canvas.setFreeFont(FF21);
    if (!dimmedMainWindow) canvas.setTextColor(TFT_RED);
    canvas.drawString("Press 2 sec", VALUEBOX_CENTER_X, VALUEBOX_CENTER_Y + 15);
    break;
  case tm_halfmoon:
    drawRotarySpeed(value);
    break;
  case tm_preset:
    memcpy(buffer1, &hx3EditArray[192], 16); // String aus HX3-Daten in buffer kopieren
    drawPreset(value, buffer1);
    break;      
  case tm_phrknob:
    strcpy_P(buffer1, phrModeStrings[value].str1); // String aus PROGMEM in buffer kopieren
    strcpy_P(buffer2, phrModeStrings[value].str2); // String aus PROGMEM in buffer kopieren
    drawTextValue2(buffer1, buffer2);
    break;      
  case tm_midicc:
    strcpy_P(buffer1, midiCCstrings[value].str1); // String aus PROGMEM in buffer kopieren
    strcpy_P(buffer2, midiCCstrings[value].str2); // String aus PROGMEM in buffer kopieren
    drawTextValue2(buffer1, buffer2);
    break;      
  case tm_organ:
    strcpy_P(buffer1, organModelStrings[value].str1); // String aus PROGMEM in buffer kopieren
    strcpy_P(buffer2, organModelStrings[value].str2); // String aus PROGMEM in buffer kopieren
    drawTextValue2(buffer1, buffer2);
    break;      
  case tm_speaker:
    strcpy_P(buffer1, speakerModelStrings[value].str1); // String aus PROGMEM in buffer kopieren
    strcpy_P(buffer2, speakerModelStrings[value].str2); // String aus PROGMEM in buffer kopieren
    drawTextValue2(buffer1, buffer2);
    break;      
  case tm_waveset:
    strcpy_P(buffer1, wavesetStrings[value].str1); // String aus PROGMEM in buffer kopieren
    strcpy_P(buffer2, wavesetStrings[value].str2); // String aus PROGMEM in buffer kopieren
    drawTextValue2(buffer1, buffer2);
    break;      
  case tm_tapering:
    strcpy_P(buffer1, taperingStrings[value].str1); // String aus PROGMEM in buffer kopieren
    strcpy_P(buffer2, taperingStrings[value].str2); // String aus PROGMEM in buffer kopieren
    drawTextValue2(buffer1, buffer2);
    break;      
  case tm_gating:
    strcpy_P(buffer1, gatingModeStrings[value].str1); // String aus PROGMEM in buffer kopieren
    strcpy_P(buffer2, gatingModeStrings[value].str2); // String aus PROGMEM in buffer kopieren
    drawTextValue2(buffer1, buffer2);
    break;      
  case tm_wifimode:
    strcpy_P(buffer1, wifiModeStrings[value].str1); // String aus PROGMEM in buffer kopieren
    strcpy_P(buffer2, wifiModeStrings[value].str2); // String aus PROGMEM in buffer kopieren
    drawTextValue2(buffer1, buffer2);
    break;      
  }
  canvas.pushSprite(VALUEBOX_LEFT, VALUEBOX_TOP);
  canvas.deleteSprite();
}

// #############################################################################
//
//     #     # #######  #####   #####     #     #####  ####### 
//     ##   ## #       #     # #     #   # #   #     # #       
//     # # # # #       #       #        #   #  #       #       
//     #  #  # #####    #####   #####  #     # #  #### #####   
//     #     # #             #       # ####### #     # #       
//     #     # #       #     # #     # #     # #     # #       
//     #     # #######  #####   #####  #     #  #####  ####### 
//                                                             
// #############################################################################

// Draw a dialog box with a message. Does not restore screen content.
// message1 is the main message, message2 is an optional secondary message
// msgType sets the icon type (and leaves space for buttons if needed)
void drawMsg(String message1, String message2, DialogBoxType msgType) {
  uint16_t center_x = DISPLAY_W / 2; 	// 160 Pixel
  uint16_t center_y = DISPLAY_H / 2; // 120 Pixel
  // spkrBeep(50);
  uint16_t x0 = center_x - MSG_WIDTH_2;
  uint16_t y0 = center_y - MSG_HEIGHT_2;
  // draw dialog/message box
  tft.drawRect(x0, y0, MSG_WIDTH, MSG_HEIGHT, TFT_WHITE);
  tft.drawRect(x0 + 1, y0 + 1, MSG_WIDTH - 2, MSG_HEIGHT - 2, TFT_WHITE);
  tft.fillRectVGradient(x0 + 2, y0 + 2, MSG_WIDTH - 4, MSG_HEIGHT - 4, TFT_DIALOGGREY, 0x39a7);
  // draw message text relative to center
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM); // middle center text datum
  if (message2.length() == 0) {
    tft.drawString(message1, center_x + 15, center_y);
  } else {
    tft.drawString(message1, center_x + 15, center_y - 12);
    tft.drawString(message2, center_x + 15, center_y + 12);
  }

  // display icon in the dialog box
  uint16_t msg_color;
  if (msgType == DB_ERROR || msgType == DB_ERROR_OK)
    msg_color = TFT_RED;
  else
    msg_color = TFT_BLUE;
  tft.fillRoundRect(center_x - 110, center_y - 18, 24, 36, 4, msg_color); // center_x - 120 +
  tft.setTextColor(TFT_WHITE, msg_color);

  tft.setFreeFont(FF22);
  center_x -= 100; // adjust center_x for icon position
  center_y -= 2;    // nudge up
  switch (msgType) {
  case DB_INFO:
  case DB_INFO_OK:
    tft.drawString("i", center_x, center_y);
    break;
  case DB_REQUEST:
  case DB_REQUEST_OK:
    tft.drawString("?", center_x, center_y);
    break;
  case DB_ERROR:
  case DB_ERROR_OK:
    tft.drawString("!", center_x, center_y);
    break;
  }
  tft.setTextFont(2);
  tft.setTextDatum(TL_DATUM); // middle center text datum
}

// Draw a dialog box with a message for a specified time
// message1 is the main message, message2 is an optional secondary message
// Screen content will be restored after the message box is displayed
void drawMsgTimeout(String message1, String message2, int duration, DialogBoxType msgType = DB_INFO) {
  // uint16_t x0 = DISPLAY_W / 2 - MSG_WIDTH_2;
  // uint16_t y0 = DISPLAY_H / 2 - MSG_HEIGHT_2;
  // uint16_t *screenbuf;
  // screenbuf = new uint16_t[MSG_WIDTH * MSG_HEIGHT]; // Create a screen buffer
  // tft.readRect(x0, y0, MSG_WIDTH, MSG_HEIGHT, screenbuf);
  drawMsg(message1, message2, msgType);
  delay(duration);
  // Restore screen content
  // tft.pushRect(x0, y0, MSG_WIDTH, MSG_HEIGHT, screenbuf);
  // delete[] screenbuf; // Free the screen buffer memory
}

// #############################################################################
//
//     ####### #     # ####### ######  #     #    ######  ####### #     # 
//     #       ##    #    #    #     #  #   #     #     # #     #  #   #  
//     #       # #   #    #    #     #   # #      #     # #     #   # #   
//     #####   #  #  #    #    ######     #       ######  #     #    #    
//     #       #   # #    #    #   #      #       #     # #     #   # #   
//     #       #    ##    #    #    #     #       #     # #     #  #   #  
//     ####### #     #    #    #     #    #       ######  ####### #     # 
//                                                                        
// #############################################################################

void drawEnterNumber(String message1, int16_t *number, int16_t minValue = 0, int16_t maxValue = 127 ) {
  uint16_t center_x = DISPLAY_W / 2; 	// 160 Pixel
  uint16_t center_y = DISPLAY_H / 2; // 120 Pixel
  // spkrBeep(50);
  uint16_t x0 = center_x - MSG_WIDTH_2;
  uint16_t y0 = center_y - MSG_HEIGHT_2;
  // draw dialog/message box
  tft.drawRect(x0, y0, MSG_WIDTH, MSG_HEIGHT, TFT_WHITE);
  tft.drawRect(x0 + 1, y0 + 1, MSG_WIDTH - 2, MSG_HEIGHT - 2, TFT_WHITE);
  tft.fillRectVGradient(x0 + 2, y0 + 2, MSG_WIDTH - 4, MSG_HEIGHT - 4, TFT_DIALOGGREY, 0x39a7);
  // draw message text relative to center
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(MC_DATUM); // middle center text datum
  tft.drawString(message1, center_x, center_y);

  // display '?' icon in the dialog box
  tft.fillRoundRect(center_x - 110, center_y - 18, 24, 36, 4, TFT_BLUE); // center_x - 120 +
  tft.setTextColor(TFT_WHITE, TFT_BLUE);

  tft.setFreeFont(FF22);
  tft.drawString("?", center_x - 100, center_y - 2);

  encoder.waitReleased(10000); 
  tft.drawRect(center_x + 76, center_y - 13, 38,  27, TFT_WHITE);
  tft.fillRect(center_x + 77, center_y - 12, 36,  25, TFT_DIALOGGREY);
  tft.setTextColor(TFT_EDITCOLOR, TFT_DIALOGGREY);
  tft.drawNumber(*number, center_x + 95, center_y - 2);
  do {
    // Wait for user input (e.g., button press or encoder turn)
    int16_t delta = encoder.getEncoderDelta();
    *number += delta; // Update encoder state
    if (*number < minValue) *number = minValue;
    if (*number > maxValue) *number = maxValue;
    if (delta != 0) {
      tft.fillRect(center_x + 77, center_y - 12, 36,  25, TFT_DIALOGGREY);
      tft.drawNumber(*number, center_x + 95, center_y - 2);
    }
    delay(2);
  } while (encoder.getButtons() == 0);
  encoder.waitReleased(10000); 

  tft.setTextFont(2);
  tft.setTextDatum(TL_DATUM); // middle center text datum
}

// #############################################################################

void drawEnterText(String message1, char *name) {
  uint16_t center_x = DISPLAY_CENTER_X; 	// 160 Pixel
  uint16_t center_y = DISPLAY_CENTER_Y; // 120 Pixel
  // spkrBeep(50);
  uint16_t x0 = DISPLAY_CENTER_X - WIDEMSG_WIDTH_2;
  uint16_t y0 = DISPLAY_CENTER_Y - WIDEMSG_HEIGHT_2;
  // draw dialog/message box
  tft.drawRect(x0, y0, WIDEMSG_WIDTH, WIDEMSG_HEIGHT, TFT_WHITE);
  tft.drawRect(x0 + 1, y0 + 1, WIDEMSG_WIDTH - 2, WIDEMSG_HEIGHT - 2, TFT_WHITE);
  tft.fillRectVGradient(x0 + 2, y0 + 2, WIDEMSG_WIDTH - 4, WIDEMSG_HEIGHT - 4, TFT_DIALOGGREY, 0x39a7);

  tft.setTextDatum(MC_DATUM); // middle center text datum
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(message1, center_x, center_y - 19);

  encoder.waitReleased(10000); 
  x0 += 10; // Box-Koordinaten
  y0 = center_y + 4;
  // use encoder to change ASCII letter by letter, max 16 characters
  // draw 12 text entry boxes, but only edit the current one, which is highlighted
  int32_t text_spacing = 20; // space between characters
  int32_t box_w = 17; // width of the text entry box
  int32_t box_h = 22; // height of the text entry box
  uint16_t text_x0 = x0 + box_w/2 - 7; // starting x position for text entry
  uint16_t text_y0 = y0 + box_h/2 + 5; // starting y position for text entry

  tft.setFreeFont(FF6);
  tft.setTextDatum(TL_DATUM); // top left text datum
  int charIndex = 0;

  for (int i = 0; i < 13; i++) {
    tft.fillRect(x0 + i * text_spacing, y0, box_w, box_h, i == 0 ? TFT_CHARCOAL : TFT_MEDGREY);
    if (name[i] == '\0') {
      name[i] = 32; // replace null characters with spaces for display
    } else {
      tft.setTextColor(i == 0 ? TFT_WHITE : TFT_BLACK);
      tft.drawChar(name[i], text_x0 + i * text_spacing, text_y0);
    }
  }
  tft.drawRect(x0 - 2, y0 - 2, text_spacing * 13 + 1,  box_h + 4, TFT_WHITE);
  char currentChar = name[charIndex];
  enum {s_select, s_edit} editState = s_select;

  uint16_t timeout = 500;
  while (true) {
    int16_t delta = encoder.getEncoderDelta();
    uint8_t buttons = encoder.getButtons();
    if (buttons == 0) {
      timeout = 500; // reset timeout if no buttons are pressed
    }
    if (delta != 0) {
      if (editState == s_select) {
        if (delta > 0) {
          buttons = 4; // simulate button press to change index
        } else {
          buttons = 2;
        }
      } else {
        currentChar += delta;
        if (currentChar < 32) currentChar = 32; // space is the first valid character
        if (currentChar > 126) currentChar = 126; // tilde is the last valid character
        name[charIndex] = currentChar; // Update the name buffer with the new character
        tft.setTextColor(TFT_EDITCOLOR);
        tft.fillRect(x0 + charIndex * text_spacing, y0, box_w, box_h, TFT_CHARCOAL);
        tft.drawChar(currentChar == 32 ? '_' : currentChar, text_x0 + charIndex * text_spacing, text_y0);
      }
    }
    if ((buttons == 4) || (buttons == 2)) {
      tft.setTextColor(TFT_BLACK);
      tft.fillRect(x0 + charIndex * text_spacing, y0, box_w, box_h, TFT_MEDGREY);
      tft.drawChar(currentChar, text_x0 + charIndex * text_spacing, text_y0);
      if (buttons == 4) {
        charIndex++;
        if (charIndex >= 13) charIndex = 0; // wrap around to the first character
      } else {
        charIndex--;
        if (charIndex < 0) charIndex = 12; // wrap around to the last character
      }
      currentChar = name[charIndex];
      tft.setTextColor(editState == s_edit ? TFT_EDITCOLOR : TFT_WHITE);
      tft.fillRect(x0 + charIndex * text_spacing, y0, box_w, box_h, TFT_CHARCOAL);
      tft.drawChar(currentChar == 32 ? '_' : currentChar, text_x0 + charIndex * text_spacing, text_y0);
      encoder.waitReleased(timeout); 
      timeout = 150;
     }
     if (buttons == 1) {
      if (editState == s_select) {
        editState = s_edit;
        tft.setTextColor(TFT_EDITCOLOR);
      } else {  
        editState = s_select;
        tft.setTextColor(TFT_WHITE);
      }
      tft.fillRect(x0 + charIndex * text_spacing, y0, box_w, box_h, TFT_CHARCOAL);
      tft.drawChar(currentChar == 32 ? '_' : currentChar, text_x0 + charIndex * text_spacing, text_y0);
      if (encoder.waitReleased(SAVE_TIMEOUT)) {
        break; // finish editing on double click
      }
    }
    delay(20);
  }
  // remove trailing spaces from name
  for (int i = 12; i >= 0; i--) {
    if (name[i] <= ' ') {
      name[i] = '\0';
    } else {
      break;
    }
  }
  name[13] = '\0';
}

// #############################################################################
//
//     #     # ####### #     # #     #    ### ####### ####### #     #  #####  
//     ##   ## #       ##    # #     #     #     #    #       ##   ## #     # 
//     # # # # #       # #   # #     #     #     #    #       # # # # #       
//     #  #  # #####   #  #  # #     #     #     #    #####   #  #  #  #####  
//     #     # #       #   # # #     #     #     #    #       #     #       # 
//     #     # #       #    ## #     #     #     #    #       #     # #     # 
//     #     # ####### #     #  #####     ###    #    ####### #     #  #####  
//                                                                            
// #############################################################################

// Draws the menu box. The main menu and submenu entries are drawn inside this box as a canvas sprite
void drawSubMenuBox() {
  tft.fillRectVGradient(LISTBOX_LEFT + 1, LISTBOX_TOP + 1, LISTBOX_W - 2, LISTBOX_H - 2, gradientStartColor, gradientEndColor);
  tft.drawRect(LISTBOX_LEFT, LISTBOX_TOP, LISTBOX_W, LISTBOX_H, borderColor);
}

// Draws a single line in the menu list, canvas must be assigned and created before calling this function
// canvas must be assigned and created before calling this function
void drawListEntry(char *text, int line, bool is_active, uint16_t line_color, uint16_t text_color, bool has_enter_action = false) {
	uint32_t top = line * LISTENTRY_H;
  if (is_active) {
    uint16_t gradient_color = canvas.alphaBlend(170, line_color, TFT_BLACK);
    canvas.fillRectVGradient(1, top + 1, LISTENTRY_W - 2, LISTENTRY_H - 2, line_color, gradient_color);
  } else {
    canvas.fillRect(1, top + 1, LISTENTRY_W - 2, LISTENTRY_H - 2, TFT_BLACK);
    text_color = TFT_DIMMED;
  }
  canvas.setTextColor(text_color);
  canvas.drawString(text,  5, top + 6);
  canvas.drawRect(0, top, LISTENTRY_W, LISTENTRY_H + 1, borderColor);
  if (has_enter_action) {
    canvas.fillCircle(LISTENTRY_W - 12, top + LISTENTRY_H/2, 5, text_color);
  }
}

// #############################################################################

// Draws a list of entries in a dialog
// The entries are drawn in a list format with a specified start entry and active line
// startentry is the index of the first entry to display, active_line is the index of the currently selected line
// count is the number of lines to display
// canvas must be assigned and created before calling this function
void drawSubMenuEntries(int start_entry, int count, int active_entry, bool menu_active, uint16_t line_color, uint16_t text_color) {
  if (dimmedMainWindow) menu_active = false;
  line_color = menu_active ? line_color : TFT_DIMMED;
  canvas.createSprite(LISTFIELD_W, LISTFIELD_H);
  canvas.fillRect(0, 0, LISTFIELD_W, LISTFIELD_H, TFT_DIMMED);
  canvas.setFreeFont(FF17);
  canvas.setTextDatum(TL_DATUM); // top left text datum
  if (count > LISTENTRY_COUNT) count = LISTENTRY_COUNT; // Begrenzung der angezeigten Einträge auf die maximale Anzahl der Listenfelder
  for (int i = 0; i < count; i++) {
    bool hasEnterAction = subMenuItems[start_entry + i].enterAction != NULL; // Check if the current menu entry has an enter action
    drawListEntry(subMenuItems[start_entry + i].menuHeader, i, (i == active_entry), line_color, text_color, hasEnterAction);
  }
  canvas.pushSprite(LISTBOX_LEFT + LISTENTRY_PADDING, LISTBOX_SUB_TOP + LISTENTRY_PADDING);
  canvas.deleteSprite();
}

// #############################################################################

// draw main menu line, the main menu is a single line at the top of the screen that shows the current main menu item
// uses canvas sprite to ensure flicker-free updates of the menu entries, especially when the active line changes
void drawMainMenu(int selected_entry, bool menu_active, uint16_t line_color, uint16_t text_color) {
  // bietet vorbefüllte Liste zur Auswahl an, z.B. Directory
  // liefert Eintrag zurück oder -1, wenn CANCEL gewählt wurde
  // strcpy(array[entry_count], "CANCEL"); // immer letzter Eintrag
  if (dimmedMainWindow) menu_active = false;
  line_color = menu_active ? line_color : TFT_DIMMED;
  canvas.createSprite(LISTFIELD_W, LISTENTRY_H);
  canvas.setFreeFont(FF21);
  canvas.setTextDatum(TL_DATUM); // top left text datum
  bool hasEnterAction = mainMenuItems[selected_entry].enterAction != NULL; // Check if the current menu entry has an enter action
  drawListEntry(mainMenuItems[selected_entry].menuHeader, 0, true, line_color, text_color, hasEnterAction);
  canvas.pushSprite(LISTBOX_LEFT + LISTENTRY_PADDING, LISTBOX_TOP + LISTENTRY_PADDING);
  canvas.deleteSprite();
}

// #############################################################################

// draw submenu with scrollable list of entries, the submenu is displayed in a box below the main menu line
// uses canvas sprite to ensure flicker-free updates of the menu entries, especially when the active line changes
void drawSubmenuSelect(int mainmenu_item, int enc_delta, bool menu_active, uint16_t line_color, uint16_t text_color) {
  // bietet vorbefüllte Liste zur Auswahl an, z.B. Directory
  // liefert Eintrag zurück oder -1, wenn CANCEL gewählt wurde
  // strcpy(array[entry_count], "CANCEL"); // immer letzter Eintrag
	// int last_item_offset = item_offset;
	// int last_hilited_line = hilited_line;
  line_color = menu_active ? line_color : TFT_DIMMED;
  int entry_count = subMenuProperties[mainmenu_item].itemCount;

  int line_count = entry_count;
  if (line_count > LISTENTRY_COUNT)
    line_count = LISTENTRY_COUNT; // tatsächlich angezeigte Zeilen

  int item_offset = subMenuProperties[mainmenu_item].itemIndex;
  int start_line = subMenuProperties[mainmenu_item].startLine;
  int hilited_line = subMenuProperties[mainmenu_item].hilitedLine;

  item_offset += enc_delta;
  if ((item_offset >= entry_count)) {
    item_offset = entry_count - 1;
  } else if ((item_offset < 0)) {
    item_offset = 0;
    start_line = 0;
    hilited_line = 0;
  }

  if (entry_count <= LISTENTRY_COUNT) {
    hilited_line = item_offset;
  } else {
    hilited_line += enc_delta;
    if (hilited_line < 0) hilited_line = 0;
    if (hilited_line >= line_count) hilited_line = line_count - 1;
  }
  // display a window of 4 lines, if there are more entries, scroll the list  
  if (item_offset < start_line) {
    hilited_line = 0;
    start_line = item_offset;
  } else if (item_offset >= start_line + line_count) {
    start_line = item_offset - line_count + 1;
    hilited_line = line_count - 1;
  }
  subMenuProperties[mainmenu_item].itemIndex = item_offset;
  subMenuProperties[mainmenu_item].startLine = start_line;
  subMenuProperties[mainmenu_item].hilitedLine = hilited_line;
  drawSubMenuEntries(start_line, line_count, hilited_line, menu_active, line_color, text_color);
}

// #############################################################################
//
//     ######  ######     #    #     #    ####### ######   #####     #    #     # 
//     #     # #     #   # #   #  #  #    #     # #     # #     #   # #   ##    # 
//     #     # #     #  #   #  #  #  #    #     # #     # #        #   #  # #   # 
//     #     # ######  #     # #  #  #    #     # ######  #  #### #     # #  #  # 
//     #     # #   #   ####### #  #  #    #     # #   #   #     # ####### #   # # 
//     #     # #    #  #     # #  #  #    #     # #    #  #     # #     # #    ## 
//     ######  #     # #     #  ## ##     ####### #     #  #####  #     # #     # 
//                                                                                
// #############################################################################

// Einfache Darstellung der wichtigsten Organ-Tabs und Zugriegel als Balken
// uses canvas sprite to ensure flicker-free updates of the graphical elements, especially when the state of the tabs or sliders changes

// canvas must be assigned and created before calling this function
void drawOrganTab(int32_t left, int32_t top, bool is_on, uint16_t on_color) {
  if (!is_on) {
    on_color = TFT_BLACK;
  }
  if (dimmedMainWindow) {
    on_color = canvas.alphaBlend(140, on_color, TFT_BLACK);
  }
  canvas.drawRect( left, top, 7, 12, borderColor);
  canvas.fillRect(left + 1, top + 1, 5, 10, on_color);
}

// canvas must be assigned and created before calling this function
void drawOrganSlider(int32_t left, int32_t top, int32_t val, uint16_t db_color) {
  int32_t len = val / 4;
  if (dimmedMainWindow) {
    db_color = canvas.alphaBlend(140, db_color, TFT_BLACK);
  }
  canvas.drawRect(left, top, 7, 40, borderColor);
  canvas.fillRect(left + 1, top + 1, 5, len, TFT_CHARCOAL);
  canvas.fillRect(left + 1, top + len + 1, 5, 38 - len, TFT_BLACK);
  canvas.fillRect(left - 1, top + len + 1, 8, 7, db_color);
}

uint16_t organSlidercolors[9] = {
  TFT_BROWN,
  TFT_BROWN,
  TFT_LIGHTGREY,
  TFT_LIGHTGREY,
  TFT_BLACK,
  TFT_LIGHTGREY,
  TFT_BLACK,
  TFT_BLACK,
  TFT_LIGHTGREY
};

// Draws a representation of main B3 organ tabs and drawbar sliders
void drawOrgan(int manual_select = 0) {
  canvas.createSprite(ORGANBOX_W, ORGANBOX_H);
  canvas.drawRect(0, 0, ORGANBOX_W, ORGANBOX_H, borderColor);
  // canvas.fillRect(ORGANBOX_LEFT + 1, ORGANBOX_TOP + 1, ORGANBOX_W - 2, ORGANBOX_H - 2, TFT_DIALOGGREY);
  canvas.fillRectVGradient(1, 1, ORGANBOX_W - 2, ORGANBOX_H - 2, gradientStartColor, gradientEndColor);
  canvas.setTextDatum(MC_DATUM); // middle center text datum
  canvas.setFreeFont(FF21);
  canvas.setTextColor(dimmedMainWindow ? TFT_DIMMED : TFT_CYAN);
  canvas.drawString(vibknobLettering[hx3EditArray[264]], 16, 48);
  drawOrganTab(10, 7, (hx3EditArray[133] != 0), TFT_YELLOW); // Beispiel: Tab 1 ist aktiv
  drawOrganTab(20, 7, (hx3EditArray[134] != 0), TFT_YELLOW); // Beispiel: Tab 2 ist inaktiv
  for (int i = 0; i < 4; i++) {
    // Beispiel: Alle 9 Zugriegel sind inaktiv
    drawOrganTab(85 + i * 10, 7, (hx3EditArray[128 + i] != 0), TFT_WHITE);
  }
  uint32_t arr_idx = 0;
  uint32_t arr_count = 9;
  switch (manual_select) {
    case 0:
      arr_idx = 0; // Alle Upper-Zugriegel
      canvas.setTextColor(dimmedMainWindow ? TFT_DIMMED : TFT_YELLOW);
      canvas.drawString("UPR", 57, 12);
      break;
    case 1:
      arr_idx = 16; // Alle Lower-Zugriegel
      canvas.setTextColor(dimmedMainWindow ? TFT_DIMMED : TFT_RED);
      canvas.drawString("LWR", 57, 12);
      break;
    case 2:
      arr_idx = 72; // Alle Pedal-Zugriegel
      arr_count = 4; // nur 4 Pedal-Zugriegel
      canvas.setTextColor(dimmedMainWindow ? TFT_DIMMED : TFT_ORANGE);
      canvas.drawString("PED", 57, 12);
      break;
    default:
      arr_idx = 0; // Alle Upper-Zugriegel
      break;
  }
  for (int i = 0; i < arr_count; i++) {
    drawOrganSlider(35 + i * 10, 30, hx3EditArray[arr_idx++], organSlidercolors[i]);
  }
  if (manual_select == 2) {
    // separater Sustain-ZR für Pedal
    drawOrganSlider(85, 30, hx3EditArray[67], TFT_ORANGE);
  }
  canvas.pushSprite(ORGANBOX_LEFT, ORGANBOX_TOP);
  canvas.deleteSprite();
}


#endif