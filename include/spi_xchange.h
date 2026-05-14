#ifndef SPI_XCHANGE_H
#define SPI_XCHANGE_H

#include <Arduino.h>
#include "global_vars.h"
#include "fpga_config.h"

// #define DEBUG_XC

#ifdef DEBUG_XC
  #define DXCPRINT(...)    Serial.print(__VA_ARGS__)
  //OR, #define DXCPRINT(args...)    Serial.print(args)
  #define DXCPRINTLN(...)  Serial.println(__VA_ARGS__)
  #define DXCPRINTF(...)    Serial.print(F(__VA_ARGS__))
  #define DXCPRINTLNF(...) Serial.println(F(__VA_ARGS__)) //printing text using the F macro
  #define DXCDUMP(...) spi_dump_buf(spi_blockbuffer.byte, 0, __VA_ARGS__)
#else
  #define DXCPRINT(...)     //blank line
  #define DXCPRINTLN(...)   //blank line
  #define DXCPRINTF(...)    //blank line
  #define DXCPRINTLNF(...)  //blank line
  #define DXCDUMP(...)   // blank line
#endif

// Commands
#define XCMD_DONE 0xED  // Löscht NAK_ERR und ACK-Bits 0 und 1
#define XCMD_NAK_ERR 0xEE  // Löscht Status-Bit 0 ACK, setzt Bit 1
#define XCMD_ACK 0xEF  // Setzt Status-Bit 0, löscht Bit 1
#define XCMD_STATUS_READ 0xF7  // 
#define XCMD_XCHNG_READ 0xF8  // gefolgt von bis zu 2048 Null-Bytes zum Lesen per SPI
#define XCMD_XCHNG_WRITE 0xF9  // gefolgt von bis zu 2048 zu schreibenden Daten-Bytes, löscht eigenes ACK
#define XCMD_FLASH_DISABLE 0xFA  // 
#define XCMD_ENABLE_DF 0xFB  // Chip-Zugriff /CS über /DF
#define XCMD_ENABLE_QSPI 0xFC  // Chip-Zugriff /CS über /DF
#define XCMD_BUS_TO_ESP32 0xFD  // HX3 Sound Engine to ESP32, ACK from ST32F MCU
#define XCMD_BUS_TO_MCU 0xFE  // HX3 Sound Engine to MCU, ACK from ESP32

// Status Masks
#define XSTA_ACK_MASK 1  // 1 wenn Gegenseite letzten Befehl bestätigt hat
#define XSTA_NAK_ERR_MASK 2  // Fehler, nicht akzeptiert
#define XSTA_RXBUF_GOT_DATA_MASK 4  // 1 wenn Daten abgeholt werden können
#define XSTA_TXBUF_UNREAD_MASK 8  // 1 wenn noch Daten im Sende-Buffer stehen, d.h. warten bis Abholung erfolgt
#define XSTA_BUS_ON_MCU_MASK 16  // Bus an MCU zugewiesen
#define XSTA_BUS_ON_ESP_MASK 32  // Bus an ESP zugewiesen
#define XSTA_FLASH_ON_MASK 64  // Bus mit DF oder QSPI verbunden
#define XSTA_ALWAYS_1_MASK 128  // ID-Bit, immer 1

// Message Header Types
#define XMSG_BINARY_CMD 1  // Binäre Befehle Param/Value
#define XMSG_STRING_CMD 2  // String-Befehl(e) Param="String"
#define XRESP_EDIT_ARR 3  // Enthält komplettes Edit Array 512 Bytes
#define XRESP_EXTD_ARR 4  // Enthält komplettes Extd Array 1536 Bytes
#define XRESP_VOICE_UPPER 5  // Voice-Daten (15 Drawbars als Bytes)
#define XRESP_VOICE_LOWER 6  // Voice-Daten (15 Drawbars als Bytes)
#define XRESP_VOICE_PEDAL 7  // Voice-Daten (15 Drawbars als Bytes)
#define XRESP_TABS 8  // Alle Schalter und Tabs ab 1128

#define XRESP_DATA 10  // Arbiträre Daten
#define XRESP_SINGLE 12  // Antwort auf Request mit Wert

#define XMSG_CMD_ABORTED 14  // Letzter Befehl (z.B. Update) wg. Fehler abgebrochen
#define XMSG_CMD_PROCESSED 15  // Letzter Befehl (z.B. Update) erfolgreich beendet

// Message Request Header Types
#define XREQ_SINGLE 16  // Parameter-Anfrage ("1000?")
#define XREQ_EDIT_ARR 19  // Fordere komplettes Edit Array an, 512 Bytes
#define XREQ_EXTD_ARR 20  // Fordere komplettes Extd Array an, 1536 Bytes
#define XREQ_VOICE_UPPER 21  // Voice-Daten (15 Drawbars als Bytes)
#define XREQ_VOICE_LOWER 22  // Voice-Daten (15 Drawbars als Bytes)
#define XREQ_VOICE_PEDAL 23  // Voice-Daten (15 Drawbars als Bytes)
#define XREQ_TABS 24  // Alle Schalter und Tabs ab 1128
#define XREQ_KNOBS 16  // Alle Knobs ab 1264


// --------------------------------------------------------------------------------

bool df_getChipParams();

// ################################################################################
//
//    ######## ##     ##  ######  ##     ##       ########  ##     ## ######## 
//    ##        ##   ##  ##    ## ##     ##       ##     ## ##     ## ##       
//    ##         ## ##   ##       ##     ##       ##     ## ##     ## ##       
//    ######      ###    ##       #########       ########  ##     ## ######   
//    ##         ## ##   ##       ##     ##       ##     ## ##     ## ##       
//    ##        ##   ##  ##    ## ##     ##       ##     ## ##     ## ##       
//    ######## ##     ##  ######  ##     ##       ########   #######  ##    
//
// ################################################################################


union {
  uint8_t byte[6];
  uint16_t word[3];
  struct  {
    uint16_t magicflag;
    uint16_t msglen; // Anzahl Bytes der Nachricht, max 2042 (2KByte - 6 Byte Header)
    uint16_t chksum; // Einfache Prüfsumme über Daten, aktuell nicht genutzt
  };
} spi_messageheader;

// ------------------------------------------------------------------------------

void spi_dump_buf(uint8_t *data, uint16_t start_addr, uint16_t len, bool print_header = true) {
  // liest len Bytes aus Exchange-RAM des FPGA und gibt sie im seriellen Monitor aus
  if (print_header) {
    Serial.printf("MFlag: 0x%04X, MsgLen: 0x%04X, ChkSum: 0x%04X", spi_messageheader.magicflag, spi_messageheader.msglen, spi_messageheader.chksum);
  }
  start_addr = (start_addr / 16) * 16;
  for (uint16_t i = start_addr; i < start_addr + len; i++) {
    if (i % 16 == 0) {
      Serial.println();
      Serial.printf("%04X", i);
      Serial.print(": ");
    }
    Serial.printf("%02X", data[i]);
    Serial.print(" ");
  }
  Serial.println();
}

// --------------------------------------------------------------------------------

void spi_dump_xchgbuf(uint16_t start_addr, uint16_t len) {
  // liest len Bytes aus FIFO des FPGA und gibt sie inkl. Header im seriellen Monitor aus
  if (len > 2048) {  // 4 Bytes für Flag und Länge!
    len = 2048;
  }
  hspi->beginTransaction(spiSettings);
  _RS_ON; // Register Select LOW für Kommando
  hspi->transfer(XCMD_XCHNG_READ); // Kommando für XCHGBUF, siehe FPGA-Design qspi_connect.vhd
  for (uint16_t i = 0; i < len; i++) {
    spi_blockbuffer.byte[i] = hspi->transfer(0x00);
  }
  _RS_OFF; // Register Select HIGH für Ende
  hspi->endTransaction();
  spi_dump_buf(spi_blockbuffer.byte, start_addr, len, false);
}


void spi_print_status(uint8_t status) {
  Serial.printf("Status: 0x%02X = ", status);
  Serial.print(status, BIN);
  Serial.print(F(" [ "));
  if ((status == 0) || (status == 255)) {
    Serial.println(F("INVALID ]"));
    return;
  }
  if (status & XSTA_FLASH_ON_MASK) Serial.print(F("FLASH_ON "));
  if (status & XSTA_BUS_ON_ESP_MASK) Serial.print(F("BUS_ESP "));
  if (status & XSTA_BUS_ON_MCU_MASK) Serial.print(F("BUS_MCU "));
  if (status & XSTA_TXBUF_UNREAD_MASK) Serial.print(F("TX_UNREAD "));
  if (status & XSTA_RXBUF_GOT_DATA_MASK) Serial.print(F("RX_AVAIL "));
  if (status & XSTA_NAK_ERR_MASK) Serial.print(F("NAK_ERR "));
  if (status & XSTA_ACK_MASK) Serial.print(F("ACK "));
  Serial.println("]");
}

// ################################################################################
//
//    ######## ##     ##  ######  ##     ##        ######  ##     ## ########  
//    ##        ##   ##  ##    ## ##     ##       ##    ## ###   ### ##     ## 
//    ##         ## ##   ##       ##     ##       ##       #### #### ##     ## 
//    ######      ###    ##       #########       ##       ## ### ## ##     ## 
//    ##         ## ##   ##       ##     ##       ##       ##     ## ##     ## 
//    ##        ##   ##  ##    ## ##     ##       ##    ## ##     ## ##     ## 
//    ######## ##     ##  ######  ##     ##        ######  ##     ## ########  
//
// ################################################################################

uint8_t spi_xc_getstatus() {
  // Ein Byte über SPI senden und empfangen
  hspi->beginTransaction(spiSettings);
  _RS_ON; // Register Select LOW für Kommando
  uint8_t status = hspi->transfer(XCMD_STATUS_READ);
  _RS_OFF; // Register Select HIGH für Ende
  hspi->endTransaction();
  return status;
}

void spi_xc_setcmd(uint8_t cmd) {
  // Ein Byte über SPI senden und empfangen
  hspi->beginTransaction(spiSettings);
  _RS_ON; // Register Select LOW für Kommando
  hspi->transfer(cmd);
  _RS_OFF; // Register Select HIGH für Ende
  hspi->endTransaction();
}

err_cmd_t spi_xc_wait_ack(uint16_t timeout = 50) {
  // Warte, bis FPGA-Status gleich ACK ist, Timeout in ms 
  uint32_t startTime = millis();
  while ((spi_xc_getstatus() & XSTA_ACK_MASK) == 0) {
    if (millis() - startTime >= timeout) {
      spi_xc_setcmd(XCMD_NAK_ERR); // ACK zurücksetzen, falls Timeout erreicht
      Serial.println(F("Acknowledge Timeout ERR"));
      return ERR_NOACK; // Timeout erreicht
    }
  }
  return ERR_CMD_OK;
}

err_cmd_t spi_xc_wait_data(uint16_t timeout = 50) {
  // Warte, bis FPGA-Status gleich DATA_READY ist, Timeout in ms
  uint32_t startTime = millis();
  while ((spi_xc_getstatus() & XSTA_RXBUF_GOT_DATA_MASK) == 0) {
    if (millis() - startTime >= timeout) {
      Serial.println(F("Data Timeout ERR"));
      return ERR_NODATA; // Timeout erreicht
    }
  }
  return ERR_CMD_OK;
}

err_cmd_t spi_xc_wait_tx_empty(uint16_t timeout = 50) {
  // Warte, bis FPGA-Status gleich TX_EMPTY ist, Timeout in ms
  uint32_t startTime = millis();
  while ((spi_xc_getstatus() & XSTA_TXBUF_UNREAD_MASK) != 0) {
    if (millis() - startTime >= timeout) {
      Serial.println(F("TX Empty Timeout ERR"));
      return ERR_NOTREAD; // Timeout erreicht
    }
  }
  return ERR_CMD_OK;
}

err_cmd_t spi_xc_wait_status_set(uint8_t mask, uint16_t timeout = 50) {
  // Warte, bis FPGA-Status gleich mask ist, Timeout in ms
  uint32_t startTime = millis();
  while ((spi_xc_getstatus() & mask) != mask) {
    if (millis() - startTime >= timeout) {
      Serial.println(F("Status Set Timeout ERR"));
      return ERR_STATUS; // Timeout erreicht
    }
  }
  return ERR_CMD_OK;
}

err_cmd_t spi_xc_wait_status_clear(uint8_t mask, uint16_t timeout = 50) {
  // Wait until all mask bits cleared, Timeout in ms
  uint32_t startTime = millis();
  while ((spi_xc_getstatus() & mask) != 0) {
    if (millis() - startTime >= timeout) {
      Serial.println(F("Status Clear Timeout ERR"));
      return ERR_STATUS; // Timeout erreicht
    }
  }
  return ERR_CMD_OK;
}
// ------------------------------------------------------------------------------

void spi_xc_flush_rxbuf() {
  // Lese alle Daten aus dem Empfangs-Buffer, bis kein neues Daten-Flag mehr gesetzt ist
  Serial.println(F("Flush XC Rx Buffer"));
  while ((spi_xc_getstatus() & XSTA_RXBUF_GOT_DATA_MASK) != 0){
    hspi->beginTransaction(spiSettings);
    _RS_ON; // Register Select LOW für Kommando
    hspi->transfer(XCMD_XCHNG_READ); // Kommando für XCHGBUF, siehe FPGA-Design qspi_connect.vhd
    for (uint16_t i = 0; i < 2048; i++) {
      hspi->transfer(0x00); // Dummy-Bytes senden, um Daten zu lesen und Buffer zu leeren
    }
    _RS_OFF; // Register Select HIGH für Ende
    hspi->endTransaction();
  }
}

err_cmd_t spi_xc_readfifo(int16_t expected_type = -1) {
  // liest 2KBytes aus Exchange-RAM des FPGA
  // benötigt hierfür etwa 4 ms
  hspi->beginTransaction(spiSettings);
  _RS_ON; // Register Select LOW für Kommando
  hspi->transfer(XCMD_XCHNG_READ); // Kommando für XCHGBUF, siehe FPGA-Design qspi_connect.vhd
  for (uint16_t i = 0; i < 6; i++) {
    spi_messageheader.byte[i] = hspi->transfer(0x00);
  }
  if ((spi_messageheader.magicflag & 0xFF00) != 0x5A00) {
    Serial.print(F("Invalid magic flag in XCHG: 0x"));
    Serial.println(spi_messageheader.magicflag, HEX);
    _RS_OFF; // Register Select HIGH für Ende
    hspi->endTransaction();
    return ERR_INVFLAG;
  }
  if (spi_messageheader.msglen > 2042) {
    spi_messageheader.msglen = 2042;
  }
  for (uint16_t i = 0; i < spi_messageheader.msglen; i++) {
    spi_blockbuffer.byte[i] = hspi->transfer(0x00);
  }
  _RS_OFF; // Register Select HIGH für Ende
  hspi->endTransaction();
  uint16_t chksum = 0;
  for (uint16_t i = 0; i < spi_messageheader.msglen; i++) {
    chksum += spi_blockbuffer.byte[i];
  }
  if (spi_messageheader.chksum != chksum) {
    Serial.print(F("Checksum mismatch in XCHG: expected 0x"));
    Serial.print(spi_messageheader.chksum, HEX);
    Serial.print(F(", calculated 0x"));
    Serial.println(chksum, HEX);
    return ERR_CHKSUM;
  }
  if ((expected_type >= 0) && ((spi_messageheader.magicflag & 0x00FF) != expected_type)) {
    return ERR_INVFLAG;
  }
  return ERR_CMD_OK;
}

// --------------------------------------------------------------------------------

void spi_xc_writefifo(uint16_t len, uint8_t type) {
  // schreibt bis zu 2KBytes in Exchange-RAM des FPGA, 
  // benötigt hierfür etwa 4 ms
  if (spi_xc_wait_tx_empty() != ERR_CMD_OK) {
    return; // Warte, bis vorherige Daten abgeholt wurden
  }
  if (len > 2042) {  // 4 Bytes für Flag und Länge!
    len = 2042;
  }
  spi_messageheader.magicflag = 0x5A00 | type; // Setze Magic Flag mit Typ
  spi_messageheader.msglen = len; // Länge der Message in Bytes
  spi_messageheader.chksum = 0; // Checksum
  for (uint16_t i = 0; i < len; i++) {
    spi_messageheader.chksum += spi_blockbuffer.byte[i];
  }
  hspi->beginTransaction(spiSettings);
  _RS_ON; // Register Select LOW für Kommando
  hspi->transfer(XCMD_XCHNG_WRITE); // Kommando für XCHGBUF, siehe FPGA-Design qspi_connect.vhd
  for (uint16_t i = 0; i < 6; i++) {
    hspi->transfer(spi_messageheader.byte[i]);
  }
  for (uint16_t i = 0; i < len; i++) {
    hspi->transfer(spi_blockbuffer.byte[i]);
  }
  _RS_OFF; // Register Select HIGH für Ende
  hspi->endTransaction();
}



// void spi_xc_fifotest() {
//   // Testet FIFO mit einem bekannten Datenmuster, z.B. 0x00, 0x01, ..., 0xFF, 0x00, ...
//   uint16_t errors = 0;
//   uint16_t count = 0;
//   do {
//     for (uint16_t i = 0; i < 2048; i++) {
//       spi_blockbuffer.byte[i] = i % 256;
//     }
//     spi_xc_writefifo(2000, XMSG_DATA); // Sende Datenmuster über SPI, Länge in Bytes
//     spi_print_status(spi_xc_getstatus());
//     delay(1);
//     memset(spi_blockbuffer.byte, 0, 2048); // Clear spi_blockbuffer vor dem Lesen
//     if (spi_xc_readfifo() != ERR_CMD_OK) { // verify
//       errors++;
//     }
//     count++;
//     spi_dump_buf(spi_blockbuffer.byte, 0, 32);
//   } while (!Serial.available());
//   Serial.print(F("FIFO test completed with "));
//   Serial.print(errors);
//   Serial.print(F(" errors out of "));
//   Serial.print(count);
//   Serial.println(F(" tests."));
// }


// ################################################################################
//
//    ########  #### ##    ##        ######  ##     ## ########  
//    ##     ##  ##  ###   ##       ##    ## ###   ### ##     ## 
//    ##     ##  ##  ####  ##       ##       #### #### ##     ## 
//    ########   ##  ## ## ##       ##       ## ### ## ##     ## 
//    ##     ##  ##  ##  ####       ##       ##     ## ##     ## 
//    ##     ##  ##  ##   ###       ##    ## ##     ## ##     ## 
//    ########  #### ##    ##        ######  ##     ## ########  
//
// ################################################################################

void spi_xc_binarycmd(uint16_t param, uint16_t value, uint16_t timeout = 50) {
  spi_xc_setcmd(XCMD_DONE); // ACK löschen, falls noch gesetzt
  spi_blockbuffer.word[0] = param;
  spi_blockbuffer.word[1] = value;
  spi_xc_writefifo(4, XMSG_BINARY_CMD); // Sende nur die tatsächlich genutzten Bytes, data_width = 16 Bit
  // bei timeout 0 wird nicht auf ACK gewartet, z.B. für lange Updates
  if (timeout > 0) {
    spi_xc_wait_ack(timeout);
  }
}

int16_t spi_xc_binaryreq(uint16_t param, uint16_t timeout = 50) {
  // Wert von MCU anfordern
  spi_xc_setcmd(XCMD_DONE); // ACK löschen, falls noch gesetzt
  spi_blockbuffer.word[0] = param;
  spi_blockbuffer.word[1] = 0;
  spi_xc_writefifo(4, XREQ_SINGLE);
  if (spi_xc_wait_status_set(XSTA_RXBUF_GOT_DATA_MASK, timeout) == ERR_CMD_OK) {
    if (spi_xc_readfifo(XRESP_SINGLE) == ERR_CMD_OK) { 
      // Lese Antwort mit Wert aus Exchange-Fifo des FPGA  
      return spi_blockbuffer.word[1];
    }
  }
  return -1; // Timeout oder Fehler, Rückgabewert -1 als Indikator
}

void spi_xc_binaryresp(uint16_t param, uint16_t value) {
  spi_xc_setcmd(XCMD_DONE); // ACK löschen, falls noch gesetzt
  spi_blockbuffer.word[0] = param;
  spi_blockbuffer.word[1] = value;
  spi_xc_writefifo(4, XRESP_SINGLE); // Sende nur die tatsächlich genutzten Bytes, data_width = 16 Bit
}

// --------------------------------------------------------------------------------

void spi_xc_sendstringcmd(String command, uint16_t timeout = 50) {
  spi_xc_setcmd(XCMD_DONE); // ACK löschen, falls noch gesetzt
  size_t len = command.length();
  char* cstr = new char[len + 1];
  command.toCharArray(cstr, len + 1);
  memset(spi_blockbuffer.byte, 0, 32); // Clear spi_blockbuffer
  // null-terminierten String hinzufügen, damit er als C-String gelesen werden kann
  strncpy((char*)spi_blockbuffer.byte, cstr, len); // Kopiere String in spi_blockbuffer
  delete[] cstr;
  spi_xc_writefifo(++len, XMSG_STRING_CMD); // Sende String, Länge in Bytes
  // bei timeout 0 wird nicht auf ACK gewartet, z.B. für lange Updates
  if (timeout > 0) {
    spi_xc_wait_ack(timeout);
  }
}

void spi_xc_sendLCDmsg_1(String line_1) {
  spi_xc_sendstringcmd("9100=\"" + line_1 + "\""); // Sende Zeile 1 an LCD
}

void spi_xc_sendLCDmsg_2(String line_2) {
  spi_xc_sendstringcmd("9101=\"" + line_2 + "\""); // Sende Zeile 2 an LCD}
}

void spi_xc_sendLCDnumber_1(int16_t topright_number) {
  spi_xc_binarycmd(9102, topright_number); // Sende Zahl für Zeile 1 an LCD, z.B. für Statuscodes
}

void spi_xc_sendLCDnumber_2(int16_t btmright_number) {
  spi_xc_binarycmd(9103, btmright_number); // Sende Zahl für Zeile 2 an LCD, z.B. für Statuscodes
}

// ################################################################################
//
//    ########  ##        #######   ######  ##    ##    ##     ##  ######  
//    ##     ## ##       ##     ## ##    ## ##   ##      ##   ##  ##    ## 
//    ##     ## ##       ##     ## ##       ##  ##        ## ##   ##       
//    ########  ##       ##     ## ##       #####          ###    ##       
//    ##     ## ##       ##     ## ##       ##  ##        ## ##   ##       
//    ##     ## ##       ##     ## ##    ## ##   ##      ##   ##  ##    ## 
//    ########  ########  #######   ######  ##    ##    ##     ##  ######  
//
// ################################################################################


void spi_xc_request_editArray() {
  // Fordere editArray von MCU an, warte auf Daten und speichere sie ab
  DXCPRINTLNF("EditArray Req... ");
  spi_xc_setcmd(XCMD_DONE); // ACK löschen, falls noch gesetzt
  spi_xc_writefifo(1, XREQ_EDIT_ARR); // Sende Request für editArray
  if (spi_xc_wait_data() == ERR_CMD_OK) {
    spi_xc_readfifo(); // Lese editArray aus Exchange-RAM des FPGA
    DXCPRINTLNF("EDIT_ARR received");
    DXCDUMP(64); // Debug: Dump der gelesenen Daten im seriellen Monitor
    memcpy(hx3EditArray, spi_blockbuffer.byte, 512); // Kopiere gelesene Daten in hx3EditArray, Länge in Bytes
  }
  spi_xc_setcmd(XCMD_ACK);
}

void spi_xc_request_extendedArray() {
  // Fordere extendedArray von MCU an, warte auf Daten und speichere sie ab
  DXCPRINTLNF("ExtdArray Req... ");
  spi_xc_setcmd(XCMD_DONE); // ACK löschen, falls noch gesetzt
  spi_xc_writefifo(1, XREQ_EXTD_ARR); // Sende Request für extendedArray
  if (spi_xc_wait_data() == ERR_CMD_OK) {
    spi_xc_readfifo(); // Lese extendedArray aus Exchange-RAM des FPGA
    DXCPRINTLNF("EXTD_ARR received");
    DXCDUMP(64); // Debug: Dump der gelesenen Daten im seriellen Monitor
    memcpy(hx3ExtendedArray, spi_blockbuffer.byte, 1536); // Kopiere gelesene Daten in hx3ExtendedArray, Länge in Bytes
  }
  spi_xc_setcmd(XCMD_ACK);
}


void spi_xc_interpret_data() {
  uint8_t new_status = spi_xc_getstatus();
  if ((new_status & XSTA_RXBUF_GOT_DATA_MASK) && (new_status != 0) && (new_status != 0xFF)) { 
    spi_xc_setcmd(XCMD_DONE);
    spi_print_status(new_status);
    if (spi_xc_readfifo() == ERR_CMD_OK) { // Lese Daten aus Exchange-RAM des FPGA
      uint16_t msg_type = spi_messageheader.magicflag & 0x00FF;
      switch (msg_type) {
        case XREQ_EDIT_ARR:
          // Sende hx3EditArray als editArray an FPGA, Länge in Bytes
          DXCPRINTLNF("EDIT_ARR requ by MCU");
          memcpy(spi_blockbuffer.byte, hx3EditArray, 512); // Kopiere Daten aus hx3EditArray in spi_blockbuffer, Länge in Bytes
          spi_xc_writefifo(512, XRESP_EDIT_ARR); // Sende spi_blockbuffer als editArray über SPI, Länge in Bytes
          spi_xc_wait_ack(); // Warte, bis MCU das editArray abgeholt hat
          break;
        case XREQ_EXTD_ARR:
          // Sende hx3ExtendedArray als extendedArray an FPGA, Länge in Bytes
          DXCPRINTLNF("EXTD_ARR requ by MCU");
          memcpy(spi_blockbuffer.byte, hx3ExtendedArray, 1536); // Kopiere Daten aus hx3ExtendedArray in spi_blockbuffer, Länge in Bytes
          spi_xc_writefifo(1536, XRESP_EXTD_ARR); // Sende spi_blockbuffer als extendedArray über SPI, Länge in Bytes
          spi_xc_wait_ack(); // Warte, bis MCU das extendedArray abgeholt hat
          break;
        case XRESP_EDIT_ARR:
          DXCPRINTLNF("EDIT_ARR received");
          DXCDUMP(64); // Debug: Dump der gelesenen Daten im seriellen Monitor
          memcpy(hx3EditArray, spi_blockbuffer.byte, 512); // Kopiere gelesene Daten in hx3EditArray, Länge in Bytes
          break;
        case XRESP_EXTD_ARR:
          DXCPRINTLNF("EXTD_ARR received");
          DXCDUMP(64); // Debug: Dump der gelesenen Daten im seriellen Monitor
          memcpy(hx3ExtendedArray, spi_blockbuffer.byte, 1536); // Kopiere gelesene Daten in hx3ExtendedArray, Länge in Bytes
          break;
        case XMSG_CMD_ABORTED:
          DXCPRINTLNF("CMD_ABORTED received");
          break;
        case XMSG_CMD_PROCESSED:
          DXCPRINTLNF("CMD_PROCESSED received");
          break;
        case XRESP_SINGLE:          
          // Antwort auf Binary Request, Parameter-Anfragen
          DXCPRINTLNF("SINGLE value received: Param=0x");
          DXCPRINT(spi_blockbuffer.word[0], HEX);
          DXCPRINTLNF(", Value=0x");
          DXCPRINT(spi_blockbuffer.word[1], HEX);
          break;
        default:
          Serial.print(F("Unknown message type in XCHG: 0x"));
          Serial.println(msg_type, HEX);
          break;
      }
      spi_xc_setcmd(XCMD_ACK);
    }
  }
}


// ################################################################################


bool spi_xc_qspi_activate() {
  spi_xc_binarycmd(9980, 1); // Bus to ESP32, siehe parser.mpas
  spi_xc_binarycmd(9981, 1, 500); // Stop SAM5504, Enable QSPI for ESP32, siehe parser.mpas
  // schaltet SPI auf externes QSPI-Flash um, damit SPI darauf zugreifen kann
  if (spi_xc_wait_status_set(XSTA_BUS_ON_ESP_MASK | XSTA_FLASH_ON_MASK, 1000) == ERR_CMD_OK) {
    // Aktivierung über MCU hat funktioniert, jetzt Chip-Parameter lesen
    Serial.println(F("Activate QSPI, reading chip params..."));
    if (df_getChipParams()) {
      return true;
    }
  }
  spi_xc_setcmd(XCMD_FLASH_DISABLE);
  spi_xc_setcmd(XCMD_BUS_TO_MCU);
  return false;
}

void spi_xc_qspi_deactivate() {
  // Trennt QSPI wieder vom FPGA, damit SAM5504 wieder Zugriff auf Flash bekommt
  spi_xc_binarycmd(9980, 0); // Bus to MCU
  spi_xc_binarycmd(9981, 0, 500); // Start SAM5504, disable QSPI for ESP32, siehe parser.mpas
  spi_xc_wait_status_clear(XSTA_BUS_ON_ESP_MASK | XSTA_FLASH_ON_MASK, 1000);
}


#endif // SPI_XCHANGE_H