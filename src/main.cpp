// #############################################################################
//
//       __ ________  _____  ____  ___   ___  ___
//      / //_/ __/\ \/ / _ )/ __ \/ _ | / _ \/ _ \
//     / ,< / _/   \  / _  / /_/ / __ |/ , _/ // /
//    /_/|_/___/_  /_/____/\____/_/_|_/_/|_/____/
//      / _ \/ _ | / _ \/_  __/ |/ / __/ _ \
//     / ___/ __ |/ , _/ / / /    / _// , _/
//    /_/  /_/ |_/_/|_| /_/ /_/|_/___/_/|_|
//
// ESP32 Web Interface for HX3.6 and HX3.7 Mainboards
// (c) KeyboardPartner UG & C. Meyer 03/2026
//
// #############################################################################

#include <Arduino.h>
#include <LittleFS.h>
#include <SPI.h>
#include "global_vars.h"

#include "fpga_config.h"
#include "spi_xchange.h"

#include "dataflash.h"
#include "server.h"
#include "parser.h"
#include "menu_items.h"
#include "drawing.h"

void encoderCallback(int16_t delta) {
  // Hier können Sie die Logik implementieren, die bei einer Encoder-Aktion ausgeführt werden soll
  // Zum Beispiel könnten Sie hier die Menü-Navigation oder andere Funktionen basierend auf der Encoder-Bewegung implementieren
  DPRINTF("Encoder Delta:");
  DPRINTLN(delta);
  switch (currentMenuState) {
    case s_inmainmenu:
      mainMenuItem += delta;
      if (mainMenuItem < 0) mainMenuItem = 0;
      if (mainMenuItem > MAIN_MENU_END) mainMenuItem = MAIN_MENU_END;
      getSubMenuItems(mainMenuItem); // Submenü-Strings basierend auf der Auswahl im Hauptmenü aktualisieren
      drawMainMenu(mainMenuItem, (currentMenuState == s_inmainmenu), TFT_WHITE, TFT_BLACK);
      drawSubmenuSelect(mainMenuItem, 0, false, TFT_WHITE, TFT_BLACK);
      activeMenuItem = mainMenuItem; 
      getMenuEntry(&currentMenuEntry, activeMenuItem); // Aktuelle Menü-Entry-Daten in globalen Variablen aktualisieren
      break;
    case s_insubmenu:
      selectSubMenu(delta, true);// setzt activeMenuItem und currentMenuEntry
      break;
    case s_invaluechange:
      changeValue(&currentMenuEntry, delta); 
      break;
  }
  drawValue(&currentMenuEntry, (currentMenuState == s_invaluechange));
  drawOrgan(manualSelects[mainMenuItem]); // Zeichnet die Orgelgrafik, z.B. für die Anzeige der Registerbelegung oder ähnliches
}

// -----------------------------------------------------------------------------

// Up/Down-Buttons wechseln Menüeintrag, auch wenn im ValueChange-Mode
// Enter-Button Wechselt zwischen Menüeintrag und Value-Edit
// Doppelklick auf Enter-Button wechselt direkt zum Hauptmenü oder zum Untermenü, je nachdem
// Langer Druck auf Enter-Button löste die enterAction für diesen Menüpunkt aus

void buttonCallback(uint8_t button) {
  // Hier können Sie die Logik implementieren, die bei einer Button-Aktion ausgeführt werden soll
  // Zum Beispiel könnten Sie hier die Menü-Navigation oder andere Funktionen basierend auf der Button-Interaktion implementieren

  static uint32_t last_click_millis = 0; // Zeit des letzten Button-Klicks, um Doppelklicks oder langes Drücken zu erkennen
  bool timeout_occurred = (button & 0x80) != 0; // Flag, um zu verfolgen, ob der Autorepeat-Timeout erreicht wurde
  button = button & 0x07; // Nur die unteren 3 Bits verwenden, um die Button-Nummer zu bestimmen (1, 2 oder 4)

  if (button == 1) { // ENTER-Button
    uint32_t now = millis();
    bool double_clicked = (now - last_click_millis) < 500; // Flag, um zu verfolgen, ob ein Doppelklick erkannt wurde
    if (timeout_occurred) {
      // Enter-Timeout erreicht, enterAction() ausführen
      DPRINTLNF("Long press detected");
      if (currentMenuEntry.enterAction != nullptr) {
        refreshMainPage(true);
        currentMenuEntry.enterAction();
        currentMenuState = s_inmainmenu; // Nach der Aktion zurück zum Hauptmenü wechseln
        activeMenuItem = mainMenuItem; // Aktuell ausgewähltes Menü-Item auf Hauptmenü-Item setzen
        getMenuEntry(&currentMenuEntry, activeMenuItem); // Aktuelle Menü-Entry-Daten in globalen Variablen aktualisieren
      }
    } else {
      // Kein Doppelklick und kein Timeout, hier regulären Klick verarbeiten
      switch (currentMenuState) {
        case s_inmainmenu:
          previousMenuState = currentMenuState; // Aktuellen Zustand als vorherigen Zustand speichern, bevor er geändert wird
          if (double_clicked) {
            gotoSubMenuIfAssigned();
          } else {
            gotoValueChange();
          }
          break;
        case s_insubmenu:
          previousMenuState = currentMenuState; // Aktuellen Zustand als vorherigen Zustand speichern, bevor er geändert wird
          if (double_clicked) {
            gotoMainMenu();
          } else {
            gotoValueChange();
          }
          break;
        case s_invaluechange:
          if (double_clicked) {
            gotoSubMenuIfAssigned();
          } else {
            gotoPreviousMenu();
          }
          break;
      }
    }
    last_click_millis = millis(); // Zeit des letzten Klicks aktualisieren, um Doppelklicks oder langes Drücken zu erkennen
  }

  if (button == 2) { // UP-Button
    if (currentMenuState == s_invaluechange) {
      if ((activeMenuItem > MAIN_MENU_END) && (subMenuProperties[mainMenuItem].itemCount > 0)) {
        if (subMenuProperties[mainMenuItem].itemIndex > 0) {
          selectSubMenu(-1, false);
       } else {
          gotoMainMenu();
        }
      }
    } else if ((currentMenuState == s_insubmenu) && (subMenuProperties[mainMenuItem].itemIndex > 0)) {
      selectSubMenu(-1, true);
    } else {
      gotoMainMenu();
    }
  }
  
  if (button == 4) { // DOWN-Button
    if (currentMenuState == s_invaluechange) {
      if ((activeMenuItem > MAIN_MENU_END) && (subMenuProperties[mainMenuItem].itemCount > 0)) {
        if (subMenuProperties[mainMenuItem].itemIndex < subMenuProperties[mainMenuItem].itemCount) {
          selectSubMenu(1, false);
        }
      }
    } else if (currentMenuState == s_insubmenu) {
      selectSubMenu(1, true);
    } else if (currentMenuState == s_inmainmenu) {
      // Wenn im Hauptmenü und DOWN gedrückt wird, wechsle zum Submenü, falls vorhanden
      gotoSubMenuIfAssigned();
    }
  }

  if (button > 0) {
    refreshMainPage(false); // Aktualisiert die Anzeige, z.B. um die neuen Farben für Hauptmenü und Submenü zu berücksichtigen
  }
}

// #############################################################################
//
//    ##     ##    ###    #### ##    ## 
//    ###   ###   ## ##    ##  ###   ## 
//    #### ####  ##   ##   ##  ####  ## 
//    ## ### ## ##     ##  ##  ## ## ## 
//    ##     ## #########  ##  ##  #### 
//    ##     ## ##     ##  ##  ##   ### 
//    ##     ## ##     ## #### ##    ## 
//
// #############################################################################

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println(F("FPGA Loader ESP32"));
  loadCredentials();
  pinMode(LED_PIN, OUTPUT);
  // Initialize LittleFS
  if(LittleFS.begin(true)) {
    Serial.println(F("Mounting LittleFS done."));
  } else {
    Serial.println(F("Error mounting LittleFS"));
  }
  showDirectory();

  #ifdef PANEL_ESP
    tft.init();
    tft.setRotation(3);  //The parameters are: 0, 1, 2, 3, representing the rotation of the screen 0°, 90°, 180°, 270°
    tft.setTextSize(1);
    tft.setFreeFont(FF22);
    encoder.setEncoderCallback(encoderCallback);
    encoder.setButtonCallback(buttonCallback);
    encoderTicker.attach_ms(2, []() { encoder.encoderISR(); }); // Ticker für Encoder-Handling, alle 2 ms
    drawBMP("/splash.bmp", 0, 0);
    delay(1500);
  #endif

  // TEST: Fill hx3EditArray with initial random values
  for (int i = 0; i < MENU_ITEMCOUNT; i++) {
    getMenuEntry(&currentMenuEntry, i); // Menü-Entry-Daten in globalen Variablen aktualisieren, um die Wertebereiche zu erhalten
    hx3EditArray[currentMenuEntry.editArrayIdx] = random(currentMenuEntry.menuValueMin, currentMenuEntry.menuValueMax); // Zufällige Werte innerhalb des gültigen Bereichs
  }
  hx3EditArray[496] = settings.wifiMode; // Aktuellen WiFi-Modus in Edit-Array laden, damit er im Menü angezeigt und bearbeitet werden kann
  
  // copy test string into hx3EditArray[192] for testing
  strcpy((char*)&hx3EditArray[192], "Irgendwas"); // String in HX3-Daten kopieren, z.B. für die Anzeige von Preset-Namen oder ähnlichem

  init_FPGAcomm();
  switch (settings.wifiMode) {
    case 1:
      wifi_connect_sta(); // Station Mode
      break;
    case 2:
      wifi_connect_ap(); // Fallback auf Access Point, wenn ungültiger Modus eingestellt ist
      break;
  }

  // spi_xc_setcmd(XCMD_BUS_TO_ESP32);
  // fpgaDate = spi_read32(F_DATEREG);
  // spi_xc_setcmd(XCMD_BUS_TO_MCU);
  // spi_xc_flush_rxbuf(); // Flush des Empfangspuffers, um alte Nachrichten zu löschen

  if (settings.wifiMode > 0) init_server();

  #ifdef PANEL_ESP
    drawMsgTimeout("Welcome to HX3", "Sound Engine", 1000, DB_INFO_OK);
    initMainMenuItems();
    displayMainPage(); // Initiales Zeichnen des Submenüs, so vorhanden
  #endif
}

void loop() {

  // uint8_t new_status = spi_xc_getstatus();
  // if ((new_status != 0) && (new_status != 0xFF)) { 
  //   // Prüfe, ob Daten vorhanden sind und ob es sich um eine echte Nachricht vom FPGA handelt
  //   // Bits 7..6 sind dann "10"
  //   spi_xc_interpret_data();
  // }

  checkSerialCommand();
  #ifdef PANEL_ESP
    encoder.checkEncoder(currentMenuEntry.menuValueMax / 16);
    encoder.checkButtonsAutorepeat(500, 100, 1500);
  #endif

  if (resetRequested) {
    delay(1000); // Kurze Verzögerung, damit die Antwort an den Browser gesendet werden kann, bevor der ESP32 neu startet
    ESP.restart();
  }
}

