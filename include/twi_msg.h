#ifndef TWI_MSG_H
#define TWI_MSG_H

// #############################################################################
//
//     ####### #     # ###       #     #  #####   #####  
//        #    #  #  #  #        ##   ## #     # #     # 
//        #    #  #  #  #        # # # # #       #       
//        #    #  #  #  #        #  #  #  #####  #  #### 
//        #    #  #  #  #        #     #       # #     # 
//        #    #  #  #  #        #     # #     # #     # 
//        #     ## ##  ###       #     #  #####   #####                                                       
//
// #############################################################################

// TWI/I2C muss benutzt werden, wenn FPGA nicht bereit ist, z.B. im Bootloader.
// Hier als Ersatz für altes 16x2-Display

#include <Arduino.h>
#include "global_vars.h"
#include "spi_xchange.h"
#include "parser.h"
#include "menu_items.h"
#include "drawing.h"


// #############################################################################

void twi_paramVal2buf(uint16_t param, uint16_t value) {
  memset(TWItxBuffer, 0, sizeof(TWItxBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
  TWItxBuffer[1] = (uint8_t)(param & 0xFF); // Parameter-Nummer als Low-Byte senden
  TWItxBuffer[2] = (uint8_t)((param >> 8) & 0xFF); // Parameter-Nummer als High-Byte senden
  TWItxBuffer[3] = (uint8_t)(value & 0xFF); // Wert als Low-Byte senden
  TWItxBuffer[4] = (uint8_t)((value >> 8) & 0xFF); // Wert als High-Byte senden
  esp32twi.txLen = 5; // Länge der zu sendenden Daten (1 Byte für den Befehl + 2 Bytes für die Parameter-Nummer + 2 Bytes für den Wert)
  esp32twi.txSema = true; // es sind Sendedaten vorhanden
}

void twi_sendRequest(uint16_t param) {
  memset(TWItxBuffer, 0, sizeof(TWItxBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
  esp32twi.status = TWI_BINARY_REQ;
  TWItxBuffer[1] = (uint8_t)(param & 0xFF); // Parameter-Nummer als Low-Byte senden
  TWItxBuffer[2] = (uint8_t)((param >> 8) & 0xFF); // Parameter-Nummer als High-Byte senden
  esp32twi.txLen = 5; // Länge der zu sendenden Daten (1 Byte für den Befehl + 2 Bytes für die Parameter-Nummer)
  esp32twi.txSema = true; // es sind Sendedaten vorhanden
}

void twi_sendResponse(uint16_t param, uint16_t value) {
  twi_paramVal2buf(param, value);
  esp32twi.status = TWI_BINARY_RESP;
}

void twi_sendCmd(uint16_t param, uint16_t value) {
  twi_paramVal2buf(param, value);
  esp32twi.status = TWI_BINARY_CMD;
}

// Callback vom TWI-Slave, wenn der Master Daten anfordert, z.B. um den Status oder Werte von Parametern abzufragen
void twi_onRequest() {
  TWItxBuffer[0] = esp32twi.status; // Statusmeldung als erstes Byte senden, damit der Master weiß, ob der ESP32 bereit ist oder nicht
  Wire.write(TWItxBuffer, esp32twi.txLen); // Befehl und Parameter senden
  esp32twi.txSema = false;
}

// Callback vom TWI-Slave, wenn Daten vom Master geschrieben wurden, z.B. um eine Nachricht auf dem Display anzuzeigen oder einen Neustart anzufordern
void twi_onReceive(int len) {
  uint16_t twi_idx = 0;
  memset(TWIrxBuffer, 0, sizeof(TWIrxBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
  while (Wire.available()) {
    TWIrxBuffer[twi_idx++] = Wire.read(); // Empfangene Daten im Buffer speichern, um sie später zu verarbeiten
    if (twi_idx >= sizeof(TWIrxBuffer)) {
      DPRINTF("TWI Buffer overflow");
      break;
    }
  }
  esp32twi.rxLen = twi_idx; // Länge der empfangenen Daten speichern
  esp32twi.rxSema = true; // Semaphore setzen, damit die empfangenen Daten im Hauptloop verarbeitet werden können
  esp32twi.status = TWI_ESPSTATUS_BUSY; // Flag für MCU-Bootloader setzen
  esp32twi.txLen = 1; // Länge der zu sendenden Daten (1 Byte für den Befehl + 2 Bytes für die Parameter-Nummer + 2 Bytes für den Wert)
}

// #############################################################################

void twi_interpret_data() {
  String message1, message2; // Zwei Anzeige-Zeilen
  // Timeout oder Zahl aus den ersten beiden Bytes nach dem Befehl zusammensetzen
  uint16_t timeout_number = (TWIrxBuffer[2] << 8) | TWIrxBuffer[1]; 
  uint16_t twi_idx;
  DialogBoxType msgType = (DialogBoxType)(TWIrxBuffer[0] - TWI_ESPCMD_DRAWINFOMSG); // Nachrichtentyp aus dem Befehl ableiten

  switch (TWIrxBuffer[0]) { // Beispiel: Je nach erstem Byte der empfangenen Daten unterschiedliche Aktionen ausführen
  case TWI_ESPCMD_DRAWINFOMSG: // String-Info-Message vom I2C-Master empfangen, um eine Nachricht auf dem Display anzuzeigen
  case TWI_ESPCMD_DRAWREQMSG:  // String-Request-Message
  case TWI_ESPCMD_DRAWERRMSG:  // String-Error-Message
    // String aus dem Buffer extrahieren, ab Byte 1 bis 0-Terminator oder Linefeed #10, je nachdem, was zuerst kommt
    message1 = "";
    message2 = "";
    // die ersten zwei Bytes nach dem Befehl enthalten Timeout-Wert in ms
    DPRINTF("Message received: Number/Timeout=");
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
    refreshMainPage(true);
    drawMsgTimeout(message1, message2, timeout_number, msgType); // Nachricht mit Timeout auf dem Display anzeigen
    break;
  case TWI_ESPCMD_DRAWNRMSG: // Wenn erstes Byte 0x56 ist, Zahl in MessageBox anzeigen
    msgTimeoutEndTime = millis() + 500; // Endzeitpunkt für die Anzeige der Zahl setzen
    drawMsgNumber(timeout_number);
    break;
  case TWI_ESPCMD_EXIT: // Message abschalten, Main Window anzeigen
    displayMainPage();
    bootloaderActive = false;
    break;
  case TWI_ESPCMD_RELOAD: // Message abschalten, Main Window anzeigen
    spi_xc_getEditArray(250); // Aktualisiert den Wert im Edit-Array, damit die Anzeige den neuen Wert zeigt
    refreshMainPage(false);
    bootloaderActive = false;
    break;
  case TWI_ESPCMD_RESET: // Wenn erstes Byte 0x5F ist, fordere einen Neustart des ESP32 an
    resetRequested = true; // Flag setzen, damit der Neustart im Hauptloop durchgeführt wird
    break;
  case TWI_ESPCMD_BLSTART:
    refreshMainPage(true); // Seite dimmen, um anzuzeigen, dass der Bootloader aktiv ist
    drawMsg("DFU Start", "", DB_INFO); // Nachricht auf dem Display anzeigen
    break;
  case TWI_ESPCMD_BOARDINFO: 
    memcpy(&boardInfo, &TWIrxBuffer[1], sizeof(boardInfo)); // Board-Info aus dem Buffer extrahieren
    break;
  case TWI_ESPCMD_BLACTIVE: 
    DPRINTF(".");
    if (!bootloaderActive) {
      refreshMainPage(true); // Seite dimmen, um anzuzeigen, dass der Bootloader aktiv ist
      drawMsg("DFU active", "", DB_INFO); // Nachricht auf dem Display anzeigen
      bootloaderActive = true; // Flag setzen, damit im Hauptloop angezeigt wird, dass der Bootloader aktiv ist
    }
    break;
  default:
    DPRINTF("Unknown TWI command, First byte: 0x");
    DPRINTLN(String(TWIrxBuffer[0], HEX));
    break;
  }
  esp32twi.status = TWI_ESPSTATUS_READY; // Flag für MCU-Bootloader setzen
}

#endif