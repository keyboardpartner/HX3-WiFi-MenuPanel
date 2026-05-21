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

// Fenster-Positionen und -Größen für die verschiedenen Anzeigebereiche

#ifdef NV3007_DRIVER
  // 428 * 142 Display, z.B. NV3007 2.79 Zoll
  #define MENU_TOP (DISPLAY_H_OFFS + 4)
  #define MENU_LEFT 10
  #define MENU_W 170

  #define MENU_LINE_H 25
  #define MENU_LINE_CENTER_Y 11
  #define SUBMENU_TOP (DISPLAY_H_OFFS + 40) // Abstand der Submenü-Einträge zum oberen Rand 
  #define SUBMENU_COUNT 4 // Anzahl der angezeigten Submenü-Einträge
  #define SUBMENU_H (SUBMENU_COUNT * MENU_LINE_H) // Höhe aller Listenfelder zusammen

  #define VALUEBOX_TOP (DISPLAY_H_OFFS + 5)
  #define VALUEBOX_LEFT (MENU_LEFT + MENU_W + 10)

  #define VALUEBOX_W MENU_W
  #define VALUEBOX_H 60
  #define VALUEBOX_CENTER_X (VALUEBOX_W/2)
  #define VALUEBOX_CENTER_Y (VALUEBOX_H/2)

  #define ORGANBOX_TOP (SUBMENU_TOP + MENU_LINE_H)
  #define ORGANBOX_LEFT VALUEBOX_LEFT
  #define ORGANBOX_W MENU_W
  #define ORGANBOX_H (MENU_LINE_H * (SUBMENU_COUNT - 1))
#else
  // 320 x 240 Display, GMT020
  #define MENU_TOP (DISPLAY_H_OFFS + 25)
  #define MENU_LEFT 5
  #define MENU_W 150

  #define MENU_LINE_H 28
  #define MENU_LINE_CENTER_Y 13
  #define SUBMENU_TOP (MENU_TOP + MENU_LINE_H + 20) // Abstand der Submenü-Einträge zum oberen Rand 
  #define SUBMENU_COUNT 5 // Anzahl der angezeigten Submenü-Einträge
  #define SUBMENU_H (SUBMENU_COUNT * MENU_LINE_H) // Höhe aller Listenfelder zusammen

  #define VALUEBOX_TOP MENU_TOP
  #define VALUEBOX_LEFT (MENU_LEFT + MENU_W + 10)

  #define VALUEBOX_W MENU_W
  #define VALUEBOX_H 70
  #define VALUEBOX_CENTER_X (VALUEBOX_W/2)
  #define VALUEBOX_CENTER_Y (VALUEBOX_H/2)

  #define ORGANBOX_TOP (SUBMENU_TOP + MENU_LINE_H)
  #define ORGANBOX_LEFT VALUEBOX_LEFT
  #define ORGANBOX_W MENU_W
  #define ORGANBOX_H (MENU_LINE_H * (SUBMENU_COUNT - 1))

#endif



// Immer zentriert auf dem TFT
#define MSGBOX_W 250
#define MSGBOX_H 70
#define MSGBOX_CENTER_H (MSGBOX_W/2) // halbe Breite
#define MSGBOX_CENTER_V (MSGBOX_H/2) // halbe Höhe

// Immer zentriert auf dem TFT
#define WIDEMSGBOX_W 280
#define WIDEMSGBOX_H 80
#define WIDEMSGBOX_CENTER_H (WIDEMSGBOX_W/2) // halbe Breite
#define WIDEMSGBOX_CENTER_V (WIDEMSGBOX_H/2) // halbe Höhe

// msgType =0/16 "i" in blau, =1/17 "?" in blau, =2/18 "!" in rot
enum DialogBoxType {
	DB_INFO = 0,
	DB_REQUEST = 1,
	DB_ERROR = 2,

  DB_INFO_OK = 8,
  DB_REQUEST_OK = 9,
  DB_ERROR_OK = 10,
};
                                                      
typedef struct {
  char str1[16];
  char str2[16];
} textValueStringType;

const textValueStringType organModelStrings[] PROGMEM = {
  {"B3", "Standard"},  // 0
  {"B3", "Old"},  // 1
  {"B3", "Recapped"},  // 2
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
  {"122 Std", "Small Room"},  // 0
  {"122 Std", "Large Room"},  // 1
  {"122 Old", "Small Room"},  // 2
  {"122 Old", "Large Room"},  // 3
  {"147 New", "Small Room"},  // 4
  {"147 New", "Large Room"},  // 5
  {"760 Std", "Small Room"},  // 6
  {"760 Std", "Large Room"},  // 7
  {"Space", "Sound"},  // 8
  {"Sharma", "2001"},  // 9
  {"Fender", "Vibratone"},  // 10
  {"Dynacord", "D-100"},  // 11
  {"Dynacord", "CLS-222"},  // 12
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
  {"TOS Gen", "Square Sine"},  // 7 Sinus aus Rechteck-Filterung
};

const char vibknobLettering[6][3] = {
  "V1", "C1", "V2", "C2", "V3", "C3"
};

const textValueStringType gatingModeStrings[] PROGMEM = {
  {"9", "Contacts"},    // 0 B3
  {"12", "Contacts"},    // 1 H100
  {"Env Gen", "ADSR Mode"},    // 2
  {"Env Gen", "Perc DBs"},  
  {"Env Gen", "Time Bend"},  
};

const textValueStringType wifiModeStrings[] PROGMEM = {
  {"OFF", ""},    // no Wifi
  {"Router", "STA Mode"},    // STA mode
  {"Access Point", "AP Mode"},    // AP mode
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
  gradientEndColor = TFT_SHADOW;
  borderColor = TFT_BORDER;
  if (dimmedMainWindow) {
    gradientStartColor = tft.alphaBlend(140, gradientStartColor, TFT_BLACK); // Apply a dimming effect to the entire screen
    gradientEndColor = tft.alphaBlend(140, gradientEndColor, TFT_BLACK); // Apply a dimming effect to the entire screen
    borderColor = tft.alphaBlend(140, borderColor, TFT_BLACK); // Apply a dimming effect to the entire screen
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
    DPRINTLNF("File not found");
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
      DPRINT("BMP loaded in "); 
      DPRINT(millis() - startTime);
      DPRINTLN(" ms");
    }
    else DPRINTLNF("BMP format not recognized.");
  }
  bmpFS.close();
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
void drawOrganTab(uint32_t scale_x_100, uint32_t scale_y_100, int32_t left, int32_t top, bool is_on, uint16_t on_color) {
  if (dimmedMainWindow) {
    on_color = canvas.alphaBlend(140, on_color, TFT_BLACK);
  }
  canvas.drawRect( left, top, (7*scale_x_100)/100, (12*scale_y_100)/100, borderColor);
  if (!is_on) {
    on_color = TFT_BLACK;
  }
  canvas.fillRect(left + 1, top + 1, scale_x_100/20, scale_y_100/10, on_color);
}

// canvas must be assigned and created before calling this function
void drawOrganSlider(uint32_t scale_x_100, uint32_t scale_y_100, int32_t left, int32_t top, int32_t val, uint16_t db_color) {
  int32_t len = (val*scale_y_100)/350;
  if (dimmedMainWindow) {
    db_color = canvas.alphaBlend(140, db_color, TFT_BLACK);
  }
  canvas.drawRect(left, top, (7*scale_x_100)/100, (45*scale_y_100)/100, borderColor);
  canvas.fillRect(left + 1, top + 1, scale_x_100/20, len, TFT_BLACK);
  canvas.fillRect(left + 1, top + len + 1, scale_x_100/20, (41*scale_y_100)/100 - len, TFT_DIMMED);
  canvas.fillRect(left - 1, top + len + 1, (8*scale_x_100)/100, (7*scale_y_100)/100, db_color);
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
void drawOrgan(int32_t x, int32_t y, int32_t w, int32_t h, int manual_select) {
  uint32_t scale_x_100 = (100*w)/150; // Skalierung basierend auf der definierten Breite der Orgelbox
  uint32_t scale_y_100 = (100*h)/84; // Skalierung basierend auf der definierten Höhe der Orgelbox
  canvas.createSprite(w, h);
  canvas.drawRect(0, 0, w, h, borderColor);
  canvas.fillRectVGradient(1, 1, w - 2, h - 2, gradientStartColor, gradientEndColor);
  canvas.setTextDatum(MC_DATUM); // middle center text datum
  if (scale_x_100 >= 125) {
    canvas.setFreeFont(FF22);
  } else {
    canvas.setFreeFont(FF21);
  }
  uint32_t scaled_dx = (12*scale_x_100)/100;
  uint32_t scaled_x = (12*scale_x_100)/100;
  uint32_t scaled_y = (7*scale_y_100)/100;
  if (manual_select == 0) {
    canvas.setTextColor(dimmedMainWindow ? TFT_DIMMED : TFT_CYAN);
    canvas.drawString(vibknobLettering[hx3EditArray[264]], (16*scale_x_100)/100, (48*scale_y_100)/100);
    drawOrganTab(scale_x_100, scale_y_100, scaled_x, scaled_y, (hx3EditArray[132] != 0), TFT_YELLOW); // Beispiel: Tab 1 ist aktiv
    drawOrganTab(scale_x_100, scale_y_100, scaled_x + scaled_dx, scaled_y, (hx3EditArray[133] != 0), TFT_YELLOW);
    for (int i = 0; i < 4; i++) {
      // Alle 4 Percussion-Tabs zeichnen
      drawOrganTab(scale_x_100, scale_y_100, scaled_x + (i+7) * scaled_dx, scaled_y, (hx3EditArray[128 + i] != 0), TFT_WHITE);
    } 
  }
  uint32_t arr_idx = 0;
  uint32_t arr_count = 9;
  if (manual_select == 0) {
    scaled_x = (65*scale_x_100)/100;
  } else{
    scaled_x = (73*scale_x_100)/100;
  }
  scaled_y = (12*scale_y_100)/100;
  switch (manual_select) {
    case 0:
      arr_idx = 0; // Alle Upper-Zugriegel
      canvas.setTextColor(dimmedMainWindow ? TFT_DIMMED : TFT_YELLOW);
      canvas.drawString("UPR", scaled_x, scaled_y);
      break;
    case 1:
      arr_idx = 16; // Alle Lower-Zugriegel
      canvas.setTextColor(dimmedMainWindow ? TFT_DIMMED : TFT_RED);
      canvas.drawString("LWR", scaled_x, scaled_y);
      break;
    case 2:
      arr_idx = 72; // Alle Pedal-Zugriegel
      arr_count = 4; // nur 4 Pedal-Zugriegel
      canvas.setTextColor(dimmedMainWindow ? TFT_DIMMED : TFT_ORANGE);
      canvas.drawString("PED", scaled_x, scaled_y);
      break;
    default:
      arr_idx = 0; // Alle Upper-Zugriegel
      break;
  }
  if (manual_select == 1) {
    scaled_x = (25*scale_x_100)/100;
  } else{
    scaled_x = (37*scale_x_100)/100;
  }
  scaled_y = (30*scale_y_100)/100;
  // Alle 5 oder 9 Zugriegel zeichnen
  for (int i = 0; i < arr_count; i++) {
    drawOrganSlider(scale_x_100, scale_y_100, scaled_x + (i*scaled_dx), scaled_y, hx3EditArray[arr_idx++], organSlidercolors[i]);
  }
  if (manual_select == 2) {
    // separater Sustain-ZR für Pedal
    drawOrganSlider(scale_x_100, scale_y_100, scaled_x + (6*scaled_dx), scaled_y, hx3EditArray[67], TFT_ORANGE);
  }
  canvas.pushSprite(x, y);
  canvas.deleteSprite();
}

// #############################################################################


// statt Submenü-Anzeige wird die linke Box im Main-Menü für die Anzeige des Obermanuals genutzt
void drawMainOrgan() {
  // Header für linke Orgel-Box im Main-Menü, überschreibt auch alte Submenü-Anzeige
  canvas.createSprite(MENU_W, MENU_LINE_H);
  // entry->displayType = tm_preset;
  canvas.setTextDatum(ML_DATUM);
  canvas.setFreeFont(FF17);
  if (dimmedMainWindow) {
    canvas.setTextColor(TFT_BLACK);
    canvas.fillRectVGradient(1, 1, MENU_W - 2, MENU_LINE_H - 1, gradientStartColor, gradientEndColor);
  } else {
    canvas.setTextColor(TFT_WHITE);
    canvas.fillRectVGradient(1, 1, MENU_W - 2, MENU_LINE_H - 1, TFT_HEADERCOLOR, TFT_HEADERCOLOR_GRAD);
  }
  canvas.drawRect(0, 0, MENU_W, MENU_LINE_H, borderColor);
  canvas.drawString(organModelStrings[hx3EditArray[265]].str1, 5, MENU_LINE_CENTER_Y);
  canvas.drawString(organModelStrings[hx3EditArray[265]].str2, 72, MENU_LINE_CENTER_Y);
  canvas.pushSprite(MENU_LEFT, SUBMENU_TOP);
  canvas.deleteSprite();
  drawOrgan(MENU_LEFT, ORGANBOX_TOP, ORGANBOX_W, ORGANBOX_H, 0); 
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
void drawTextValue(int32_t center_x, int32_t center_y, const char* text) {
  canvas.drawString(text, center_x, center_y - 2);
}

// Zeige einen zweizeiligen Text in der Valuebox an
// canvas must be assigned and created before calling this function
void drawTextValue2(int32_t center_x, int32_t center_y, const char* text1, const char* text2) {
  if (strlen(text2) == 0) {
    drawTextValue(center_x, center_y, text1); // Nur 1 Zeile
    return;
  }
  if (strlen(text1) > 11) {
    canvas.setFreeFont(FF21);
  } else {
    canvas.setFreeFont(FF22);
  }
  canvas.drawString(text1, center_x, center_y - (VALUEBOX_H/5));
  if (strlen(text2) > 11) {
    canvas.setFreeFont(FF21);
  } else {
    canvas.setFreeFont(FF22);
  }
  canvas.drawString(text2, center_x, center_y + (VALUEBOX_H/5));
}

// Zeige Preset-Nummer und -Text in der Valuebox an
// canvas must be assigned and created before calling this function
void drawPreset(int32_t center_x, int32_t center_y, const uint16_t preset, const char* text) {
  canvas.drawNumber(preset, center_x, center_y - (VALUEBOX_H/5));
  canvas.setFreeFont(FF21);
  canvas.drawString(text, center_x, center_y + (VALUEBOX_H/5));
}

// Zeige einen numerischen Wert in der Valuebox an
// canvas must be assigned and created before calling this function
void drawNumeric(int32_t center_x, int32_t center_y, const int16_t value) {
  uint32_t text_width = canvas.textWidth("888") + 2; // Breite von 3 Ziffern in der Schriftart FF24 
  canvas.drawNumber(value, center_x, center_y - 2);
}

// Zeige einen binären Wert (ON/OFF) in der Valuebox an
// canvas must be assigned and created before calling this function
void drawBinary(int32_t center_x, int32_t center_y, const int16_t value) {
  if (value == 0) {
    drawTextValue(center_x, center_y, "OFF");
  } else {
    drawTextValue(center_x, center_y, "ON");
  }
}

// Zeige den Status des Rotary Speed in der Valuebox an (SLOW, STOP, FAST)
// canvas must be assigned and created before calling this function
void drawRotarySpeed(int32_t center_x, int32_t center_y, const int16_t value) {
  switch (value) {
  case 0:
    drawTextValue(center_x, center_y, "SLOW");
    break;
  case 1:
    drawTextValue(center_x, center_y, "STOP");
    break;
  case 2:
    drawTextValue(center_x, center_y, "FAST");
    break;
  }
}

// Zeige einen Wert als Potentiometer-Arc in der Valuebox an
// canvas must be assigned and created before calling this function
void drawPot(int16_t center_x, int16_t center_y, int16_t w, int16_t h, const int16_t value, const int16_t max, uint16_t arc_color) {
  uint16_t text_width = canvas.textWidth("888") + 2; // Breite von 3 Ziffern in der Schriftart FF23 
  int16_t end_angle = map(value, 0, max, 45, 315);
  canvas.drawNumber(value, center_x - 33, center_y - 2);
  int16_t radius = center_y - 5;
  int16_t inner_radius = center_y - 10;
  canvas.drawArc(center_x + 28, center_y, radius, inner_radius, end_angle, 315, TFT_DIMMED, TFT_CHARCOAL); // Draw the arc
  canvas.drawArc(center_x + 28, center_y, radius, inner_radius, 45, end_angle, arc_color, TFT_CHARCOAL); // Draw the arc
}

// Zeige einen Wert als VibKnob in der Valuebox an
// canvas must be assigned and created before calling this function
void drawVibKnob(int16_t center_x, int16_t center_y, int16_t w, int16_t h, const int16_t value, const int16_t max, uint16_t arc_color) {
  uint16_t text_width = canvas.textWidth("C3") + 2; // Breite von 2 Buchstaben in der Schriftart FF23 
  canvas.drawString(vibknobLettering[value], center_x - 33, center_y - 2);
  int16_t angle_step = 360 / (max + 1); // Winkel pro Markierung
  // draw arc segment as knob pointer
  for (int i = 0; i <= max; i++) {
    int16_t markAngle = (288 + i * angle_step) % 360; // Winkel der aktuellen Markierung
    if (value == i) {
      canvas.drawArc(center_x + 28, center_y, 28, 23, markAngle, (markAngle + 24) % 360, arc_color, TFT_CHARCOAL); // Draw the active mark
    } else {
      canvas.drawArc(center_x + 28, center_y, 27, 23, markAngle, (markAngle + 24) % 360, TFT_CHARCOAL, TFT_CHARCOAL); // Draw the inactive mark
    }
  }
  canvas.drawCircle(center_x + 28, center_y, 28, TFT_CHARCOAL); // Draw the outer circle of the knob
  canvas.drawCircle(center_x + 28, center_y, 29, TFT_CHARCOAL); // Draw the outer circle of the knob
}
// Zeige einen Wert als horizontalen Fortschrittsbalken in der Valuebox an
// canvas must be assigned and created before calling this function
void drawProgress(int16_t center_x, int16_t center_y, int16_t w, int16_t h, const int16_t value, const int16_t max, uint16_t bar_color) {
  int16_t bar_length = map(value, 0, max, 0, w - 22);
  uint16_t text_width = canvas.textWidth("888") + 2; // Breite von 3 Ziffern in der Schriftart FF23 
  uint16_t bar_gradient = canvas.alphaBlend(140, bar_color, TFT_BLACK);
  canvas.fillRectVGradient(center_x - (w - 22)/2, center_y + 10, bar_length, 18, bar_color, bar_gradient);
  canvas.fillRectVGradient(center_x - (w - 22)/2 + bar_length, center_y + 10, w - 22 - bar_length, 18, TFT_CHARCOAL, TFT_BLACK);
  canvas.drawRect(center_x - (w - 20)/2, center_y + 9, w - 20, 20, borderColor);
  canvas.drawNumber(value, center_x, center_y - 15);
}


// #############################################################################


// Draws a value from currentMenuEntry in the value box. If in_valuechange is true, the value is highlighted to indicate it can be edited.
void drawValue(menuEntryType* entry, int16_t x, int16_t y, int16_t w, int16_t h) {
  uint16_t scale_x_100 = (10*w)/15; // Skalierung basierend auf der nominalen Breite der Value-Box
  uint16_t scale_y_100 = (10*h)/7;  // Skalierung basierend auf der nominalen Höhe der Value-Box
  uint16_t center_x = w/2;
  uint16_t center_y = h/2;
  int16_t value = hx3EditArray[entry->editArrayIdx];
  uint16_t text_color = TFT_BLACK;
  if (editingOn && (!dimmedMainWindow)) {
    text_color = TFT_EDITCOLOR;
  }
  canvas.createSprite(w, h);
  canvas.drawRect(0, 0, w, h, borderColor);
  canvas.fillRectVGradient(1, 1, w - 2, h - 2, gradientStartColor, gradientEndColor);

  canvas.setTextDatum(MC_DATUM); // middle center text datum
  canvas.setTextColor(text_color);
  if (scale_x_100 >= 100) {
    canvas.setFreeFont(FF23);
  } else if (scale_x_100 >= 75) {
    canvas.setFreeFont(FF22);
  } else {
    canvas.setFreeFont(FF21); 
  }

  switch (entry->displayType) {
  case tm_numeric:
    drawNumeric(center_x, center_y, value);
    break;
  case tm_midich:
    drawNumeric(center_x, center_y, value + 1); // MIDI-Kanal 1-16 anzeigen, intern aber 0-15
    break;
  case tm_pot:
    // als Bogen
    drawPot(center_x, center_y, w, h, value, entry->menuValueMax, text_color);
    break;
  case tm_vibknob:
    // als runder Drehschalter
    drawVibKnob(center_x, center_y, w, h, value, entry->menuValueMax, text_color);
    break;
  case tm_drawbar:
    // als Balken
    drawProgress(center_x, center_y, w, h, value, entry->menuValueMax, editingOn ? text_color : TFT_LIGHTGREY);
    break;
  case tm_tab:
    // als Ein/Aus
    drawBinary(center_x, center_y, value);
    break;
  case tm_button:
    // auch möglich, wenn keine Variable angegeben ist
    canvas.drawString("OK", center_x, center_y - 15*scale_y_100/100);
    canvas.setFreeFont(FF17);
    if (!dimmedMainWindow) canvas.setTextColor(TFT_RED);
    canvas.drawString("Press 2 sec", center_x, center_y + 15*scale_y_100/100);
    break;
  case tm_halfmoon:
    drawRotarySpeed(center_x, center_y, value);
    break;
  case tm_preset:
    drawPreset(center_x, center_y, value, hx3PresetName);
    break;      
  case tm_phrknob:
    drawTextValue2(center_x, center_y, phrModeStrings[value].str1, phrModeStrings[value].str2);
    break;      
  case tm_midicc:
    drawTextValue2(center_x, center_y, midiCCstrings[value].str1, midiCCstrings[value].str2);
    break;      
  case tm_organ:
    drawTextValue2(center_x, center_y, organModelStrings[value].str1, organModelStrings[value].str2);
    break;      
  case tm_speaker:
    drawTextValue2(center_x, center_y, speakerModelStrings[value].str1, speakerModelStrings[value].str2);
    break;      
  case tm_waveset:
    drawTextValue2(center_x, center_y, wavesetStrings[value].str1, wavesetStrings[value].str2);
    break;      
  case tm_tapering:
    drawTextValue2(center_x, center_y, taperingStrings[value].str1, taperingStrings[value].str2);
    break;      
  case tm_gating:
    drawTextValue2(center_x, center_y, gatingModeStrings[value].str1, gatingModeStrings[value].str2);
    break;      
  case tm_wifimode:
    drawTextValue2(center_x, center_y, wifiModeStrings[value].str1, wifiModeStrings[value].str2);
    break;      
  }
  canvas.pushSprite(x, y);
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

void drawShadowRect(int16_t x, int16_t y, int16_t w, int16_t h) {
  tft.drawRect(x, y, w, h, TFT_MEDGREY);
  tft.drawRect(x + 1, y + 1, w - 2, h - 2, TFT_WHITE);
  tft.fillRectVGradient(x + 2, y + 2, w - 4, h - 4, TFT_DIALOGGREY, TFT_SHADOW);
}

// Draw a dialog box with a message. Does not restore screen content.
// message1 is the main message, message2 is an optional secondary message
// msgType sets the icon type (and leaves space for buttons if needed)
void drawMsg(String message1, String message2, DialogBoxType msgType) {
  // spkrBeep(50);
  uint16_t x0 = DISPLAY_CENTER_X - MSGBOX_CENTER_H;
  uint16_t y0 = DISPLAY_CENTER_Y - MSGBOX_CENTER_V;
  // draw dialog/message box
  drawShadowRect(x0, y0, MSGBOX_W, MSGBOX_H);
  // draw message text relative to center
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(ML_DATUM); // middle center text datum
  if (message2.length() == 0) {
    tft.drawString(message1, x0 + 50, DISPLAY_CENTER_Y - 2);
  } else {
    tft.drawString(message1, x0 + 50, DISPLAY_CENTER_Y - 14);
    tft.drawString(message2, x0 + 50, DISPLAY_CENTER_Y + 10);
  }

  // display icon in the dialog box
  tft.setTextDatum(MC_DATUM); // middle center text datum
  uint16_t icon_color;
  if (msgType == DB_ERROR || msgType == DB_ERROR_OK)
    icon_color = TFT_RED;
  else
    icon_color = TFT_BLUE;
  tft.fillRoundRect(DISPLAY_CENTER_X - 112, DISPLAY_CENTER_Y - 18, 26, 36, 4, icon_color); // center_x - 120 +
  tft.setTextColor(TFT_WHITE, icon_color);

  tft.setFreeFont(FF22);
  int16_t icon_x = DISPLAY_CENTER_X - 100; // adjust center_x for icon position
  int16_t icon_y = DISPLAY_CENTER_Y - 2;    // nudge up
  switch (msgType) {
  case DB_INFO:
  case DB_INFO_OK:
    tft.drawString("i", icon_x, icon_y);
    break;
  case DB_REQUEST:
  case DB_REQUEST_OK:
    tft.drawString("?", icon_x, icon_y);
    break;
  case DB_ERROR:
  case DB_ERROR_OK:
    tft.drawString("!", icon_x, icon_y);
    break;
  }
  tft.setTextFont(2);
  tft.setTextDatum(TL_DATUM); // middle center text datum
}

// Draw a dialog box with a message for a specified time
// message1 is the main message, message2 is an optional secondary message
// Screen content will be restored after the message box is displayed
void drawMsgTimeout(String message1, String message2, int duration, DialogBoxType msgType = DB_INFO) {
  // uint16_t x0 = DISPLAY_CENTER_X - MSGBOX_CENTER_H;
  // uint16_t y0 = DISPLAY_CENTER_Y - MSGBOX_CENTER_V;
  // uint16_t *screenbuf;
  // screenbuf = new uint16_t[MSGBOX_W * MSGBOX_H]; // Create a screen buffer
  // tft.readRect(x0, y0, MSGBOX_W, MSGBOX_H, screenbuf);
  drawMsg(message1, message2, msgType);
  msgTimeoutEndTime = millis() + duration;
  msgTimeoutActive = true;
  // Restore screen content
  // tft.pushRect(x0, y0, MSGBOX_W, MSGBOX_H, screenbuf);
  // delete[] screenbuf; // Free the screen buffer memory
}

// Draw a dialog box with a message for a specified time
// message1 is the main message, message2 is an optional secondary message
// Screen content will be restored after the message box is displayed
void drawMsgWaitDuration(String message1, String message2, int duration, DialogBoxType msgType = DB_INFO) {
  drawMsg(message1, message2, msgType);
  delay(duration);
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

// Dialog mit Eingabe eines numerischen Werts. Der Wert wird über den Encoder geändert 
// und mit einem dem ENTER-Button bestätigt. 
// message1 ist die Anweisung, number ist die Variable, die geändert wird, 
// minValue und maxValue begrenzen den Wertebereich.
// Liefert TRUE, wenn ENTER-Button länger als SAVE_TIMEOUT gedrückt gehalten wurde, sonst FALSE

void drawMsgNumber(int16_t number) {
  tft.setFreeFont(FF22);
  tft.setTextDatum(MC_DATUM); // middle center text datum
  tft.drawRect(DISPLAY_CENTER_X + 74, DISPLAY_CENTER_Y - 14, 44,  28, TFT_WHITE);
  tft.fillRect(DISPLAY_CENTER_X + 75, DISPLAY_CENTER_Y - 13, 42,  26, TFT_DIALOGGREY);
  tft.setTextColor(TFT_EDITCOLOR, TFT_DIALOGGREY);
  tft.drawNumber(number, DISPLAY_CENTER_X + 95, DISPLAY_CENTER_Y - 2);
}

bool drawEnterNumber(String message1, int16_t *number, int16_t minValue = 0, int16_t maxValue = 127 ) {
  drawMsg(message1, "", DB_REQUEST);
  drawMsgNumber(*number);
  encoder.waitReleased(10000); 
  do {
    // Wait for user input (e.g., button press or encoder turn)
    int16_t delta = encoder.getEncoderDelta();
    *number += delta; // Update encoder state
    if (*number < minValue) *number = minValue;
    if (*number > maxValue) *number = maxValue;
    if (delta != 0) {
      drawMsgNumber(*number);
    }
    delay(2);
  } while (encoder.getButtons() == 0);
  return encoder.waitReleased(SAVE_TIMEOUT) != 0;
}

// #############################################################################

// Dialog mit Eingabe eines bis zu 12 Zeichen langen Namens. Der Name wird über den Encoder geändert 
void drawEnterText(String message1, char *name) {
  uint16_t center_x = DISPLAY_CENTER_X; 	// 160 Pixel
  uint16_t center_y = DISPLAY_CENTER_Y; // 120 Pixel
  // spkrBeep(50);
  uint16_t x0 = center_x - WIDEMSGBOX_CENTER_H;
  uint16_t y0 = center_y - WIDEMSGBOX_CENTER_V;
  // draw dialog/message box
  drawShadowRect(x0, y0, WIDEMSGBOX_W, WIDEMSGBOX_H);

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
        break; // finish editing on long click
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

// Draws a single line in the menu list, canvas must be assigned and created before calling this function
// canvas must be assigned and created before calling this function
void drawListEntry(char *text, int line, bool line_active, bool menu_active, bool has_enter_action, bool has_submenu = false) {
  if (dimmedMainWindow) menu_active = false;
  uint16_t text_color;
  uint16_t line_color;
  canvas.setFreeFont(FF17);
  if (menu_active) {
    if (editingOn) {
      line_color = 0x6ecd;
    } else {
      line_color = TFT_WHITE;
    }
    // if (line_active) canvas.setFreeFont(FF21); // sieht nicht so schön aus
  } else {
     line_color = TFT_DIMMED;
  }
	uint32_t top = line * MENU_LINE_H;
  if (line_active) {
    uint16_t gradient_color = canvas.alphaBlend(140, line_color, TFT_BLACK);
    canvas.fillRectVGradient(1, top + 1, MENU_W - 2, MENU_LINE_H - 2, line_color, gradient_color);
    text_color = TFT_BLACK;
  } else {
    canvas.fillRect(1, top + 1, MENU_W - 2, MENU_LINE_H - 2, TFT_BLACK);
    text_color = menu_active ? TFT_MEDGREY : TFT_DIMMED;
   }
  canvas.setTextColor(text_color);
  canvas.drawString(text,  5, top + MENU_LINE_CENTER_Y);
  canvas.drawFastHLine(0, top + MENU_LINE_H, MENU_W, borderColor);
	uint32_t x = MENU_W - 12;
  if (has_enter_action) {
    canvas.fillCircle(x, top + MENU_LINE_CENTER_Y, 5, text_color);
    x -=14;
  }
  if (has_submenu) {
    canvas.fillTriangle(x - 4, top + MENU_LINE_CENTER_Y - 4, x + 4, top + MENU_LINE_CENTER_Y - 4, x, top + MENU_LINE_CENTER_Y + 4, text_color);
  }
}

// #############################################################################

// Draws a list of entries in a dialog
// The entries are drawn in a list format with a specified start entry and active line
// startentry is the index of the first entry to display, active_line is the index of the currently selected line
// count is the number of lines to display
// canvas must be assigned and created before calling this function
void drawSubMenuEntries(int start_entry, int count, int active_entry) {
  canvas.createSprite(MENU_W, SUBMENU_H);
  canvas.fillRect(0, 0, MENU_W, SUBMENU_H, TFT_DIMMED);
  canvas.drawRect(0, 0, MENU_W, SUBMENU_H, borderColor);
  canvas.setTextDatum(ML_DATUM); // mid left text datum
  if (count > SUBMENU_COUNT) count = SUBMENU_COUNT; // Begrenzung der angezeigten Einträge auf die maximale Anzahl der Listenfelder
  for (int i = 0; i < count; i++) {
    bool hasEnterAction = subMenuItems[start_entry + i].enterAction != NULL; // Check if the current menu entry has an enter action
    drawListEntry(subMenuItems[start_entry + i].menuHeader, i, (i == active_entry), (currentMenuState == s_insubmenu), hasEnterAction, false);
  }
  canvas.pushSprite(MENU_LEFT, SUBMENU_TOP);
  canvas.deleteSprite();
}
 
// #############################################################################

// draw main menu line, a single line at the top of the screen that shows the current main menu item
// uses canvas sprite to ensure flicker-free updates of the menu entries, especially when the active line changes
void drawMainMenu(int selected_entry) {
  canvas.createSprite(MENU_W, MENU_LINE_H);
  canvas.setTextDatum(ML_DATUM); // mid left text datum
  bool hasEnterAction = mainMenuItems[selected_entry].enterAction != NULL; // Check if the current menu entry has an enter action
  bool hasSubMenu = (mainMenuItems[selected_entry].submenuStart >= 0) && (mainMenuItems[selected_entry].submenuEnd >= 0); // Check if the current menu entry has a submenu
  drawListEntry(mainMenuItems[selected_entry].menuHeader, 0, true, (currentMenuState == s_inmainmenu), hasEnterAction, hasSubMenu);
  canvas.drawRect(0, 0, MENU_W, MENU_LINE_H, borderColor);
  canvas.pushSprite(MENU_LEFT, MENU_TOP);
  canvas.deleteSprite();
}

// #############################################################################

// draw submenu with scrollable list of entries, the submenu is displayed in a box below the main menu line
// uses canvas sprite to ensure flicker-free updates of the menu entries, especially when the active line changes
void drawSubmenuSelect(int mainmenu_item, int enc_delta) {
  // bietet vorbefüllte Liste zur Auswahl an, z.B. Directory
  // liefert Eintrag zurück oder -1, wenn CANCEL gewählt wurde
  // strcpy(array[entry_count], "CANCEL"); // immer letzter Eintrag
	// int last_item_offset = item_offset;
	// int last_hilited_line = hilited_line;
  int entry_count = subMenuProperties[mainmenu_item].itemCount;

  int line_count = entry_count;
  if (line_count > SUBMENU_COUNT)
    line_count = SUBMENU_COUNT; // tatsächlich angezeigte Zeilen

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

  if (entry_count <= SUBMENU_COUNT) {
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
  drawSubMenuEntries(start_line, line_count, hilited_line);
}

#endif