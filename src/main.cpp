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
  if(editingOn) {
    changeValue(&currentMenuEntry, delta); // Wertänderung im aktuellen Menüpunkt vornehmen
  } else {
    switch (currentMenuState) {
      case s_inmainmenu:
        mainMenuItem += delta;
        if (mainMenuItem < 0) mainMenuItem = 0;
        if (mainMenuItem > MAIN_MENU_END) mainMenuItem = MAIN_MENU_END;
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
  static uint32_t last_click_millis = 0; // Zeit des letzten Button-Klicks, um Doppelklicks oder langes Drücken zu erkennen
  bool timeout_occurred = (button & 0x80) != 0; // Flag, um zu verfolgen, ob der Autorepeat-Timeout erreicht wurde
  button = button & 0x07; // Nur die unteren 3 Bits verwenden, um die Button-Nummer zu bestimmen (1, 2 oder 4)

  if (button == 1) { // ENTER-Button
    uint32_t now = millis();
    bool double_clicked = (now - last_click_millis) < 500; // Flag, um zu verfolgen, ob ein Doppelklick erkannt wurde
    if (timeout_occurred) {
      // Enter-Timeout erreicht, enterAction() ausführen
      if (currentMenuEntry.enterAction != nullptr) {
        editingOn = false;
        refreshMainPage(true);
        currentMenuEntry.enterAction(); // Funktion aus Tabelle ausführen
        getMainMenuItem();
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
          }
          break;
        case s_insubmenu:
          if (double_clicked) {
            getMainMenuItem();
          } else {
            getActiveItem();
          }
          break;
      }
    }
    last_click_millis = millis(); // Zeit des letzten Klicks aktualisieren, um Doppelklicks oder langes Drücken zu erkennen
  }

  if (button == 2) { // UP-Button
    if ((currentMenuState == s_insubmenu) && (subMenuProperties[mainMenuItem].itemIndex > 0)) {
      selectSubMenu(-1);
    } else {
      getMainMenuItem();
    }
  }
  
  if (button == 4) { // DOWN-Button
    if (currentMenuState == s_insubmenu) {
      selectSubMenu(1);
    } else if (currentMenuState == s_inmainmenu) {
      // Wenn im Hauptmenü und DOWN gedrückt wird, wechsle zum Submenü, falls vorhanden
      getSubMenuItemIfAssigned();
    }
  }

  if (button > 0) {
    if (currentMenuEntry.displayType == tm_none) {
      editingOn = false; // Wenn kein Wert zugeordnet ist, Editiermodus verlassen
    }
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

uint32_t i = 0;

void I2ConRequest() {
  Wire.write(0x55);
  Serial.println("onRequest");
  Serial.println();
}

void I2ConReceive(int len) {
  uint16_t twi_idx = 0;
  Serial.printf("onReceive[%d]: ", len);
  Serial.println();
  memset(TWIrxBuffer, 0, sizeof(TWIrxBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
  while (Wire.available()) {
    TWIrxBuffer[twi_idx++] = Wire.read(); // Empfangene Daten im Buffer speichern, um sie später zu verarbeiten
    if (twi_idx >= sizeof(TWIrxBuffer)) {
      DPRINTF("TWI Buffer overflow");
      break;
    }
  }
  String message1, message2; // Zwei Anzeige-Zeilen
  // Timeout oder Zahl aus den ersten beiden Bytes nach dem Befehl zusammensetzen
  uint16_t timeout_number = (TWIrxBuffer[2] << 8) | TWIrxBuffer[1]; 

  switch (TWIrxBuffer[0]) { // Beispiel: Je nach erstem Byte der empfangenen Daten unterschiedliche Aktionen ausführen
  case 0x50: // String-Info-Message vom I2C-Master empfangen, z.B. um eine Nachricht auf dem Display anzuzeigen
  case 0x51: // String-Request-Message
  case 0x52: // String-Error-Message
    // String aus dem Buffer extrahieren, ab Byte 1 bis 0-Terminator oder Linefeed #10, je nachdem, was zuerst kommt
    message1 = "";
    message2 = "";
    // die ersten zwei Bytes nach dem Befehl enthalten Timeout-Wert in ms
    DPRINTF("Message received: Timeout=");
    DPRINTLN(timeout_number);

    for (twi_idx = 3; twi_idx < sizeof(TWIrxBuffer); twi_idx++) {
      if (TWIrxBuffer[twi_idx] == 0 || TWIrxBuffer[twi_idx] == 10) { // 0-Terminator oder Linefeed als Ende der Nachricht
        break;
      }
      message1 += (char)TWIrxBuffer[twi_idx]; // Zeichen zur ersten Zeile hinzufügen
    }
    if (TWIrxBuffer[twi_idx] == 10) { // Wenn Linefeed gefunden wurde, könnte eine zweite Zeile folgen
      for (uint16_t j = twi_idx + 1; j < sizeof(TWIrxBuffer); j++) {
        if (TWIrxBuffer[j] == 0 || TWIrxBuffer[j] == 10) { // 0-Terminator oder weiteres Linefeed als Ende der zweiten Nachricht
          break;
        }
        message2 += (char)TWIrxBuffer[j]; // Zeichen zur zweiten Zeile hinzufügen
      }
    }
    if (timeout_number > 0) {
      refreshMainPage(true);
      drawMsgTimeout(message1, message2, timeout_number, (DialogBoxType)(TWIrxBuffer[0] - 0x50)); // Nachricht mit Timeout auf dem Display anzeigen
    } else {
      drawMsg(message1, message2, (DialogBoxType)(TWIrxBuffer[0] - 0x50)); // Nachricht auf dem Display anzeigen
    }
    break;
  case 0x56: // Wenn erstes Byte 0x53 ist, Zahl in MessageBox anzeigen
    drawMsgNumber(timeout_number);
    break;
  case 0x59: // Message abschalten
    displayMainPage();   // Seite wieder normal anzeigen, z.B. um eine zuvor angezeigte Meldung zu entfernen
    break;
  case 0xF5: // Wenn erstes Byte 0xF5 ist, fordere einen Neustart des ESP32 an
    resetRequested = true; // Flag setzen, damit der Neustart im Hauptloop durchgeführt wird
    break;
  default:
    DPRINTF("Unknown TWI command: ");
    DPRINTLNF(TWIrxBuffer[0]);
    break;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("FPGA Loader ESP32"));
  loadCredentials();
  pinMode(ESP_595_STROBE, OUTPUT);
  pinMode(ESP_ROTRY_LED, OUTPUT);
  analogWriteResolution(8); // 8-Bit PWM-Auflösung für die LED einstellen
  analogWriteFrequency(500); // PWM-Frequenz für die LED einstellen (z.B. 500 Hz)
  analogWrite(ESP_ROTRY_LED, 128); // Set initial duty cycle to 50%
  delay(100);
  Wire.onRequest(I2ConRequest);
  Wire.begin((uint8_t)I2C_DEV_ADDR, 21, 22, 400000); // I2C mit Adresse 0x21 und Pins 21 (SDA) und 22 (SCL) initialisieren, 400kHz
  // Initialize LittleFS
  if(LittleFS.begin(true)) {
    Serial.println(F("Mounting LittleFS done."));
  } else {
    Serial.println(F("Error mounting LittleFS"));
  }
  showDirectory();

  #ifdef PANEL_ESP
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
  #endif

  
  init_FPGAcomm();
  spi_xc_setcmd(XCMD_BUS_TO_ESP32);
  fpgaDate = spi_read32(F_DATEREG);
  spi_xc_setcmd(XCMD_BUS_TO_MCU);
  spi_xc_flush_rxbuf(); // Flush des Empfangspuffers, um alte Nachrichten zu löschen
  
  spi_xc_request_editArray(250); // Aktualisiert den Wert im Edit-Array, damit die Anzeige den neuen Wert zeigt
  hx3EditArray[496] = settings.wifiMode; // Aktuellen WiFi-Modus in Edit-Array laden, damit er im Menü angezeigt und bearbeitet werden kann
  hx3EditArray[499] = settings.screenSaver; // Aktuellen Bildschirmschoner-Modus in Edit-Array laden, damit er im Menü angezeigt und bearbeitet werden kann

  #ifdef USE_WIFI
    switch (settings.wifiMode) {
      case 1:
        wifi_connect_sta(); // Station Mode
        break;
      case 2:
        wifi_connect_ap(); // Fallback auf Access Point, wenn ungültiger Modus eingestellt ist
        break;
    }
     #ifdef ENABLE_SCREENSAVER
    configTzTime(MY_TIMEZONE, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!
    // Erste Anzeige initialisieren
    time(&now);
    timeinfo = localtime(&now);
    #endif
    if (settings.wifiMode > 0) init_server();
  #endif


  #ifdef PANEL_ESP
    Wire.onReceive(I2ConReceive);
    drawMsgWaitDuration("Welcome to HX3", "Sound Engine", 1000, DB_INFO_OK);
    initMainMenuItems();
    displayMainPage(); // Initiales Zeichnen des Submenüs, so vorhanden
  #endif
}

void loop() {

  uint8_t new_status = spi_xc_getstatus();
  if ((new_status != 0) && (new_status != 0xFF)) { 
    // Prüfe, ob Daten vorhanden sind und ob es sich um eine echte Nachricht vom FPGA handelt
    // Bits 7..6 sind dann "10"
    spi_xc_interpret_data();
    if (new_status & XSTA_FLASH_ON_MASK) {
      // Meldung anzeigen, wenn Update läuft, z.B. nach einem Firmware-Update über die Weboberfläche
      refreshMainPage(true); // Seite dimmen
      drawMsgTimeout("Update running", "Please wait...", 1000, DB_INFO);
      do {
        delay(100);
        new_status = spi_xc_getstatus();
      } while (new_status & XSTA_FLASH_ON_MASK);
      refreshMainPage(false); // Seite wieder normal anzeigen
    }
  }

  checkSerialCommand();
  #ifdef PANEL_ESP
    encoder.checkEncoder(currentMenuEntry.menuValueMax / 16);
    encoder.checkButtonsAutorepeat(500, 100, SAVE_TIMEOUT);
    // Vom Webserver angeforderte Aktionen ausführen, z.B. Neuzeichnen der Orgelgrafik oder Neustart des ESP32
    if (redrawOrganRequest) {
      redrawOrgan();
      redrawOrganRequest = false; // Flag zurücksetzen, damit die Orgel nicht in jeder Schleife neu gezeichnet wird
    }
  #endif

  if (msgTimeoutActive && (millis() > msgTimeoutEndTime)) {
    refreshMainPage(false); // Seite dimmen, um anzuzeigen, dass die Meldung nicht mehr aktiv ist
    msgTimeoutActive = false; // Flag zurücksetzen, damit die Meldung nicht in jeder Schleife neu gezeichnet wird
  }

  if (resetRequested) {
    delay(1000); // Kurze Verzögerung, damit die Antwort an den Browser gesendet werden kann, bevor der ESP32 neu startet
    ESP.restart();
  }

  static uint32_t lastUpdateTime = 0;
  uint32_t currentTime = millis();
  if (currentTime - lastUpdateTime > 30) {
    lastUpdateTime = currentTime;
    sendPanelLEDs();
  }
}

