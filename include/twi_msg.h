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
  memset(esp32twi.txBuffer, 0, sizeof(esp32twi.txBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
  esp32twi.txBuffer[1] = (uint8_t)(param & 0xFF); // Parameter-Nummer als Low-Byte senden
  esp32twi.txBuffer[2] = (uint8_t)((param >> 8) & 0xFF); // Parameter-Nummer als High-Byte senden
  esp32twi.txBuffer[3] = (uint8_t)(value & 0xFF); // Wert als Low-Byte senden
  esp32twi.txBuffer[4] = (uint8_t)((value >> 8) & 0xFF); // Wert als High-Byte senden
  esp32twi.txLen = 5; // Länge der zu sendenden Daten (Status + Parameter + Wert)
}

void twi_sendResponse(uint16_t param, uint16_t value) {
  twi_paramVal2buf(param, value);
  esp32twi.txStatus = TWI_BINARY_RESP;
}

void twi_sendCmd(uint16_t param, uint16_t value) {
  twi_paramVal2buf(param, value);
  esp32twi.txStatus = TWI_BINARY_CMD;
}

void twi_sendRequest(uint16_t param) {
  memset(esp32twi.txBuffer, 0, sizeof(esp32twi.txBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
  esp32twi.txStatus = TWI_BINARY_REQ;
  esp32twi.txBuffer[1] = (uint8_t)(param & 0xFF); // Parameter-Nummer als Low-Byte senden
  esp32twi.txBuffer[2] = (uint8_t)((param >> 8) & 0xFF); // Parameter-Nummer als High-Byte senden
  esp32twi.txLen = 3; // Länge der zu sendenden Daten (Status + Parameter)
}

// Callback vom TWI-Slave, wenn der Master Daten anfordert, z.B. um den Status oder Werte von Parametern abzufragen
void twi_onRequest() {
  esp32twi.txBuffer[0] = esp32twi.txStatus; // Statusmeldung als erstes Byte senden, damit der Master weiß, ob der ESP32 bereit ist oder nicht
  Wire.write(&esp32twi.txBuffer[esp32twi.txSendIdx++], 1); // Befehl und Parameter senden, Byte für Byte
  if (esp32twi.txSendIdx >= esp32twi.txLen) { // Wenn alle zu sendenden Daten übertragen wurden, Status zurücksetzen
    esp32twi.txSendIdx = 0; // Zeiger auf erstes Byte der zu sendenden Daten zurücksetzen
  }
}

// Callback vom TWI-Slave, wenn Daten vom Master geschrieben wurden, z.B. um eine Nachricht auf dem Display anzuzeigen oder einen Neustart anzufordern
void twi_onReceive(int len) {
  uint16_t twi_idx = 0;
  memset(esp32twi.rxBuffer, 0, sizeof(esp32twi.rxBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
  while (Wire.available()) {
    esp32twi.rxBuffer[twi_idx++] = Wire.read(); // Empfangene Daten im Buffer speichern, um sie später zu verarbeiten
    if (twi_idx >= sizeof(esp32twi.rxBuffer)) {
      DPRINTF("TWI rX Buffer overflow");
      break;
    }
  }
  esp32twi.rxLen = twi_idx; // Länge der empfangenen Daten speichern
  esp32twi.rxSema = true; // Semaphore setzen, damit die empfangenen Daten im Hauptloop verarbeitet werden können
  esp32twi.txStatus = TWI_ESPSTATUS_BUSY; // Flag für MCU setzen, bis behandelt
  esp32twi.txSendIdx = 0; // Zeiger auf erstes Byte der zu sendenden Daten zurücksetzen
}

// #############################################################################

void twi_interpret_data() {
  String message1, message2; // Zwei Anzeige-Zeilen
  // Timeout oder Zahl aus den ersten beiden Bytes nach dem Befehl zusammensetzen
  uint16_t timeout_number = (esp32twi.rxBuffer[2] << 8) | esp32twi.rxBuffer[1]; 
  uint16_t twi_idx;
  DialogBoxType msgType = (DialogBoxType)(esp32twi.rxBuffer[0] - TWI_ESPCMD_DRAWINFOMSG); // Nachrichtentyp aus dem Befehl ableiten
  int param; // Parameter-Nummer aus den empfangenen Daten zusammensetzen
  int value; // Wert aus den empfangenen Daten zusammensetzen

  switch (esp32twi.rxBuffer[0]) { // Je nach erstem Byte der empfangenen Daten unterschiedliche Aktionen ausführen
  case TWI_ESPCMD_DRAWINFOMSG: // String-Info-Message vom I2C-Master empfangen, um eine Nachricht auf dem Display anzuzeigen
  case TWI_ESPCMD_DRAWREQMSG:  // String-Request-Message
  case TWI_ESPCMD_DRAWERRMSG:  // String-Error-Message
    // String aus dem Buffer extrahieren, ab Byte 1 bis 0-Terminator oder Linefeed #10, je nachdem, was zuerst kommt
    message1 = "";
    message2 = "";
    // die ersten zwei Bytes nach dem Befehl enthalten Timeout-Wert in ms
    DPRINTF("Message received: Number/Timeout=");
    DPRINTLN(timeout_number);

    for (twi_idx = 3; twi_idx < sizeof(esp32twi.rxBuffer); twi_idx++) {
      if (esp32twi.rxBuffer[twi_idx] == 0 || esp32twi.rxBuffer[twi_idx] == 10) { // 0-Terminator oder Linefeed als Ende der Nachricht
        break;
      }
      message1 += (char)esp32twi.rxBuffer[twi_idx]; // Zeichen zur ersten Zeile hinzufügen
    }
    if (esp32twi.rxBuffer[twi_idx] == 10) { // Wenn Linefeed gefunden wurde, könnte eine zweite Zeile folgen
      for (uint16_t j = twi_idx + 1; j < sizeof(esp32twi.rxBuffer); j++) {
        if (esp32twi.rxBuffer[j] == 0 || esp32twi.rxBuffer[j] == 10) { // 0-Terminator oder weiteres Linefeed als Ende der zweiten Nachricht
          break;
        }
        message2 += (char)esp32twi.rxBuffer[j]; // Zeichen zur zweiten Zeile hinzufügen
      }
    }
    refreshMainPage(true);
    drawMsgTimeout(message1, message2, timeout_number, msgType); // Nachricht mit Timeout auf dem Display anzeigen
    break;
  case TWI_ESPCMD_DRAWNRMSG: // Zahl in MessageBox anzeigen
    msgTimeoutEndTime = millis() + 500; // Endzeitpunkt für die Anzeige der Zahl setzen
    drawMsgNumber(timeout_number);
    break;
  case TWI_ESPCMD_EXIT: // Message abschalten, Main Window anzeigen
    displayMainPage();
    requestSema.bootloaderActiveMsg = false;
    break;
  case TWI_ESPCMD_RELOAD: // Message abschalten, Main Window anzeigen
    spi_xc_requestEditArray(250); // Aktualisiert den Wert im Edit-Array, damit die Anzeige den neuen Wert zeigt
    refreshMainPage(false);
    requestSema.bootloaderActiveMsg = false;
    break;
  case TWI_ESPCMD_RESET: // Wenn erstes Byte 0x5F ist, fordere einen Neustart des ESP32 an
    requestSema.reset = true; // Flag setzen, damit der Neustart im Hauptloop durchgeführt wird
    break;
  case TWI_ESPCMD_BLSTART:
    refreshMainPage(true); // Seite dimmen, um anzuzeigen, dass der Bootloader aktiv ist
    drawMsg("DFU Start", "", DB_INFO); // Nachricht auf dem Display anzeigen
    break;
  case TWI_ESPCMD_BOARDINFO: 
    memcpy(&boardInfo, &esp32twi.rxBuffer[1], sizeof(boardInfo)); // Board-Info aus dem Buffer extrahieren
    break;
  case TWI_ESPCMD_BLACTIVE: 
    if (!requestSema.bootloaderActiveMsg) {
      DPRINTF("Bootloader active..");
      refreshMainPage(true); // Seite dimmen, um anzuzeigen, dass der Bootloader aktiv ist
      drawMsg("DFU active", "", DB_INFO); // Nachricht auf dem Display anzeigen
      requestSema.bootloaderActiveMsg = true; // Flag setzen, damit im Hauptloop angezeigt wird, dass der Bootloader aktiv ist
    }
    DPRINTF(".");
    if (requestSema.uploadEnded) {
      DPRINTLNF("Done, send EXIT cmd");
      esp32twi.txStatus = TWI_BLCMD_EXIT; // Bootloader beenden, falls noch aktiv
      esp32twi.txSendIdx = 0; // Zeiger auf erstes Byte der zu sendenden Daten zurücksetzen
      delay(500); // Warte, bis Bootloader das begriffen hat
      displayMainPage();
      requestSema.uploadEnded = false; // Flag zurücksetzen, damit der Befehl nicht in jeder Schleife gesendet wird
    }
    break;
  case TWI_BINARY_RESP: 
    param = (esp32twi.rxBuffer[2] << 8) | esp32twi.rxBuffer[1]; // Parameter-Nummer aus den empfangenen Daten zusammensetzen
    value = (esp32twi.rxBuffer[4] << 8) | esp32twi.rxBuffer[3]; // Wert aus den empfangenen Daten zusammensetzen
    DPRINTF("TWI response, param: ");
    DPRINT(String(param));
    DPRINTF(", value: ");
    DPRINTLN(String(value));
    break;
  case 0x06: 
    break;
  default:
    DPRINTF("Unknown TWI command, First byte: 0x");
    DPRINTLN(String(esp32twi.rxBuffer[0], HEX));
    break;
  }
  esp32twi.txStatus = TWI_ESPSTATUS_READY; // Flag für MCU-Bootloader setzen
  esp32twi.txSendIdx = 0; // Zeiger auf erstes Byte der zu sendenden Daten zurücksetzen
}

#endif