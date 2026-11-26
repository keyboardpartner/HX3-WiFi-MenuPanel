#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <Arduino.h>
#include <LittleFS.h>
#include <Strings.h>
#include <EEPROM.h>
#include <time.h>
// #include <ArduinoJson.h>
// #include <Wire.h>
#include <TFT_eSPI.h>
#include <Ticker.h>
#include "time.h"
#include "Wire.h"

#define I2C_DEV_ADDR 0x21

#define VERSION "FPGA Loader v0.1"
#define CREATOR "C.Meyer 3/2026  "

#define ENABLE_SCREENSAVER
#define USE_WIFI
// bei Änderung der Settings-Struktur auch diesen Wert ändern:
#define SETTINGS_VALIDFLAG 0x55A3

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


// https://rgbcolorpicker.com/565
#define TFT_BTNGREY  0x736e    // 16-bit colour, RGB565 format
#define TFT_MEDGREY  0x8c51
#define TFT_DIALOGGREY TFT_MEDGREY     // Window background color
#define TFT_DIMMED   0x528A
#define TFT_CHARCOAL 0x39a7
#define TFT_SHADOW   0x4a49
#define TFT_BORDER   0xCE59
#define TFT_INACTIVE   0xd6ba // inactive knobs
#define TFT_EDITCOLOR 0x07e8 // TFT_GREEN

#define TFT_HEADERCOLOR   0x29dc
#define TFT_HEADERCOLOR_GRAD   0x10ec

#define SAVE_TIMEOUT 1000
#define MSG_DISPLAY_TIME 1500

// Size of the screen defined in PLATFORMIO.INI
#define DISPLAY_W  TFT_HEIGHT
#define DISPLAY_H  TFT_WIDTH

#ifdef NV3007_DRIVER
  #define DISPLAY_H_OFFS  0
  #define TFT_ROTATION 1
#else
  // Nudges down menus on screen if needed
  #define DISPLAY_H_OFFS  0
  #define TFT_ROTATION 3
#endif
#define DISPLAY_CENTER_X  (DISPLAY_W / 2)
#define DISPLAY_CENTER_Y  (DISPLAY_H_OFFS + DISPLAY_H / 2)

  // Define the pins of the ESP32 connected to the LCD - DEFINED IN PLATFORMIO.INI
  // #define LCD_MOSI 23  // SDA Pin on ESP32 D23
  // #define LCD_SCLK 18  // SCL Pin on ESP32 D18
  // #define LCD_CS   15  // Chip select control pin on ESP32 D15
  // #define LCD_DC    2  // Data Command control pin on ESP32 D2
  // #define LCD_RST   4  // Reset pin (could connect to RST pin) on ESP32 D4
  // #define LCD_BLK  32  // Black Light Pin on ESP32 D32

// https://botland.store/arduino-compatible-boards-dfrobot/9153-dfrobot-firebeetle-esp32-iot-wi-fi-bluetooth-6959420912155.html
// benutzbare Pins: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/


#define ENC_A 34
#define ENC_B 39
#define BTN_UP 36
#define BTN_DOWN 33
#define BTN_ENTER 35
#define LED_PIN 25

#include <Encoder.h>

#define ESP_MISO 16   // HSPI_MISO, an FPGA P126
#define ESP_MOSI 13   // HSPI_MOSI, an FPGA P127
#define ESP_SCK  14   // HSPI_SCLK, an FPGA P132
#define ESP_RS 17      // Register Select, an FPGA P128
#define ESP_DS 5     // Data Select, an FPGA P129
#define ESP_595_STROBE 27  // HC595 DCLK
// #define ESP_ROTRY_LED 26   // PWM-LED
#define ESP_PWM 12   // 595 Output Enable

// #define _ROTRY_LED_ON digitalWrite(ESP_ROTRY_LED, LOW)  // LED an
// #define _ROTRY_LED_OFF digitalWrite(ESP_ROTRY_LED, HIGH) // LED aus

//Create the Adafruit_ST7789 object
//  Adafruit_ST7789 tft = Adafruit_ST7789(LCD_CS, LCD_DC, LCD_RST);
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library as global
TFT_eSprite canvas = TFT_eSprite(&tft);  // Declare Sprite object "canvas" with pointer to "tft" object

Encoder encoder(BTN_ENTER, BTN_UP, BTN_DOWN, ENC_A, ENC_B);
Ticker encoderTicker;
Ticker delayTicker;


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

enum {
  ERR_DF_OK = 0, 
  ERR_DF_ERASE = 1, 
  ERR_DF_WRITE = 2,
  ERR_DF_FILE = 4, 
  ERR_DF_VERIFY = 8
}; // Fehlercodes für DataFlash-Funktionen

enum {
  ERR_CMD_OK = 0, 
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

#define MY_TIMEZONE "CET-1CEST,M3.5.0/02,M10.5.0/03" // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define MY_NTP_SERVER "de.pool.ntp.org"
time_t now;    // this is the epoch
struct tm *timeinfo;

struct {
  char ssid[32] = "KeyboardPartner";      // Default Router SSID FCKAFD
  char password[32] = "z28hev111";  // Default Router PW
  // char ssid[32] = "WILHELM.TEL-Z7XLWCE1";      // Default Router SSID
  // char password[32] = "15456528653098957079";  // Default Router PW
  // char ssid[32] = "ImpfChipC19";      // Default Router SSID
  // char password[32] = "z28hev111";  // Default Router PW
  uint16_t autoUpload = 1;
  uint16_t wifiMode = 1; // OFF, WIFI (STA) oder WIFI (AP)
  uint16_t screenSaver = 1; // OFF, ON
 } settings;

struct {
  uint32_t Valid;       // True wenn initialisiert
  float FirmwareVersion;
  uint32_t FPGAsernum;
  uint32_t FPGAversion;
  uint32_t LicenceOrgan;
  uint32_t LicenceExtd;
  uint8_t ScanVersion;
  uint8_t ScanRevision;
  uint16_t Dummy;
  uint32_t FPGAloaded;  // FPGA OK - bool funktioniert hier nicht!
  uint32_t LicenceOrganValid;
  uint32_t LicenceExtValid;
  uint32_t ScanValid;
  uint16_t DSPversion;
  char Username[16];
  uint32_t InitFlag;     // $AA554B50 = c_magic_flag_DW wenn initialisiert
} boardInfo;

char confDirectory[16][32]; // Verzeichnis auf SPIFFS, max. 16 Dateien mit max. 32 Zeichen Länge, wird bei jedem Zugriff auf die Weboberfläche aktualisiert
int confDirectoryCount = 0; // Anzahl der Dateien im Verzeichnis, wird bei jedem Zugriff auf die Weboberfläche aktualisiert
uint32_t msgTimeoutEndTime = 0; // Timeout für die Anzeige von Meldungen auf dem Display, wird bei Bedarf gesetzt und in loop() abgefragt
bool msgTimeoutActive = false; // Flag, ob gerade ein Meldungs-Timeout aktiv ist, wird bei Bedarf gesetzt und in loop() abgefragt
uint32_t fpgaDate = 0;
bool boardInfoDone = false; // Flag, ob die Board-Info bereits empfangen und angezeigt wurde
uint8_t hx3EditArray[512];
uint8_t hx3ExtendedArray[1536];
char hx3PresetName[16]; // Array für den Namen des Presets, wird bei Bedarf aus dem Edit-Array gefüllt

enum {
  s_inmainmenu, 
  s_insubmenu
}; // Index-Bereich der Hauptmenü-Items in MenuItems-Tabelle

int currentMenuState = s_inmainmenu; // Aktueller Menü-State, z.B. für Anzeige auf LCD
int mainMenuItem = 0; // Aktuell ausgewähltes Menü-Item, z.B. für Anzeige auf LCD
int activeMenuItem = 0; // Aktuell ausgewähltes Menü-Item, z.B. für Anzeige auf LCD
bool editingOn = false; // Flag, ob gerade ein Wert editiert wird, z.B. für Anzeige auf LCD

// Status von ESP32 an MCU Bootloader
// Bit 4, 5, 6: Button State (1 = pressed, 0 = released)
enum {
  TWI_BLMENU_STARTFDU = 4,
  TWI_BLMENU_RESTORE_FW = 5,
  TWI_BLMENU_RESTORE_FPGA = 6, 
  TWI_BLMENU_SDCARD_UPD = 7, 
  TWI_BLMENU_EXIT = 8, 

  TWI_BLMENU_NONE = 9, 
  TWI_BLMENU_BUSY    = 10,
  TWI_BLMENU_BOOTING = 11,  
} twiStatus; // I2C-Befehle vom Master bzw. MCU Bootloader an den ESP32

enum {
  // Befehle/Status Bootloader an ESP32
  TWI_ESPCMD_SETIDLE    = 0x30,  // Setzen des Bootloader-Idle-Status 
  TWI_ESPCMD_SETBUSY     = 0x31,  // Setzen des Bootloader-Busy-Status 

  // In Value-Box angezeigt
  TWI_ESPCMD_DRAW_DFU  = 0x40,
  TWI_ESPCMD_DRAW_FWRECOVER  = 0x41,
  TWI_ESPCMD_DRAW_FPGARECOVER  = 0x42,
  TWI_ESPCMD_DRAW_SDLOAD  = 0x43,
  TWI_ESPCMD_DRAW_SDERR  = 0x44,
  TWI_ESPCMD_DRAW_DFU_RCVD  = 0x45,
  TWI_ESPCMD_DRAW_WIFI_RCVD  = 0x46,

  TWI_ESPCMD_DRAW_BLBOX   = 0x4C, // mit String-Nachricht
  TWI_ESPCMD_DRAW_UNPACK_NUM  = 0x4D, // with number!
  TWI_ESPCMD_DRAW_UNPACK  = 0x4E, // generic msg for unpacking
  TWI_ESPCMD_DRAW_DONE  = 0x4F,

  TWI_ESPCMD_DRAW_INFOMSG = 0x50,
  TWI_ESPCMD_DRAW_REQMSG  = 0x51,
  TWI_ESPCMD_DRAW_ERRMSG  = 0x52,

  TWI_ESPCMD_GETSTATUS   = 0x53,  // Status-Register-Auswahl, Lese Statusbyte
  TWI_ESPCMD_GETBUTTONS  = 0x54,  // Button-Register-Auswahl, Lese Button-Byte
  TWI_ESPCMD_SETSTATUS   = 0x55,  // Setzen des Bootloader-Status 
  
  TWI_ESPCMD_BLSTART     = 0x57,
  TWI_ESPCMD_BLERR_FPGA  = 0x58,  // FPGA corrupted Error Message
  TWI_ESPCMD_BLERR_FW    = 0x59,  // Firmware corrupted Error Message

  TWI_ESPCMD_RELOAD   = 0x5B,  // Reload edit data
  TWI_ESPCMD_BOARDINFO   = 0x5C,  // Board-Info als Datensatz
  TWI_ESPCMD_BLEXIT      = 0x5E,  // Bootloader verlassen, zurück zu normaler Anzeige
  TWI_ESPCMD_RESET       = 0x5F,

} twiVals; // I2C-Befehle vom Master bzw. MCU Bootloader an den ESP32

struct {
  uint8_t rxBuffer[96]; // Buffer für I2C-Receive
  uint8_t txBuffer[16]; // not used for now
} esp32twi; // Struktur für den aktuellen Befehl, der über I2C empfangen oder gesendet wird

uint8_t BtnMenuStatus= TWI_BLMENU_BOOTING; // Kodierte Statusinformation inkl. Buttons und Bootloader-Status
bool bootloaderActive = false; // Flag, ob sich die MCU ST32F im Bootloader-Modus befindet
bool bootloaderBusy = false;   // Flag, ob die MCU ST32F im Bootloader-Modus beschäftigt ist; sperrt Buttons und Encoder

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
  tm_midich,
  tm_midicc,
  tm_halfmoon,
  tm_button,   // Enter-Klick sofort oder mit timeout
  tm_button_timeout, // zeigt "Press 2 sec" an
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
  action timeoutAction; // wenn Wert mit Timeout bestätigt wird, z.B. um die Änderung zu übernehmen, z.B. FPGA-Register schreiben oder Datei auswählen
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

struct {
  bool reset = false;
  bool redrawOrgan = false;
  bool reloadOrgan = false;
  bool reloadExtended = false;
  bool reconnect = false;
  bool uploadEnded = false;
} requestSema; // Struktur für den aktuellen Befehl, der über I2C empfangen oder gesendet wird

// Zähler, wird bei jedem vom HX3 empfangenen Edit-Array erhöht. Die Webseiten fragen ihn per
// Polling ab und aktualisieren ihre Anzeige, sobald er sich ändert.
volatile uint16_t editArrayRev = 0;


// -----------------------------EEPROM-DEFAULTS-----------------------------------

// Note: Wifi Mode is made editable by placing in HX3 EditArray at index 409
// and is referenced by MENU_WIFIMODE (205) in the main menu
#define EDIT_IDX_SCREENSAVER 408
#define EDIT_IDX_WIFIMODE 409

/* Store WLAN credentials to "EEPROM" */
void saveCredentials() {
  // get editable items from HX3 EditArray (dummy indexes) and store them in settings
  settings.wifiMode = hx3EditArray[EDIT_IDX_WIFIMODE]; // was editable item
  settings.screenSaver = hx3EditArray[EDIT_IDX_SCREENSAVER]; // was editable item
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
  // make settings editable items by placing them into HX3 EditArray (dummy indexes) 
  hx3EditArray[EDIT_IDX_WIFIMODE] = settings.wifiMode; // make editable item
  hx3EditArray[EDIT_IDX_SCREENSAVER] = settings.screenSaver; // make editable item
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

uint16_t getParamValue(int16_t param) {
  // Get the value of an (extended) parameter based on its arbitrary number
  if (param >= 1000 && param <= 1511) {
    param -= 1000;
    return hx3EditArray[param];
  } else if (param >= 2000 && param <= 2099) {
    // eep_SAM_RevDSPvals (ab Index 192 im Extended Array)
    param -= 2000;
    return hx3ExtendedArray[param + 192];
  } else if (param >= 2100 && param <= 2199) {
    // eep_LeslieInits (ab Index 128 im Extended Array)
    param -= 2100;
    return hx3ExtendedArray[param + 128];
  } else if (param >= 2500 && param <= 2699) {
    // eep_PhasingRotorSets (ab Index 0 im Extended Array)
    param -= 2500;
    return hx3ExtendedArray[param];
  } else if (param >= 3000 && param <= 3399) {
    // eep_PedalDBfacs (ab Index 216 im Extended Array)
    param -= 3000;
    return hx3ExtendedArray[param + 216];
  } else if (param >= 5000 && param <= 5099) {
    // eep_AnalogRemaps (ab Index 768 im Extended Array)
    param -= 5000;
    return hx3ExtendedArray[param + 768]; 
  } else if (param >= 5100 && param <= 5199) {
    // eep_ButtonRemaps (ab Index 1024 im Extended Array) 
    param -= 5100;
    return hx3ExtendedArray[param + 1024];
  } else if (param >= 5200 && param <= 5299) {
    // eep_ButtonModes (ab Index 1152 im Extended Array) 
    param -= 5200;
    return hx3ExtendedArray[param + 1152];
  } else if (param >= 5300 && param <= 5331) {
    // eep_ButtonRemaps_XB2 (ab Index 1280 im Extended Array), auch SwitchRemaps 5316..5331 
    param -= 5300;
    return hx3ExtendedArray[param + 1280];
  } else if (param >= 5400 && param <= 5499) {
    // eep_OrganModelAssignments (ab Index 1312 im Extended Array)
    param -= 5400;
    return hx3ExtendedArray[param + 1312];
  } else if (param >= 5500 && param <= 5599) {
    // eep_SpeakerModelAssignments (ab Index 1328 im Extended Array) 
    param -= 5500;
    return hx3ExtendedArray[param + 1328];
  } else if (param >= 6000 && param <= 6199) {
    // eep_MenuEnables (ab Index 512 im Extended Array) 
    param -= 6000;
    return hx3ExtendedArray[param + 512];
  }
  return 0;
}

void setParamValue(int16_t param, uint16_t value) {
  // set value in the appropriate array based on the arbitrary parameter number
  DPRINTF("Set ");
  DPRINT(param);
  DPRINTF("=");
  DPRINTLN(value);
  if (param >= 1000 && param <= 1511) {
    param -= 1000;
    hx3EditArray[param] = value;
    if (param < 272) { 
      // Knobs, Tabs & Drawbars only
      editArrayRev++; // Webseiten per Polling informieren, dass sich ein Wert geändert hat
      requestSema.redrawOrgan = true;
    }
  } else if (param >= 2000 && param <= 2099) {
    param -= 2000;
    hx3ExtendedArray[param + 192] = value;
  } else if (param >= 2100 && param <= 2199) {
    param -= 2100;
    hx3ExtendedArray[param + 128] = value;
  } else if (param >= 2500 && param <= 2699) {
    param -= 2500;
    hx3ExtendedArray[param] = value;
  } else if (param >= 3000 && param <= 3399) {
    param -= 3000;
    hx3ExtendedArray[param + 216] = value;
  } else if (param >= 5000 && param <= 5099) {
    param -= 5000;
    hx3ExtendedArray[param + 768] = value;
  } else if (param >= 5100 && param <= 5199) {
    param -= 5100;
    hx3ExtendedArray[param + 1024] = value;
  } else if (param >= 5200 && param <= 5299) {
    param -= 5200;
    hx3ExtendedArray[param + 1152] = value;
  } else if (param >= 5300 && param <= 5331) {
    param -= 5300;
    hx3ExtendedArray[param + 1280] = value;
  } else if (param >= 5400 && param <= 5499) {
    param -= 5400;
    hx3ExtendedArray[param + 1312] = value;
  } else if (param >= 5500 && param <= 5599) {
    param -= 5500;
    hx3ExtendedArray[param + 1328] = value;
  } else if (param >= 6000 && param <= 6199) {
    param -= 6000;
    hx3ExtendedArray[param + 512] = value;
  }
}
#endif  // GLOBALVARS_H