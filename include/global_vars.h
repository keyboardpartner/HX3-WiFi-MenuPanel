#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <Arduino.h>
#include <LittleFS.h>
#include <Strings.h>
#include <EEPROM.h>
#include <time.h>
// #include <ArduinoJson.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>    // Importing the Adafruit_GFX library
// #include <Adafruit_ST7789.h> // Import the Adafruit_ST7789 library
// #include <LittleFS_ImageReader.h>
#include <TFT_eSPI.h>
#include <Ticker.h>

#define VERSION "FPGA Loader v0.1"
#define CREATOR "C.Meyer 3/2026  "

#define DEBUG
// #define DEBUG_VERBOSE
#define DEBUG_DF_TESTS
#define DEBUG_SPI_TESTS

// #define USE_DF // Testboard FireBeetle an XC3S500E-208

#ifdef DEBUG
  #define DPRINT(...)    Serial.print(__VA_ARGS__)
  //OR, #define DPRINT(args...)    Serial.print(args)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
  #define DPRINTF(...)    Serial.print(F(__VA_ARGS__))
  #define DPRINTLNF(...) Serial.println(F(__VA_ARGS__)) //printing text using the F macro
#else
  #define DPRINT(...)     //blank line
  #define DPRINTLN(...)   //blank line
  #define DPRINTF(...)    //blank line
  #define DPRINTLNF(...)  //blank line
#endif

#ifdef DEBUG_VERBOSE
  #define VERBOSE_DPRINT(...)    Serial.print(__VA_ARGS__)
  //OR, #define VERBOSE_DPRINT(args...)    Serial.print(args)
  #define VERBOSE_DPRINTLN(...)  Serial.println(__VA_ARGS__)
  #define VERBOSE_DPRINTF(...)    Serial.print(F(__VA_ARGS__))
  #define VERBOSE_DPRINTLNF(...) Serial.println(F(__VA_ARGS__)) //printing text using the F macro
#else
  #define VERBOSE_DPRINT(...)     //blank line
  #define VERBOSE_DPRINTLN(...)   //blank line
  #define VERBOSE_DPRINTF(...)    //blank line
  #define VERBOSE_DPRINTLNF(...)  //blank line
#endif

// bei Änderung der Settings-Struktur auch diesen Wert ändern:
#define SETTINGS_VALIDFLAG 0x55A1

// https://rgbcolorpicker.com/565
#define TFT_BTNGREY  0x736e    // 16-bit colour, RGB565 format
#define TFT_MEDGREY  0x7BEF
#define TFT_DIALOGGREY TFT_BTNGREY     // Window background color
#define TFT_DIMMED   0x528A
#define TFT_CHARCOAL 0x3186
#define TFT_BORDER   0xCE59
#define TFT_EDITCOLOR TFT_GREEN

// https://botland.store/arduino-compatible-boards-dfrobot/9153-dfrobot-firebeetle-esp32-iot-wi-fi-bluetooth-6959420912155.html
// benutzbare Pins: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

#ifdef PANEL_ESP
  #define ENC_A 34
  #define ENC_B 39
  #define BTN_UP 36
  #define BTN_DOWN 19
  #define BTN_ENTER 35
  #define LED_PIN 12
  #define _LED_ON digitalWrite(LED_PIN, LOW)  // LED an
  #define _LED_OFF digitalWrite(LED_PIN, HIGH) // LED aus

  #include <Encoder.h>

  //Define the size of the screen
  #define DISPLAY_W 320
  #define DISPLAY_H  170
  #define DISPLAY_CENTER_X  160
  #define DISPLAY_CENTER_Y  85

  // Define the pins of the ESP32 connected to the LCD - DEFINED IN PLATFORMIO.INI
  // #define LCD_MOSI 23  // SDA Pin on ESP32 D23
  // #define LCD_SCLK 18  // SCL Pin on ESP32 D18
  // #define LCD_CS   15  // Chip select control pin on ESP32 D15
  // #define LCD_DC    2  // Data Command control pin on ESP32 D2
  // #define LCD_RST   4  // Reset pin (could connect to RST pin) on ESP32 D4
  // #define LCD_BLK  32  // Black Light Pin on ESP32 D32

  #define ESP_MISO 16   // HSPI_MISO, an FPGA P126
  #define ESP_MOSI 13   // HSPI_MOSI, an FPGA P127
  #define ESP_SCK  14   // HSPI_SCLK, an FPGA P132
  #define ESP_RS 5      // Register Select, an FPGA P128
  #define ESP_DS 17     // Data Select, an FPGA P129
  #define ESP_595 27    // HC595 DCLK
  
  //Create the Adafruit_ST7789 object
  //  Adafruit_ST7789 tft = Adafruit_ST7789(LCD_CS, LCD_DC, LCD_RST);
  TFT_eSPI tft = TFT_eSPI();       // Invoke custom library as global
  TFT_eSprite canvas = TFT_eSprite(&tft);  // Declare Sprite object "canvas" with pointer to "tft" object

  Encoder encoder(BTN_ENTER, BTN_UP, BTN_DOWN, ENC_A, ENC_B);
  Ticker encoderTicker;
#else
  #define LED_PIN LED_BUILTIN
  #define _LED_ON digitalWrite(LED_PIN, HIGH)  // LED an (bei FireBeetle LOW aktiv)
  #define _LED_OFF digitalWrite(LED_PIN, LOW) // LED aus (bei FireBeetle LOW aktiv)

  #define ESP_MISO 16   // HSPI_MISO, an FPGA P126
  #define ESP_MOSI 13   // HSPI_MOSI, an FPGA P127
  #define ESP_SCK 14    // HSPI_SCLK, an FPGA P132
  #define ESP_RS 4      // Register Select, an FPGA P128
  #define ESP_DS 5      // Data Select, an FPGA P129
#endif


#define _DS_ON digitalWrite(ESP_DS, LOW);
#define _DS_OFF digitalWrite(ESP_DS, HIGH);
#define _RS_ON digitalWrite(ESP_RS, LOW);
#define _RS_OFF digitalWrite(ESP_RS, HIGH);
// Dataflash externer SPI-Flash-Speicher, über _DS-Pin
#define _DF_ON  digitalWrite(ESP_DS, LOW)  // Data Select LOW für Daten
#define _DF_OFF digitalWrite(ESP_DS, HIGH) // Data Select HIGH für Daten-Ende

#define F_DATEREG 3      // SPI Date Register

#define DF_BLOCKSIZE 4096 // Buffer/Blockgröße in Bytes


static const int spiClk = 10000000;  // 10 MHz
SPISettings spiSettings(spiClk, MSBFIRST, SPI_MODE3);

enum {ERR_DF_OK = 0, 
      ERR_DF_ERASE = 1, 
      ERR_DF_WRITE = 2,
      ERR_DF_FILE = 4, 
      ERR_DF_VERIFY = 8
}; // Fehlercodes für DataFlash-Funktionen

enum {ERR_CMD_OK = 0, 
      ERR_NOACK = 1, 
      ERR_NOTREAD = 2,
      ERR_NODATA = 3,
      ERR_STATUS = 4,
      ERR_INVFLAG = 5,
      ERR_CHKSUM = 6
} cmderrs; // Fehlercodes für Upload-Funktionen

typedef uint8_t err_df_t; // Fehlercodes für DataFlash-Funktionen
typedef uint8_t err_cmd_t; // Fehlercodes für Commands

// Voreinstellungen und Skalierungen, als Credentials gespeichert
struct {
  char ssid[32] = "KeyboardPartner";      // Default Router SSID FCKAFD
  char password[32] = "z28hev111";  // Default Router PW
  // char ssid[32] = "WILHELM.TEL-Z7XLWCE1";      // Default Router SSID
  // char password[32] = "15456528653098957079";  // Default Router PW
  // char ssid[32] = "ImpfChipC19";      // Default Router SSID
  // char password[32] = "z28hev111";  // Default Router PW
  uint16_t autoUpload = 1;
  uint16_t wifiMode = 1; // OFF, WIFI (STA) oder WIFI (AP)
 } settings;

bool resetRequested = false; // Flag, um einen Reset anzufordern, z.B. über die Weboberfläche, wird in loop() abgefragt und ggf. zurückgesetzt
char confDirectory[16][32]; // Verzeichnis auf SPIFFS, max. 16 Dateien mit max. 32 Zeichen Länge, wird bei jedem Zugriff auf die Weboberfläche aktualisiert
int confDirectoryCount = 0; // Anzahl der Dateien im Verzeichnis, wird bei jedem Zugriff auf die Weboberfläche aktualisiert

char pbDirectory[16][32]; // Verzeichnis auf SPIFFS, max. 16 Dateien mit max. 32 Zeichen Länge, wird bei jedem Zugriff auf die Weboberfläche aktualisiert
int pbDirectoryCount = 0; // Anzahl der Dateien im Verzeichnis, wird bei jedem Zugriff auf die Weboberfläche aktualisiert
uint32_t fpgaDate = 0;

int8_t hx3EditArray[512];
int8_t hx3ExtendedArray[1536];

enum {
  s_inmainmenu, 
  s_insubmenu, 
  s_invaluechange
}; // Index-Bereich der Hauptmenü-Items in MenuItems-Tabelle

int currentMenuState = s_inmainmenu; // Aktueller Menü-State, z.B. für Anzeige auf LCD
int previousMenuState = s_inmainmenu; // Vorheriger Menü-State, z.B. für Anzeige auf LCD
int mainMenuItem = 0; // Aktuell ausgewähltes Menü-Item, z.B. für Anzeige auf LCD
int activeMenuItem = 0; // Aktuell ausgewähltes Menü-Item, z.B. für Anzeige auf LCD

// Bei mehr als 127 Menüpunkten müssen die Datentypen in menuEntryType angepasst werden
enum {
  tm_none,
  tm_preset,
  tm_numeric, 
  tm_drawbar,
  tm_pot,
  tm_tab,
  tm_vibknob,
  tm_phrknob,
  tm_waveset,
  tm_tapering,
  tm_gating,
  tm_organ,
  tm_speaker,
  tm_midicc,
  tm_halfmoon,
  tm_button,
  tm_wifimode,
  tm_cancel
}; // Index-Bereich der Hauptmenü-Items in MenuItems-Tabelle


// Action-Routine über Tabelle, ohne Parameter
// typedef void (*action)(uint16_t line_color, int16_t delta);
typedef void (*action)();

typedef struct {
  char menuHeader[16];
  int16_t submenuStart;
  int16_t submenuEnd;
  int16_t editArrayIdx;
  action enterAction; // wenn Wert mit ENTER bestätigt wird, z.B. um die Änderung zu übernehmen, z.B. FPGA-Register schreiben oder Datei auswählen
  int8_t displayType; // display and edit type, e.g. numeric, drawbar, tab, etc. to decide how to display and edit the value
  int8_t menuValueMin;
  int8_t menuValueMax;
  int8_t extdRequired; // Flag, ob für die Bearbeitung dieses Menüeintrags die Extended License benötigt wird
} menuEntryType;

// Um RAM zu sparen, werden nur das aktuelle Menü, die zugehörigen Submenüs 
// und die Werte der Menüeinträge im RAM gehalten, 
// alle anderen Menütexte und -strukturen werden bei Bedarf aus PROGMEM gelesen.

#define MAX_MAINMENU_ITEMS 24
#define MAX_SUBMENU_ITEMS 32

menuEntryType mainMenuItems[MAX_MAINMENU_ITEMS], subMenuItems[MAX_SUBMENU_ITEMS];
menuEntryType currentMenuEntry; // aktueller extrahierter Menüpunkt

// Index: MainMenuItem
typedef struct {
  int16_t startIndex; // Startindex des Submenüs in der MenuItems-Tabelle
  int16_t itemCount;
  int16_t itemIndex;
  int16_t hilitedLine;
  int16_t startLine;
} subMenuPropertyType;

subMenuPropertyType subMenuProperties[MAX_MAINMENU_ITEMS]; // letzte Positionen der Submenüeinträge

// -----------------------------EEPROM-DEFAULTS-----------------------------------

/* Store WLAN credentials to "EEPROM" */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, settings);
  EEPROM.put(500, SETTINGS_VALIDFLAG);
  EEPROM.commit();
  EEPROM.end();
  Serial.println(F("Save Credentials to EEPROM"));
}

/* Load WLAN credentials from "EEPROM" or use predefined strings */
void loadCredentials() {
  EEPROM.begin(512);
  uint16_t validFlag;
  EEPROM.get(500, validFlag);
  if (validFlag == SETTINGS_VALIDFLAG) {
    EEPROM.get(0, settings);
    EEPROM.end();
    Serial.println(F("Load Credentials from EEPROM"));
  } else {
    EEPROM.end();
    saveCredentials(); // vorbelegte Werte speichern
  }
}

// #############################################################################
//
//    ######## ########   ######      ###        ##        ######  
//    ##       ##     ## ##    ##    ## ##       ##       ##    ## 
//    ##       ##     ## ##         ##   ##      ##       ##       
//    ######   ########  ##   #### ##     ##     ##       ##       
//    ##       ##        ##    ##  #########     ##       ##       
//    ##       ##        ##    ##  ##     ##     ##       ##    ## 
//    ##       ##         ######   ##     ##     ########  ######  
//
// #############################################################################

// Load Core (LC) Target-Nummern
// Größere Datenmengen werden nicht als SPI-Register egesetzt, sondern an
// einen "LoadCore"-Buffer mit Auto-Inkrement im FPGA übertragen,
// siehe FPGA_Hilevel.h
#define LCTARGET_SCAN_DRIVER 0  // aus DataFlash
#define LCTARGET_TAPERING 1  // aus DataFlash
#define LCTARGET_FIR_COEFF 2  // aus DataFlash
#define LCTARGET_KEYMAP 3  // berechnet
#define LCTARGET_WAVESET 4  // aus DataFlash
#define LCTARGET_TUNING_VALS 5  // berechnet
#define LCTARGET_HP_FILTER 6  // berechnet
#define LCTARGET_TUBE_AMP_SLOPE 7  // berechnet
#define LCTARGET_UPPER_DRAWBARS 8  // berechnet
#define LCTARGET_LOWER_DRAWBARS 9  // berechnet
#define LCTARGET_PEDAL_DRAWBARS 10  // berechnet
#define LCTARGET_ADSR_UPPER 11  // berechnet
#define LCTARGET_ADSR_LOWER 12  // berechnet
#define LCTARGET_ADSR_PEDAL 13  // berechnet

#define LC_NONE 255

// Das Dataflash enthält nicht nur dass FPGA-Binary, sondern
// auch diverse Daten wie Scan Driver, Taperings etc.,
// die über den "LoadCore"-Mechanismus an die FPGA-Cores übertragen werden
// Block-Nummern im DataFlash ab 0x3B0, siehe FPGA_Hilevel.h
#define BLOCK_FPGA 0  // XC6S25 Binary, 196 Blöcke
#define BLOCK_FIRMWARE 0  // Direkt in MCU-Flash!
#define BLOCK_FAILSAFE_BASE 320  // Sicherungskopien
#define BLOCK_FAILSAFE_FPGA 320  // Sicherungskopie FPGA
#define BLOCK_FAILSAFE_FW 640    // Sicherungskopie Firmware
#define BLOCK_UPDATE_INFO 637    // DFUDL Update List
#define BLOCK_BOARD_INFO 639     // 
#define BLOCK_SPEAKER_MODEL_BASE 768  // 16 Blöcke
#define BLOCK_ORGAN_MODEL_BASE 784  // 16 Blöcke
#define BLOCK_PRESET_BASE 800  // 100 Blöcke
#define BLOCK_MIDI_CC_BASE 928  // 16 Blöcke
#define BLOCK_CCSET_0 928  // 1 Block
#define BLOCK_CCSET_1 929  // 2 Block
#define BLOCK_CCSET_2 930  // 3 Block
#define BLOCK_CCSET_3 931  // 4 Block
#define BLOCK_CCSET_4 932  // 5 Block
#define BLOCK_CCSET_5 933  // 6 Block
#define BLOCK_CCSET_6 934  // 7 Block
#define BLOCK_CCSET_7 935  // 8 Block
#define BLOCK_CCSET_8 936  // 9 Block
#define BLOCK_CCSET_9 937  // 10 Block
#define BLOCK_CCSET_10 938  // 11 Block
#define BLOCK_CORE_BASE 944  // 
#define BLOCK_SCAN 944  // 
#define BLOCK_VOICE 946  // 
#define BLOCK_DEFAULTS 947  // HX3 Edit Array
#define BLOCK_EEPROM 953  // 
#define BLOCK_TAPER_BASE 955  // 4 Taperings
#define BLOCK_TAPER_0 955  // 
#define BLOCK_TAPER_1 956  // 
#define BLOCK_TAPER_2 957  // 
#define BLOCK_TAPER_3 958  // 
#define BLOCK_FIR_COEFF 959  // Filterkoeffizienten
#define BLOCK_WAVESET_BASE 960  // 8 Wavesets
#define BLOCK_WAVESET_0 960  // 
#define BLOCK_WAVESET_1 964  // 
#define BLOCK_WAVESET_2 968  // 
#define BLOCK_WAVESET_3 972  // 
#define BLOCK_WAVESET_4 976  // 
#define BLOCK_WAVESET_5 980  // 
#define BLOCK_WAVESET_6 984  // 
#define BLOCK_WAVESET_7 988  // 

#define BLOCK_DSP_DFU 0x3D00  // = 15.616 dez. = 0x3D00000 / 4096

// Achtung: DFU-Ranges in DSP-Firmware sind Wort-Adressen!
#define DSP_FW_START  0x0040000 // Adresse im Dataflash, ab der die DSP-Firmware gespeichert wird, @262.144
#define DSP_FW_END    0x00FFFFF // Ende der DSP-Firmware, @1.048.575
#define DSP_FW_LEN    0x00C0000 // Länge der DSP-Firmware im Dataflash, 786.432 Bytes, also 192 Blöcke zu je 4kB oder 3 Blöcke zu je 256kB, je nach Chip

#define DSP_GM_START  0x0100000 // Start der GM-Soundbank im Dataflash, @1.048.576
#define DSP_GM_END    0x09FFFFF // Ende der GM-Soundbank im Dataflash, @1.048.576
#define DSP_EXT_START 0x0A00000 // Start der extended-Soundbank im Dataflash, @10.485.760
#define DSP_EXT_END   0x3CFFFFF // Ende der extended-Soundbank im Dataflash, @62.914.047

#define DSP_DFU_START 0x3D00000 // Start der DFU-Sektion im Dataflash, @63.963.136
#define DSP_DFU_END   0x3FFFFFF // Ende der DFU-Sektion im Dataflash, bis 67.108.863, also 3.145.728 Bytes für DFU
#define DSP_DFU_LEN (DSP_DFU_END - DSP_DFU_START + 1) // Länge der DFU-Sektion im Dataflash, 3.145.728 Bytes

#define DSP_DFU_BLK (DSP_DFU_START/4096) // Start der DFU-Sektion im Dataflash, @63.963.136


void blinkLED(uint8_t times) {
  // Board-LED blinkt zur Bestätigung von Aktionen, z.B. Speichern von Werten im EEPROM
  for (uint8_t i=0; i<times; i++) {
    digitalWrite(LED_PIN, LOW); // sets the LED on
    delay(150);
    digitalWrite(LED_PIN, HIGH);  // sets the LED off
    delay(150);
  }
}

uint8_t mulDivByte(uint8_t value, uint8_t mul, uint8_t div) {
  // Für AVRco-Kompatibilität, oft genutzt
  uint16_t temp = (uint16_t)value;
  temp = (temp * mul) / div;
  return temp;
}

int16_t mulDivInt(int16_t value, int16_t mul, int16_t div) {
  // Für AVRco-Kompatibilität, oft genutzt
  uint32_t temp = (uint32_t)value;
  temp = (temp * mul) / div;
  return temp;
}

void dividerLine() {
  Serial.println(F("----------------------------------------"));
}

#define OFS_SYSTEMINITS 496
#define OFS_VIBKNOBMODE 497
#define OFS_RESTORECOMMONPRESETMASK 498
#define OFS_BUTTONMASK0 499
#define OFS_BUTTONMASK1 500
#define OFS_CONFBITS1 501
#define OFS_CONFBITS2 502
#define OFS_ADCCONFIG 503
#define OFS_1STDBSELECT 504
#define OFS_2NDDBSELECT 505
#define OFS_PEDALDBSETUP 506
#define OFS_ADCSCALING 507

#define OFS_DEVICETYPE 509
#define OFS_PRESETSTRUCTURE 510
#define OFS_EDITMAGICFLAGIDX 511


#endif  // GLOBALVARS_H