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


struct {
  uint8_t SD_present;  // True wenn SD eingesetzt ist (Bootloader wartet)
  uint8_t FW_OK;    // True wenn Firmware-Vektoren stimmen
  uint8_t FPGA_OK;  // True wenn FPGA geladen und funktionsfähig ist
  uint8_t BL_procRunning; // 0 = nicht laufend, 1..xx = Bootloader-Prozess läuft
} bootloaderInfo;

// #############################################################################

// void twi_paramVal2buf(uint16_t param, uint16_t value) {
//   memset(esp32twi.txBuffer, 0, sizeof(esp32twi.txBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
//   esp32twi.txBuffer[1] = (uint8_t)(param & 0xFF); // Parameter-Nummer als Low-Byte senden
//   esp32twi.txBuffer[2] = (uint8_t)((param >> 8) & 0xFF); // Parameter-Nummer als High-Byte senden
//   esp32twi.txBuffer[3] = (uint8_t)(value & 0xFF); // Wert als Low-Byte senden
//   esp32twi.txBuffer[4] = (uint8_t)((value >> 8) & 0xFF); // Wert als High-Byte senden
//   esp32twi.txLen = 5; // Länge der zu sendenden Daten (Status + Parameter + Wert)
// }

// void twi_sendResponse(uint16_t param, uint16_t value) {
//   twi_paramVal2buf(param, value);
//   esp32twi.txStatus = TWI_BINARY_RESP;
// }

// void twi_sendCmd(uint16_t param, uint16_t value) {
//   twi_paramVal2buf(param, value);
//   esp32twi.txStatus = TWI_BINARY_CMD;
// }

// void twi_sendRequest(uint16_t param) {
//   memset(esp32twi.txBuffer, 0, sizeof(esp32twi.txBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
//   esp32twi.txStatus = TWI_BINARY_REQ;
//   esp32twi.txBuffer[1] = (uint8_t)(param & 0xFF); // Parameter-Nummer als Low-Byte senden
//   esp32twi.txBuffer[2] = (uint8_t)((param >> 8) & 0xFF); // Parameter-Nummer als High-Byte senden
//   esp32twi.txLen = 3; // Länge der zu sendenden Daten (Status + Parameter)
// }



// #############################################################################

// Frame vom Master in esp32twi.rxBuffer: Byte 0 = Kommando, Byte 1 = Anzahl der folgenden Nutzdaten-Bytes (0 = keine), danach die Nutzdaten.
// payloadLen ist die Länge laut Längenbyte (bereits auf Vollständigkeit geprüft).
void twi_interpret_rxdata() {
  const uint8_t cmd = esp32twi.rxBuffer[0];
  uint8_t payloadLen = esp32twi.rxBuffer[1];
  const uint8_t *payload = &esp32twi.rxBuffer[2]; // erstes Nutzdaten-Byte
  String message1, message2; // Zwei Anzeige-Zeilen
  // Timeout oder Zahl aus den ersten beiden Nutzdaten-Bytes zusammensetzen
  uint16_t timeout_number = (payloadLen >= 2) ? ((payload[1] << 8) | payload[0]) : 0;
  uint16_t twi_idx;
  DialogBoxType msgType = (DialogBoxType)(cmd - TWI_ESPCMD_DRAW_INFOMSG); // Nachrichtentyp aus dem Befehl ableiten

  switch (cmd) { // Je nach Kommandobyte unterschiedliche Aktionen ausführen
  case TWI_ESPCMD_DRAW_BLBOX: // Bootloader-Box rechts anzeigen
  case TWI_ESPCMD_DRAW_INFOMSG: // String-Info-Message vom I2C-Master empfangen, um eine Nachricht auf dem Display anzuzeigen
  case TWI_ESPCMD_DRAW_REQMSG:  // String-Request-Message
  case TWI_ESPCMD_DRAW_ERRMSG:  // String-Error-Message
    if (payloadLen < 2) return; // Wenn nicht mindestens der Timeout-Wert empfangen wurde, abbrechen
    // String aus den Nutzdaten extrahieren, ab Byte 2 bis 0-Terminator oder Linefeed #10, je nachdem, was zuerst kommt
    message1 = "";
    message2 = "";
    // die ersten zwei Nutzdaten-Bytes enthalten Timeout-Wert in ms
    for (twi_idx = 2; twi_idx < payloadLen; twi_idx++) {
      if (payload[twi_idx] == 0 || payload[twi_idx] == 10) { // 0-Terminator oder Linefeed als Ende der Nachricht
        break;
      }
      message1 += (char)payload[twi_idx]; // Zeichen zur ersten Zeile hinzufügen
    }
    if (twi_idx < payloadLen && payload[twi_idx] == 10) { // Wenn Linefeed gefunden wurde, könnte eine zweite Zeile folgen
      for (uint16_t j = twi_idx + 1; j < payloadLen; j++) {
        if (payload[j] == 0 || payload[j] == 10) { // 0-Terminator oder weiteres Linefeed als Ende der zweiten Nachricht
          break;
        }
        message2 += (char)payload[j]; // Zeichen zur zweiten Zeile hinzufügen
      }
    }
    if (cmd == TWI_ESPCMD_DRAW_BLBOX) {
      drawBootloaderMessagepanel(message1.c_str(), message2.c_str());
    } else {
      refreshMainPage(true);
      drawMsgTimeout(message1, message2, timeout_number, msgType); // Nachricht mit Timeout auf dem Display anzeigen
    }
    break;

  case TWI_ESPCMD_DRAW_FWRECOVER: // MessageBox anzeigen
    drawBootloaderMessagepanel("Firmware", "recover"); // Nachricht auf dem Display anzeigen
    break;
  case TWI_ESPCMD_DRAW_FPGARECOVER: // MessageBox anzeigen
    drawBootloaderMessagepanel("FPGA", "recover"); // Nachricht auf dem Display anzeigen
    break;
  case TWI_ESPCMD_DRAW_SDLOAD: // MessageBox anzeigen
    drawBootloaderMessagepanel("SD Card", "Update"); // Nachricht auf dem Display anzeigen
    break;
  case TWI_ESPCMD_DRAW_SDERR: // MessageBox anzeigen
    drawBootloaderMessagepanel("SD Card", "Error"); // Nachricht auf dem Display anzeigen
    break;
  case TWI_ESPCMD_DRAW_UNPACK:
    drawBootloaderMessagepanel("Unpacking", ""); // Nachricht auf dem Display anzeigen
    break;
  case TWI_ESPCMD_DRAW_WIFI_RCVD:
    drawBootloaderMessagepanel("WiFi", "received"); // Nachricht auf dem Display anzeigen
    break;

  case TWI_ESPCMD_DRAW_UNPACK_NUM: // Zahl in MessageBox anzeigen
    // convert timeout_number to const char * for display
    char number_str[6];
    snprintf(number_str, sizeof(number_str), "%u", timeout_number);
    drawBootloaderMessagepanel("Unpacking", number_str); // Nachricht auf dem Display anzeigen
    break;

  case TWI_ESPCMD_DRAW_DFU: // MessageBox anzeigen
    drawBootloaderMessagepanel("DFU", "active"); // Nachricht auf dem Display anzeigen
    break;
  case TWI_ESPCMD_DRAW_DFU_RCVD: // MessageBox anzeigen
    drawBootloaderMessagepanel("DFU", "received"); // Nachricht auf dem Display anzeigen
    break;
  case TWI_ESPCMD_DRAW_DONE: // MessageBox anzeigen
    drawBootloaderMessagepanel("Done.", ""); // Nachricht auf dem Display anzeigen
    break;

  case TWI_ESPCMD_BLSTART:
    enterBootloader();
    break;

  case TWI_ESPCMD_BLERR_FPGA:
    drawMsg("FPGA", "corrupted!", DB_ERROR);
    break;
  case TWI_ESPCMD_BLERR_FW:
    drawMsg("Firmware", "corrupted!", DB_ERROR);
    break;
  case TWI_ESPCMD_BLEXIT: // Message abschalten, Main Window anzeigen
    bootloaderBusy = false; // Bedienungs-Flag zurücksetzen, da der Bootloader nicht mehr aktiv ist
    exitBootloader();
    break;

  case TWI_ESPCMD_RESET: // Wenn erstes Byte 0x5F ist, fordere einen Neustart des ESP32 an
    bootloaderActive = false;
    bootloaderBusy = false; // Flag zurücksetzen, da der Bootloader nicht mehr aktiv ist
    requestSema.reset = true; // Flag setzen, damit der Neustart im Hauptloop durchgeführt wird
    break;

  case TWI_ESPCMD_BOARDINFO: 
    if (payloadLen < sizeof(boardInfo)) { // Board-Info unvollständig
      DPRINTLNF("TWI: Board Info incomplete");
      return;
    }
    DPRINTLNF("TWI: Board Info rcvd");
    memcpy(&boardInfo, payload, sizeof(boardInfo)); // Board-Info aus den Nutzdaten extrahieren
    break;

  case TWI_ESPCMD_SETBUSY:
    bootloaderBusy = true;
    break;
  case TWI_ESPCMD_SETIDLE:
    bootloaderBusy = false;
    break;

  case TWI_ESPCMD_RELOAD:
    requestSema.reloadOrgan = true; // Flag setzen, um die Organ-Daten neu zu laden
    break;

  case TWI_ESPCMD_SETSTATUS: 
    if (payloadLen < 4) return; // Wenn nicht genügend Daten empfangen wurden, abbrechen
    bootloaderInfo.SD_present = payload[0];
    bootloaderInfo.FW_OK = payload[1];
    bootloaderInfo.FPGA_OK = payload[2];
    bootloaderInfo.BL_procRunning = payload[3];
    DPRINTF("TWI: Status, SD_present: ");
    DPRINT(String(bootloaderInfo.SD_present));
    DPRINTF(", FW_OK: ");
    DPRINT(String(bootloaderInfo.FW_OK));
    DPRINTF(", FPGA_OK: ");
    DPRINT(String(bootloaderInfo.FPGA_OK));
    DPRINTF(", BL_procRunning: ");
    DPRINTLN(String(bootloaderInfo.BL_procRunning));
    break;

  case TWI_ESPCMD_GETSTATUS: 
    // wurde schon in twi_onReceive() behandelt, daher hier nichts tun
    break;
  default:
    DPRINTF("TWI: Unknown cmd 0x");
    DPRINTLN(String(cmd, HEX));
    break;
  }
}

// #############################################################################

// Callback vom TWI-Slave nach einem Lesezugriff des Masters: Frame für ein Lesen ohne vorheriges Kommando bereitstellen
// void twi_onRequest() {
//   twi_fillStatusFrame();
//   Wire.write(esp32twi.txBuffer, 1);
// }

// Callback vom TWI-Slave, wenn Daten vom Master geschrieben wurden, z.B. um eine Nachricht auf dem Display anzuzeigen oder einen Neustart anzufordern
// Der ESP32-Slave ruft den Callback erst am Ende der Master-Schreibübertragung (STOP) auf, daher sind alle Bytes des Frames bereits da.
// Frame: [Kommando][Länge N][N Nutzdaten-Bytes]; interpretiert wird nur, wenn der Frame vollständig ist.
void twi_onReceive(int len) {
  uint16_t twi_idx = 0;
  memset(esp32twi.rxBuffer, 0, sizeof(esp32twi.rxBuffer)); // Empfangspuffer vor der Verwendung löschen, um alte Daten zu entfernen
  if (len > 0) {
    while (Wire.available()) {
      esp32twi.rxBuffer[twi_idx++] = Wire.read(); // Empfangene Daten im Buffer speichern, um sie später zu verarbeiten
      if (twi_idx >= sizeof(esp32twi.rxBuffer)) {
        DPRINTLNF("TWI: Rx Buffer overflow");
        while (Wire.available()) Wire.read(); // Rest verwerfen
        return; // Frame unvollständig, nicht interpretieren
      }
    }
    if (esp32twi.rxBuffer[0] == TWI_ESPCMD_GETSTATUS) {
      // Der ESP32-I2C-Slave kann nicht "on the fly" antworten: Der TX-FIFO muss VOR dem Lesen durch den Master gefüllt sein.
      // Der Request-Callback wird erst nach Ende des Master-Lesezugriffs (STOP) aufgerufen. Daten, die dort geschrieben werden,
      // kommen erst beim NÄCHSTEN Lesen beim Master an. Daher wird die Antwort auf GETSTATUS schon in twi_onReceive() geladen.
      // Master-Ablauf: Kommando schreiben, STOP, 1..2 ms warten, dann 1 Byte lesen (kein Repeated Start).
      Wire.slaveWrite(&BtnMenuStatus, 1); // nur EIN Byte, mehrere funktioniert nicht mit STM32 als Master
    } else {
      if (twi_idx < 2 || twi_idx < 2u + esp32twi.rxBuffer[1]) {
          DPRINTF("TWI: Incomplete frame, Cmd 0x");
          DPRINTLN(String(esp32twi.rxBuffer[0], HEX));
        } else {
          twi_interpret_rxdata();
        }
      }
   }
}

#endif