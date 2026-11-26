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
#include "twi_msg.h"
#include "drawing.h"

void getActiveItem() {
  if (currentMenuState == s_inmainmenu) {
    activeMenuItem = mainMenuItem; // Aktuell ausgewähltes Menü-Item speichern, bevor es geändert wird
  } else if (currentMenuState == s_insubmenu) {
    activeMenuItem = mainMenuItems[mainMenuItem].submenuStart + subMenuProperties[mainMenuItem].itemIndex; // Aktuell ausgewähltes Menü-Item speichern, bevor es geändert wird
  }
  getMenuEntry(&currentMenuEntry, activeMenuItem); // Aktuelle Menü-Entry-Daten in globalen Variablen aktualisieren
}

void getMainMenuItem() {
  currentMenuState = s_inmainmenu;
  getActiveItem(); // Aktuelle Menü-Entry-Daten in globalen Variablen aktualisieren
}

void getSubMenuItemIfAssigned() {
  // Hilfsfunktion, um bei einem Menüpunkt mit zugeordnetem Untermenü direkt ins Untermenü zu wechseln, z.B. nach einem Button-Click
  if (currentMenuEntry.submenuStart >= 0) {
    currentMenuState = s_insubmenu; // Ins Untermenü wechseln, wenn im Hauptmenü
  } else {
    currentMenuState = s_inmainmenu;
  }
  getActiveItem(); // Aktuelle Menü-Entry-Daten in globalen Variablen aktualisieren
}


// Callback vom Encoder- und Button-Handler, um die Menü-Navigation und Werteänderungen zu steuern
void encoderCallback(int16_t delta) {
  if (bootloaderBusy) return;
  if(editingOn) {
    changeValue(&currentMenuEntry, delta); // Wertänderung im aktuellen Menüpunkt vornehmen
  } else {
    switch (currentMenuState) {
      case s_inmainmenu:
        mainMenuItem += delta;
        if (bootloaderActive) {
          mainMenuItem = BOOTLOAD_MENU; // keep within bootload menu range when bootload menu is active
        } else {
          if (mainMenuItem < 0) mainMenuItem = 0;
          if (mainMenuItem >= MAIN_MENU_END) mainMenuItem = MAIN_MENU_END - 1; // except Bootloader menu
        }
        getSubMenuItems(mainMenuItem); // Submenü-Strings basierend auf der Auswahl im Hauptmenü aktualisieren
        break;
      case s_insubmenu:
        selectSubMenu(delta);// setzt activeMenuItem und currentMenuEntry
        break;
    }
  }
  getActiveItem();
  refreshMainPage(false);
}

// -----------------------------------------------------------------------------


// Up/Down-Buttons wechseln Menüeintrag, auch wenn im ValueChange-Mode
// Enter-Button Wechselt zwischen Menüeintrag und Value-Edit
// Doppelklick auf Enter-Button wechselt direkt zum Hauptmenü oder zum Untermenü, je nachdem
// Langer Druck auf Enter-Button löste die enterAction für diesen Menüpunkt aus
void buttonCallback(uint8_t button) {
  if (bootloaderBusy) return;
  static uint32_t last_click_millis = 0; // Zeit des letzten Button-Klicks, um Doppelklicks oder langes Drücken zu erkennen
  bool timeout_occurred = (button & 0x80) != 0; // Flag, um zu verfolgen, ob der Autorepeat-Timeout erreicht wurde
  button = button & 0x07; // Nur die unteren 3 Bits verwenden, um die Button-Nummer zu bestimmen (1, 2 oder 4)
  if (button == 1) { // ENTER-Button
    if (bootloaderActive) return; 
    uint32_t now = millis();
    bool double_clicked = ((now - last_click_millis) < 500); // Flag, um zu verfolgen, ob ein Doppelklick erkannt wurde
    if (bootloaderActive)  {
      editingOn = false;
      getActiveItem();
      if (currentMenuEntry.enterAction != nullptr) {
        editingOn = false;
        refreshMainPage(true);
        currentMenuEntry.enterAction(); // Funktion aus Tabelle ausführen
        button = 0;
      }
    } else if (timeout_occurred) {
      // Enter-Timeout erreicht, timeoutAction() ausführen
      if (currentMenuEntry.timeoutAction != nullptr) {
        editingOn = false;
        refreshMainPage(true);
        currentMenuEntry.timeoutAction(); // Funktion aus Tabelle ausführen
        button = 0;
      }
    } else {
      // Kein Doppelklick und kein Timeout, hier regulären Klick verarbeiten
      editingOn = !editingOn; // Toggle des Editiermodus bei einfachem Klick
      switch (currentMenuState) {
        case s_inmainmenu:
          if (double_clicked) {
            getSubMenuItemIfAssigned();
          } else {
            getActiveItem();
            if (currentMenuEntry.enterAction != nullptr) {
              editingOn = false;
              refreshMainPage(true);
              currentMenuEntry.enterAction(); // Funktion aus Tabelle ausführen
              button = 0;
            }
          }
          break;
        case s_insubmenu:
          if (double_clicked) {
            getMainMenuItem();
          } else {
            getActiveItem();
            if (currentMenuEntry.enterAction != nullptr) {
              editingOn = false;
              refreshMainPage(true);
              currentMenuEntry.enterAction(); // Funktion aus Tabelle ausführen
              button = 0;
            }
          }
          break;
      }
    }
    last_click_millis = millis(); // Zeit des letzten Klicks aktualisieren, um Doppelklicks oder langes Drücken zu erkennen
  }

  if (button == 2) { // UP-Button
    if (bootloaderActive) currentMenuState = s_insubmenu; 
    if ((currentMenuState == s_insubmenu) && (subMenuProperties[mainMenuItem].itemIndex > 0)) {
      selectSubMenu(-1);
    } else {
      if (!bootloaderActive) getMainMenuItem();
    }
  }
  
  if (button == 4) { // DOWN-Button
    if (bootloaderActive) currentMenuState = s_insubmenu; 
    if (currentMenuState == s_insubmenu) {
      selectSubMenu(1);
    } else if (currentMenuState == s_inmainmenu) {
      // Wenn im Hauptmenü und DOWN gedrückt wird, wechsle zum Submenü, falls vorhanden
      if (!bootloaderActive) getSubMenuItemIfAssigned();
    }
  }

  if (button > 0) {
    if (currentMenuEntry.displayType == tm_none) {
      editingOn = false; // Wenn kein Wert zugeordnet ist, Editiermodus verlassen
    }
    if (bootloaderActive) editingOn = false;
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
  Serial.println(F("FPGA Loader ESP32"));
  // WICHTIG:
  // in esp32-hal-i2c-slave.c die Funktion i2c_slave_check_line_state ändern, damit Slave funktioniert
  // Zeilen 539 bis 587 auskommentieren
  activeMenuItem= 0;
  BtnMenuStatus = TWI_BLMENU_BOOTING; // Flag für MCU-Bootloader setzen
  Wire.begin((uint8_t)I2C_DEV_ADDR, 21, 22, 400000); // I2C mit Adresse 0x21 und Pins 21 (SDA) und 22 (SCL) initialisieren, 400kHz
  Wire.onReceive(twi_onReceive);
  // Wire.onRequest(twi_onRequest); // erledigt jetzt TWI_ESPCMD_GETSTATUS

  loadCredentials();
  pinMode(ESP_595_STROBE, OUTPUT);
  // pinMode(ESP_ROTRY_LED, OUTPUT);
  analogWriteResolution(8); // 8-Bit PWM-Auflösung für die LED einstellen
  analogWriteFrequency(500); // PWM-Frequenz für die LED einstellen (z.B. 500 Hz)
  // analogWrite(ESP_ROTRY_LED, 128); // Set initial duty cycle to 50%
  // Initialize LittleFS
  if(LittleFS.begin(true)) {
    Serial.println(F("Mounting LittleFS done."));
  } else {
    Serial.println(F("Error mounting LittleFS"));
  }
  showDirectory();

  tft.init();
  tft.setRotation(TFT_ROTATION);  //The parameters are: 0, 1, 2, 3, representing the rotation of the screen 0°, 90°, 180°, 270°
  tft.setTextSize(1);
  #ifdef ENABLE_SCREENSAVER
    screenSaverInit();
    for (int i = 0; i < 50; ++i) {
      starFieldUpdate(false);
      delay(20);
    }
  #endif
  // tft.fillScreen(TFT_BLACK);
  encoder.setEncoderCallback(encoderCallback);
  encoder.setButtonCallback(buttonCallback);
  encoderTicker.attach_ms(2, []() { encoder.encoderISR(); }); // Ticker für Encoder-Handling, alle 2 ms
  if (DISPLAY_H == 142) {
    drawBMP("/splash142.bmp", 0, 0);
  } else if (DISPLAY_H == 170) {
    drawBMP("/splash170.bmp", 0, 0);
  } else {  
    drawBMP("/splash240.bmp", 0, 0);
  }

  init_FPGAcomm();
  spi_xc_setcmd(XCMD_BUS_TO_ESP32);
  fpgaDate = spi_read32(F_DATEREG);
  spi_xc_setcmd(XCMD_BUS_TO_MCU);
  while (Wire.available()) Wire.read(); // Empfangene Daten verwerfen
  spi_xc_flush_rxbuf();

  #ifdef USE_WIFI
    switch (settings.wifiMode) {
      case 1:
        wifi_connect_sta(); // Station Mode
        break;
      case 2:
        wifi_connect_ap(); // Fallback auf Access Point, wenn ungültiger Modus eingestellt ist
        break;
    }
    #ifdef ENABLE_TIME
      if ((WiFi.status() == WL_CONNECTED)) {
        configTzTime(MY_TIMEZONE, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!
        // Erste Anzeige initialisieren
        time(&now);
        timeinfo = localtime(&now);
     }
    #endif
    if (settings.wifiMode > 0) init_server();
  #endif

  initMainMenuItems();
  drawMsg("Welcome to HX3", "Sound Engine", DB_INFO);
  delay(MSG_DISPLAY_TIME);

  displayMainPage(); // Initiales Zeichnen des Submenüs, so vorhanden
}

void loop() {

  if (bootloaderActive) {
    requestSema.reloadOrgan = false; // Flag zurücksetzen, damit die Orgel nicht in jeder Schleife neu gezeichnet wird
    requestSema.reloadExtended = false; // Flag zurücksetzen, damit die erweiterte Anzeige nicht in jeder Schleife neu gezeichnet wird
    requestSema.reconnect = false; // Flag zurücksetzen, damit die Verbindung nicht in jeder Schleife neu aufgebaut wird
  } else do {
    uint8_t new_status = spi_xc_getstatus();
    if ((new_status != 0) && (new_status != 0xFF)) { 
      // Prüfe, ob Daten vorhanden sind und ob es sich um eine echte Nachricht vom FPGA handelt
      // Bits 7..6 sind dann "10"
      spi_xc_interpret_data();
    } else {
      break;
    }
  } while(0);
 
  checkSerialCommand();
  encoder.checkEncoder(currentMenuEntry.menuValueMax / 16);

  // Set response status for TWI request
  if (activeMenuItem < BOOTLOAD_MENU_START) {
    BtnMenuStatus = TWI_BLMENU_NONE; 
    encoder.checkButtonsAutorepeat(500, 100, SAVE_TIMEOUT);
  } else {
    BtnMenuStatus = (activeMenuItem - BOOTLOAD_MENU_START + TWI_BLMENU_STARTFDU) | (encoder.checkButtons() << 4);
  }

  // Vom Webserver angeforderte Aktionen ausführen, z.B. Neuzeichnen der Orgelgrafik oder Neustart des ESP32

  if (requestSema.redrawOrgan) {
    redrawOrgan();
    requestSema.redrawOrgan = false; // Flag zurücksetzen, damit die Orgel nicht in jeder Schleife neu gezeichnet wird
  }

  if (requestSema.reloadOrgan) {
    spi_xc_requestEditArray(250);
    redrawOrgan();
    requestSema.reloadOrgan = false; // Flag zurücksetzen, damit die Orgel nicht in jeder Schleife neu gezeichnet wird
  }

  if (requestSema.reloadExtended) {
    spi_xc_getExtendedArray(250);
    redrawOrgan();
    requestSema.reloadExtended = false; // Flag zurücksetzen, damit die erweiterte Anzeige nicht in jeder Schleife neu gezeichnet wird
  }

  if (requestSema.reset) {
    spi_xc_binarycmd(9999, 1); // Reset-Befehl
    delay(1000); // Kurze Verzögerung, damit die Antwort an den Browser gesendet werden kann, bevor der ESP32 neu startet
    ESP.restart();
  }

  if (requestSema.reconnect) {
    refreshMainPage(true); // dimmed
    WiFi.disconnect();
    switch (settings.wifiMode) {
    case 1:
      wifi_connect_sta(); // Station Mode
      break;
    case 2:
      wifi_connect_ap(); // Fallback auf Access Point, wenn ungültiger Modus eingestellt ist
      break;
    } 
    if (settings.wifiMode > 0) init_server();
    displayMainPage();
    requestSema.reconnect = false; // Flag zurücksetzen, damit die Orgel nicht in jeder Schleife neu gezeichnet wird
  }

  if (msgTimeoutActive && (millis() > msgTimeoutEndTime)) {
    refreshMainPage(false); // not dimmed 
    msgTimeoutActive = false; // Flag zurücksetzen, damit die Meldung nicht in jeder Schleife neu gezeichnet wird
  }

  static uint32_t lastUpdateTime = 0;
  uint32_t currentTime = millis();
  if (currentTime - lastUpdateTime > 30) {
    lastUpdateTime = currentTime;
    sendPanelLEDs();
  }
}

