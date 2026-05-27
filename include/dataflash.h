#ifndef DATAFLASH_H
#define DATAFLASH_H

// #############################################################################
//
//     ######     #    #######    #    ####### #          #     #####  #     #
//     #     #   # #      #      # #   #       #         # #   #     # #     #
//     #     #  #   #     #     #   #  #       #        #   #  #       #     #
//     #     # #     #    #    #     # #####   #       #     #  #####  #######
//     #     # #######    #    ####### #       #       #######       # #     #
//     #     # #     #    #    #     # #       #       #     # #     # #     #
//     ######  #     #    #    #     # #       ####### #     #  #####  #     #
//
// #############################################################################

// Most DF support Mode 0 with Clock Polarity (CPOL) = 0 and, Clock Phase (CPHA) = 0
// or Mode 3 with CPOL = 1 and, CPHA = 1

// QSPI enthält ab Block 15616 die DFUDL-Liste mit den zu ladenden Blöcken, siehe dataflash.mpas
// SAM5504 speichert nach Umwandlung aus DFU als Rohdaten, allerdings mit umgekehrter Byte-Reihenfolge
// (Worte mit MSB First). Damit die MCU ST32F sie wie DFU-Daten interpretiert,
// müssen sie vor dem Schreiben in den DataFlash entsprechend umgewandelt werden.

// Es darf hier also keine DFU-Datei abgelegt werden, sondern eine Datei mit DFUDL-Header,
// wie sie vom HX3 Manager Bootload Utility erzeugt wird.

#include <Arduino.h>
#include "global_vars.h"
#include "fpga_config.h"
#include "spi_xchange.h"
#include "files.h"

#define FLAG_32BIT_ADDR    0x01  // larger than 16 MByte address
#define FLAG_STATUS_CMD70  0x02  // requires special busy flag check
#define FLAG_DIFF_SUSPEND  0x04  // uses 2 different suspend commands
#define FLAG_MULTI_DIE     0x08  // multiple die, don't read cross 32M barrier
#define FLAG_256K_BLOCKS   0x10  // has 256K erase blocks
#define FLAG_DIE_MASK      0xC0  // top 2 bits count during multi-die erase

#define ID0_WINBOND   0xEF
#define ID0_SPANSION  0x01
#define ID0_MICRON    0x20
#define ID0_MACRONIX  0xC2
#define ID0_SST       0xBF
#define ID0_ADESTO    0x1F

static const int dfClk = 8000000;  // duch Leitungslänge nicht mehr als 8 MHz!
SPISettings dfSettings(dfClk, MSBFIRST, SPI_MODE3);

typedef struct {
  uint16_t FileStartIdx;
  uint16_t FileEntryCount;
  char filename[28];
} updatePart_t;

typedef struct {
  // Zieladressen (Blocknummern) der enthaltenen Blocks mit Destination im oberen Nibble
  uint16_t BlockEntries[1024]; // bis zu 1024 Ziel-Blocknummern incl. DestinationCode im oberen Nibble
  updatePart_t FileEntries[48];  // 1536 Bytes @ Byte 2486
  uint8_t DummyBytes[464];  // @ Byte 3584, Filler
  // 12 x 4 Bytes = 48 Bytes @ Byte 4048, oberes Ende des Blocks:
  uint32_t Exitflag;            // DW #1012 Exit Bootloader wenn <> 0
  uint32_t Param_count;         // DW #1013 Ist eine Parameter-Update-Liste > 0
  uint32_t Timestamp;           // DW #1014 Unix-Timestanp (Sekunden seit 1.1.1970)
  uint32_t PartBits;            // DW #1015 Bit-Einträge, welche Parts upgedated wurden
  uint32_t ScanFailsafeBcount;  // DW #1016 ist 0 wenn kein scanbkup.dat enthalten
  uint32_t FPGAFailsafeBcount;  // DW #1017 ist 0 wenn kein FPGA-Failsafe enthalten
  uint32_t FwFailsafeBcount;    // DW #1018 ist 0 wenn kein FW-Failsafe enthalten
  uint32_t FPGAversion;         // DW #1019 optional: Version geladenes FPGA $0006 0xxx
  uint32_t FWversion;           // DW #1020 optional: Version geladene Firmware $0006 0xxx
  uint32_t Datachecksum;        // DW #1021 DW-Summe aller Datenblocks
  uint32_t BlockCount;          // DW #1022 Total Block Count in DFUDL_DW
  uint32_t Validflag;           // DW #1023 Offset in DFUDL_DW-Array, $55AA
} dfudlInfo_t;

union {
  uint8_t byte[DF_BLOCKSIZE];
  uint16_t word[DF_BLOCKSIZE / 2];
  int16_t integer[DF_BLOCKSIZE / 2];
  uint32_t dword[DF_BLOCKSIZE / 4];
  dfudlInfo_t dfudlInfo;
} df_buffer;


struct {
  uint8_t flags = 0; // Flags für Chip-spezifische Eigenschaften, z.B. 32-Bit-Adressierung, spezielle Statusabfrage, etc.
  uint8_t id[5];
  uint32_t size;
  uint32_t eraseBlockSize;
} dfChip;


  // DFUprefix wird von SAM2DFU.exe
  // vor jedem Datenblock (dann nur noch 2048 Bytes!) eingefügt
struct {  // DREAM DFU element prefix, 12 Bytes
    uint32_t DestAddr;    // beginnt mit $81E00000, zählt in Words!
    uint16_t Count;   // Anzahl Words bis zum nächsten Prefix, $400
    uint16_t Signature;   // Firmware Signature, 0x4452 bei DSP-FW, 0x000A bei Manager-DFU
    uint32_t Fill;        // immer 0
} dfu_prefix, dfu_temp_prefix;



uint8_t df_wait() {
  uint8_t status;
  _DF_ON;
  hspi->transfer(0x05);
  do {
    status = hspi->transfer(0x00);
    delayMicroseconds(10);
  } while (status & 0x01);
  _DF_OFF;
  return status;
}


void df_reset() {
  hspi->beginTransaction(dfSettings);
  _DF_ON;
  hspi->transfer(0xF0); // Enable Reset
  _DF_OFF;
  delay(10);
  hspi->endTransaction();
}

void df_readID() {
  hspi->beginTransaction(dfSettings);
  _DF_ON;
  hspi->transfer(0x9F);
  dfChip.id[0] = hspi->transfer(0); // manufacturer ID
  dfChip.id[1] = hspi->transfer(0); // memory type
  dfChip.id[2] = hspi->transfer(0); // capacity
  if (dfChip.id[0] == ID0_SPANSION) {
    dfChip.id[3] = hspi->transfer(0); // ID-CFI
    dfChip.id[4] = hspi->transfer(0); // sector size
  }
  _DF_OFF;
  hspi->endTransaction();
  //Serial.printf("ID: %02X %02X %02X\n", dfChip.id[0], dfChip.id[1], dfChip.id[2]);
}

uint32_t df_capacity(const uint8_t *id) {
  uint32_t n = 1048576; // unknown chips, default to 1 MByte
  if (id[0] == ID0_ADESTO && id[1] == 0x89) {
    n = 1048576*16; //16MB
  } else
  if (id[2] >= 16 && id[2] <= 31) {
    n = 1ul << id[2];
  } else
  if (id[2] >= 32 && id[2] <= 37) {
    n = 1ul << (id[2] - 6);
  } else
  if ((id[0]==0 && id[1]==0 && id[2]==0) || 
    (id[0]==255 && id[1]==255 && id[2]==255)) {
    n = 0;
  }
  return n;
}

// #############################################################################

bool df_getChipParams() {
  // Lese ID und Größe des DataFlash, speichere sie in globalen Variablen
  dfChip.flags = 0; // reset flags
  df_readID();
  if ((dfChip.id[0]==0 && dfChip.id[1]==0 && dfChip.id[2]==0) || (dfChip.id[0]==255 && dfChip.id[1]==255 && dfChip.id[2]==255)) {
    DXCPRINTLNF("DataFlash ID invalid");
    return false;
  }
  dfChip.size = df_capacity(dfChip.id);
  #ifdef DEBUG
    Serial.printf("DataFlash ID: %02X %02X %02X\n", dfChip.id[0], dfChip.id[1], dfChip.id[2]);
    Serial.printf("DataFlash Size: %lu bytes\n", dfChip.size);
  #endif
  if (dfChip.size > 16777216) {
    // more than 16 Mbyte requires 32 bit addresses
    dfChip.flags |= FLAG_32BIT_ADDR;
    if (dfChip.id[0] == ID0_MICRON) dfChip.flags |= FLAG_MULTI_DIE;
  }
  if (dfChip.id[0] == ID0_SPANSION) {
    // Spansion has separate suspend commands
    dfChip.flags |= FLAG_DIFF_SUSPEND;
    if (!dfChip.id[4]) {
      // Spansion chips with dfChip.id[4] == 0 use 256K sectors
      dfChip.flags |= FLAG_256K_BLOCKS;
      dfChip.eraseBlockSize = 262144; // Spansion chips >= 512 mbit use 256K sectors
    } else {
      dfChip.eraseBlockSize = 4096;   // everything else seems to have 4K sectors
    }
  }
  if (dfChip.id[0] == ID0_MICRON) {
    // Micron requires busy checks with a different command
    dfChip.flags |= FLAG_STATUS_CMD70; // TODO: all or just multi-die chips?
  }
  return true;
}

// #############################################################################

void df_unprotect() {
  hspi->beginTransaction(dfSettings);
  _DF_ON;
  hspi->transfer(0x06); // Write Enable
  _DF_OFF;
  delayMicroseconds(1);
  _DF_ON;
  hspi->transfer(0x01);
  hspi->transfer(0x00); // Write 0, Global Unprotect
  _DF_OFF;
  df_wait();
  hspi->endTransaction();
}

void df_protect() {
  hspi->beginTransaction(dfSettings);
  _DF_ON;
  hspi->transfer(0x06); // Write Enable
  _DF_OFF;
  delayMicroseconds(1);
  _DF_ON;
  hspi->transfer(0x01);
  hspi->transfer(0x1C); // Write $1C, Global Protect
  _DF_OFF;
  df_wait();
  hspi->endTransaction();
}

void df_chiperase() {
  // Sollte nur im Notfall verwendet werden, da es sehr lange dauert
  uint32_t start_time = millis();
  VERBOSE_DPRINTF("CHIP ERASE... ");
  hspi->beginTransaction(dfSettings);
  _DF_ON;
  hspi->transfer(0x06); // Write Enable
  _DF_OFF;
  delayMicroseconds(1);
  _DF_ON;
  hspi->transfer(0x01);
  hspi->transfer(0x00); // Write 0, Global Unprotect
  _DF_OFF;
  df_wait();
  _DF_ON;
  hspi->transfer(0x06); // Write Enable
  _DF_OFF;
  delayMicroseconds(1);
  _DF_ON;
  hspi->transfer(0xC7);
  _DF_OFF;
  df_wait();
  hspi->endTransaction();
  VERBOSE_DPRINTF("done in ");
  VERBOSE_DPRINT(millis() - start_time);
  VERBOSE_DPRINTLNF(" ms");
}

void df_eraseblock(uint16_t block_4k) {
  // Lösche 4-KByte-Block, nur bei Winbond 25Q32JV, andere Chips haben möglicherweise andere Befehle oder unterstützen kein Block-Erase
  // liefert TRUE wenn erfolgreich
  uint32_t addr = (uint32_t)block_4k * 4096;
  hspi->beginTransaction(dfSettings);
  _DF_ON;
  hspi->transfer(0x06); // Write Enable
  _DF_OFF;
  delayMicroseconds(1);
  _DF_ON;
  if (dfChip.flags & FLAG_32BIT_ADDR) {
    hspi->transfer(0xDC); // 256 KByte Block Erase, 4-Byte-Adresse
    hspi->transfer16(addr >> 16);
    hspi->transfer16(addr);
  } else {
    hspi->transfer16(0x2000 | ((addr >> 16) & 255));
    hspi->transfer16(addr);
  }
  _DF_OFF;
  df_wait();
  hspi->endTransaction();
  VERBOSE_DPRINTF("Erased Block 0x");
  VERBOSE_DPRINT(addr / 4096, HEX);
  VERBOSE_DPRINTF(", ");
  VERBOSE_DPRINT(dfChip.eraseBlockSize);
  VERBOSE_DPRINTLNF(" bytes");

}

uint16_t df_verifyblock(uint16_t block_4k, uint16_t df_blocklen, bool swapped_bytes) {
  // Vergleiche BlockBuffer8 mit DataFlash, max. 4096 bytes
  // liefert TRUE wenn Block in DataFlash mit BlockBuffer8 übereinstimmt
  uint32_t addr = (uint32_t)block_4k * 4096;
  uint16_t err_count = 0;
  uint16_t word_count = df_blocklen / 2;
  hspi->beginTransaction(dfSettings);
  _DF_ON;
  if (dfChip.flags & FLAG_32BIT_ADDR) {
    hspi->transfer(0x13); // 256 KByte Block Erase, 4-Byte-Adresse
    hspi->transfer16(addr >> 16);
    hspi->transfer16(addr);
  } else {
    hspi->transfer16(0x0300 | ((addr >> 16) & 255));
    hspi->transfer16(addr);
  }
  for (uint16_t idxw = 0; idxw < word_count; idxw++) {
    uint8_t verify_byte1 = hspi->transfer(0x00);
    uint8_t verify_byte2 = hspi->transfer(0x00);
    uint16_t idxb = idxw * 2;
    if (swapped_bytes) {
      if (df_buffer.byte[idxb] != verify_byte2 || df_buffer.byte[idxb + 1] != verify_byte1) {
        err_count++;
      }
    } else {
      if (df_buffer.byte[idxb] != verify_byte1 || df_buffer.byte[idxb + 1] != verify_byte2) {
        err_count++;
      }
    }
  }
  _DF_OFF;
  hspi->endTransaction();
  VERBOSE_DPRINTF("Verify Block 0x");
  VERBOSE_DPRINT(block_4k, HEX);
  if (err_count > 0) {
    DPRINTLN();
    DPRINTF("ERR: Verify, count: ");
    DPRINTLN(err_count);
  } else {
    VERBOSE_DPRINTLNF(" - OK");
  }
  return err_count;
}

void df_readblock(uint16_t block_4k, uint16_t df_blocklen) {
  // Lese BlockBuffer8 aus DataFlash, max. 4096 bytes
  uint32_t addr = (uint32_t)block_4k * DF_BLOCKSIZE;
  hspi->beginTransaction(dfSettings);
  _DF_ON;
  if (dfChip.flags & FLAG_32BIT_ADDR) {
    hspi->transfer(0x13); // read, 4-Byte-Adresse
    hspi->transfer16(addr >> 16);
    hspi->transfer16(addr);
  } else {
    hspi->transfer16(0x0300 | ((addr >> 16) & 255));
    hspi->transfer16(addr);
  }
  for (uint16_t idxw = 0; idxw < df_blocklen; idxw++) {
    df_buffer.byte[idxw] = hspi->transfer(0x00);
  }
  _DF_OFF;
  hspi->endTransaction();
}

void df_writeblock(uint16_t block_4k, uint16_t df_blocklen, bool swapped_bytes = false) {
  // Schreibe BlockBuffer8 in DataFlash, max. 4096 bytes
  // liefert TRUE wenn erfolgreich
  // df_blocklen sollte Vielfaches von 256 sein,
  // es können max. 256 Bytes auf einmal geschrieben werden
  // Vorher muss df_unprotect() und ggf. df_eraseblock_4k()
  // aufgerufen werden, damit der Block beschreibbar ist!
  uint32_t addr = (uint32_t)block_4k * DF_BLOCKSIZE;
  uint16_t idx = 0;
  hspi->beginTransaction(dfSettings);
  do {
    _DF_ON;
    hspi->transfer(0x06); // Write Enable
    _DF_OFF;
    df_wait();
    _DF_ON;
    if (dfChip.flags & FLAG_32BIT_ADDR) {
      hspi->transfer(0x12); // 4-Byte-Adresse
      hspi->transfer16(addr >> 16);
      hspi->transfer16(addr);
    } else {
      hspi->transfer16(0x0200 | ((addr >> 16) & 255));
      hspi->transfer16(addr);
    }
    if (swapped_bytes) {
      for (uint16_t i = 0; i < 128; i++) {
        // Bei umgekehrter Byte-Reihenfolge (MSB First) müssen die Bytes paarweise getauscht werden
        hspi->transfer(df_buffer.byte[idx + 1]);
        hspi->transfer(df_buffer.byte[idx]);
        idx += 2;
      }
    } else {
      for (uint16_t i = 0; i < 256; i++) {
        hspi->transfer(df_buffer.byte[idx + i]);
      }
      idx += 256;
    }
    _DF_OFF;
    df_wait();
    addr += 256;
  } while (idx < df_blocklen);
  hspi->endTransaction();
}

// #############################################################################
//
//    ########  ########    ######## #### ##       ########  ######  
//    ##     ## ##          ##        ##  ##       ##       ##    ## 
//    ##     ## ##          ##        ##  ##       ##       ##       
//    ##     ## ######      ######    ##  ##       ######    ######  
//    ##     ## ##          ##        ##  ##       ##             ## 
//    ##     ## ##          ##        ##  ##       ##       ##    ## 
//    ########  ##          ##       #### ######## ########  ######   
//                                                            
// #############################################################################

err_df_t df_flash_file(String df_filename, uint16_t block_4k, bool do_verify, bool swapped_bytes = true) {
  // Highlevel-Funktion: Lese Datei von SPIFFS und schreibe sie in DataFlash, z.B. für Firmware-Updates
  // In QSPI immer mit umgekehrter Byte-Reihenfolge (MSB First) speichern,
  // damit die MCU ST32F sie wie Daten vom SAM5504 interpretieren kann
  err_df_t err = ERR_DF_OK;
  char filename[32];
  uint32_t next_erase_limit = (block_4k * DF_BLOCKSIZE); // Set the next erase limit
  // Concatenate "/" and df_filename into filename
  strcpy(filename, "/");
  strcat(filename, df_filename.c_str());
  DPRINTF("Flash file <");
  DPRINT(filename);
  DPRINTF("> to start block 0x");
  DPRINTLN(block_4k, HEX);
  File df_file = LittleFS.open(filename, "r");
  uint16_t current_block = block_4k;
  if (df_file) {
    uint32_t start_time = millis();
    int file_size = 0;
    while (df_file.available()) {
      // folgen noch weitere Daten?
      if (current_block * DF_BLOCKSIZE >= next_erase_limit) {
        // Erase next block if we have crossed the erase limit
        df_eraseblock(current_block);
        next_erase_limit += dfChip.eraseBlockSize; // Set the next erase limit
      }
      memset(df_buffer.byte, 0xFF, DF_BLOCKSIZE); // BlockBuffer mit 0xFF füllen, damit unbeschriebene Bereiche im Block mit 0xFF verglichen werden können
      int byte_count = df_file.read(df_buffer.byte, DF_BLOCKSIZE); // Lese bis zu 1024 Instruktionen (4096 Bytes) auf einmal
      file_size += byte_count; // Anzahl der gelesenen Bytes zählen
      df_writeblock(current_block, DF_BLOCKSIZE, swapped_bytes); // Schreibe BlockBuffer in DataFlash, max. 4096 bytes
      if (do_verify && (df_verifyblock(current_block, DF_BLOCKSIZE, swapped_bytes) > 0)) {
        err = ERR_DF_VERIFY;
        break;
      }     
      current_block++;
    }
    DPRINTF("Stored to DF, ");
    DPRINT(file_size);
    DPRINTF(" bytes in ");
    DPRINT(millis() - start_time);
    DPRINTLN(" ms");
  } else {
    DPRINTF("ERR: Failed to open ");
    DPRINTLN(filename);
    err = ERR_DF_FILE;
  }
  // Close File
  df_file.close();
  return err;
}

// #############################################################################

#ifdef DEBUG_DF_TESTS

void df_dump_block(uint16_t block_4k, uint16_t len) {
  // Debug-Funktion: Dump BlockBuffer und DataFlash-Blockinhalt über Serial ausgeben, max. 4096 bytes
  Serial.print(F("Dump Flash Block from address 0x"));
  Serial.println(uint32_t(block_4k) * 4096, HEX);
  df_readblock(block_4k, len); // Lese aktuellen Block in BlockBuffer
  spi_dump_buf(df_buffer.byte, 0, len);
}

// ------------------------------------------------------------------------------

err_df_t df_verify_file(const char* df_filename, uint16_t block_4k) {
  // Highlevel-Funktion: Lese Datei von SPIFFS und vergleiche sie mit DataFlash
  err_df_t err = ERR_DF_OK;
  char filename[32];
  uint32_t next_erase_limit = (block_4k * DF_BLOCKSIZE); // Set the next erase limit
  // Concatenate "/" and df_filename into filename
  strcpy(filename, "/");
  strcat(filename, df_filename);
  DPRINTF("Verify file <");
  DPRINT(filename);
  DPRINTF("> from start block 0x");
  DPRINTLN(block_4k, HEX);
  File df_file = LittleFS.open(filename, "r");
  uint16_t current_block = block_4k;
  if (df_file) {
    uint32_t start_time = millis();
    int file_size = 0;
    while (df_file.available()) {
      df_readblock(current_block, DF_BLOCKSIZE); // Lese BlockBuffer aus DataFlash, max. 4096 bytes
      for (uint16_t i = 0; i < DF_BLOCKSIZE; i++) {
        uint8_t file_byte = df_file.read();
        file_size++;
        if (file_byte != df_buffer.byte[i]) {
          err = ERR_DF_VERIFY;
          DPRINTF("First verify error at offset ");
          DPRINT((current_block * DF_BLOCKSIZE) + i);
          DPRINTF(": file has 0x");
          DPRINT(file_byte, HEX);
          DPRINTF(", but DF has 0x");
          DPRINT(df_buffer.byte[i], HEX);
          DPRINTLN();
          if (i > 10) break;
        }
      }
      if (err != ERR_DF_OK) {
        break; // Fehler => Abbruch
      }
      current_block++;
    }
    DPRINTF("Verified DF, ");
    DPRINT(file_size);
    DPRINTLNF(" bytes");
  } else {
    DPRINTF("ERR: Failed to open ");
    DPRINTLN(filename);
    err = ERR_DF_FILE;
  }
  // Close File
  df_file.close();
  return err;
}

#endif


// #############################################################################
//
//    ##     ## ########  ##        #######     ###    ########  
//    ##     ## ##     ## ##       ##     ##   ## ##   ##     ## 
//    ##     ## ##     ## ##       ##     ##  ##   ##  ##     ## 
//    ##     ## ########  ##       ##     ## ##     ## ##     ## 
//    ##     ## ##        ##       ##     ## ######### ##     ## 
//    ##     ## ##        ##       ##     ## ##     ## ##     ## 
//     #######  ##        ########  #######  ##     ## ########  
//                                                            
// #############################################################################

uint32_t DFUDLdataChecksum = 0; // Checksumme über alle Datenbytes, damit die Summe aller Datenbytes inklusive dieser Checksumme 0xFFFFFFFF ergibt
uint32_t DFUblockCount = 0;   // Anzahl der verarbeiteten Blöcke
uint32_t DFUtotalByteCount = 0;   // Anzahl der verarbeiteten Bytes
uint32_t CurrentQSPIaddress = 0;     // Zieladresse im QSPI
uint16_t CurrentQSPIblock = 0;     // Zieladresse im QSPI
uint32_t NextQSPIeraseLimit = BLOCK_DSP_DFU * DF_BLOCKSIZE; // Nächste Löschgrenze
bool UploadHasDFUprefix = false; // Gibt an, ob der DFUDL-Header einen Prefix enthält
bool UploadIsForDSP = false;  // Daten sind für DSP bestimmt
bool UploadHasDFUDL = false;  // Gibt an, ob die Datei bereits einen DFUDL-Header enthält
uint16_t DFUprefixCount = 0;     // Zähler DFU-Prefix-Bytes
uint16_t DFUchunkBytecount = 0;    // Anzahl der verarbeiteten Bytes
uint16_t DFUfillBytecount = 0;    // Anzahl der verarbeiteten Bytes

enum prefixState_t {
  COLLECTING_PREFIX,
  COLLECTING_DATA,
  SUFFIX_REACHED
}; // Aktueller Zustand der DFU-Chunk-Verarbeitung, initial auf WAIT_FOR_PREFIX setzen

prefixState_t DFUprefixState = COLLECTING_PREFIX;


bool qspi_collect_stream(uint8_t databyte) {
  // collects incoming data byte by byte in df_buffer, returns TRUE if full  
  if (UploadHasDFUprefix) {
    // Es kommt als erstes der DFU-Prefix
    // alle (dfu_prefix.Count) Bytes den DFU-Prefix überspringen, die im DFUDL-Header enthalten sind, damit die Daten direkt in den DataFlash geschrieben werden können
    switch (DFUprefixState) {
      case COLLECTING_PREFIX: 
        DFUchunkBytecount = 0; // Block-Zähler zurücksetzen
        ((uint8_t*)&dfu_temp_prefix)[DFUprefixCount] = databyte; // Aktuelles Byte in dfu_temp_prefix schreiben
        DFUprefixCount++;
        if (DFUprefixCount == sizeof(dfu_prefix)) {
          // DFU-Prefix komplett empfangen, nächstes Byte ist  Datenbyte, jetzt mit dem Sammeln der Daten beginnen
          DFUprefixState = COLLECTING_DATA; // Jetzt die Daten sammeln, bis der nächste DFU-Prefix kommt
          DFUchunkBytecount = 0; // Byte-Zähler für den aktuellen Datenblock zurücksetzen
          if (dfu_temp_prefix.Fill == 0x4F270800) {
            // DFU-Prefix mit Fill 0x4F270800 kennzeichnet die Suffix-Bytes am Ende der Datei, 
            // die nicht ins DataFlash geschrieben werden sollen
            DFUprefixState = SUFFIX_REACHED; // Suffix-Bytes erreicht
            return true; // Suffix-Bytes überspringen
          }
        }
        break;
      case COLLECTING_DATA:
        DFUprefixCount = 0; // Byte-Zähler für den DFU-Prefix zurücksetzen
        df_buffer.byte[DFUfillBytecount] = databyte; // Aktuelles Datenbyte in Buffer speichern
  
        DFUfillBytecount++; // Byte-Zähler erhöhen
        DFUchunkBytecount++; // Byte-Zähler für den aktuellen Chunk erhöhen
        DFUtotalByteCount++; // Gesamt-Byte-Zähler erhöhen

        if (DFUchunkBytecount >= dfu_prefix.Count) {
          // Alle Datenbytes für den aktuellen DFU-Prefix empfangen, jetzt auf den nächsten DFU-Prefix warten
          DFUprefixState = COLLECTING_PREFIX; // Nächster DFU-Prefix kommt jetzt, wieder mit dem Sammeln des Prefix beginnen
        }

        if (DFUfillBytecount >= DF_BLOCKSIZE) {
          DFUfillBytecount = 0; // Byte-Zähler zurücksetzen
          return true; // Block wurde gefüllt
        }
        break;
      case SUFFIX_REACHED:
        // Suffix-Bytes am Ende der Datei erreicht, nicht mehr in DataFlash schreiben
        break;
    }
  } else {
    // Kein DFU-Prefix, die Datei enthält nur die reinen Daten, die direkt in den DataFlash geschrieben werden können
    df_buffer.byte[DFUfillBytecount] = databyte; // Aktuelles Datenbyte in Buffer speichern
    DFUfillBytecount++; // Byte-Zähler erhöhen
    if (DFUfillBytecount >= DF_BLOCKSIZE) {
      DFUfillBytecount = 0; // Byte-Zähler zurücksetzen
      return true; // Block wurde gefüllt
    }
  }
  return false; // Prefix- und Suffix-Bytes überspringen
}

err_df_t qspi_flash_dfu_chunk(uint8_t *data, uint32_t index, uint16_t len, bool is_final, bool do_verify) {
  // Flash incoming data chunk directly to DataFlash, called from server.h
  // *data: Zeiger auf den Daten-Buffer mit dem aktuellen Chunk
  // index: Byte-Index des aktuellen Chunks innerhalb der Datei
  // len: Länge des aktuellen Chunks in Bytes
  // is_final: Gibt an, ob dies der letzte Chunk ist
  // do_verify: Gibt an, ob der geschriebene Block überprüft werden soll
  err_df_t err = ERR_DF_OK;
  //static uint32_t offset_for_dfudl = 0; // ggf. 1 Block für DFUDL reservieren
  if (index == 0) {
    // Erster Block 0 wurde für DFUDL freigehalten
    // Start of transmission: Erase first Block
    DFUprefixCount = 0; // Reset chunk byte count at the start of the upload
    DFUDLdataChecksum = 0; // Reset checksum at the start of the upload
    DFUblockCount = 0; // Reset block count at the start of the upload
    DFUchunkBytecount = 0; // Reset byte count at the start of the upload
    DFUfillBytecount = 0; // Reset byte count at the start of the upload
    DFUtotalByteCount = 0; // Reset byte count at the start of the upload
    memcpy(&dfu_prefix, data, sizeof(dfu_prefix)); // Ersten Block direkt in Buffer schreiben, damit er mit DFUDL-Header zusammen geschrieben
    if ((dfu_prefix.DestAddr >= 0x80020000) && (dfu_prefix.DestAddr <= 0x81E80000) && (dfu_prefix.Count == 0x0400) && (dfu_prefix.Fill == 0)) {
      // Datei enthält bereits einen DFU-Prefix, der die Zieladresse enthält, also keinen DFUDL-Header mehr anlegen
      // DSP-Firmware hat Signature 0x4452, Manager-DFU oder Soundbank hat Signature 0x000A
      UploadIsForDSP = (dfu_prefix.Signature == 0x4452) || (dfu_prefix.DestAddr == 0x80080000) || (dfu_prefix.DestAddr == 0x80500000); 
      UploadHasDFUDL = (dfu_prefix.Signature == 0x000A) && (dfu_prefix.DestAddr == 0x81E80000); // Manager-DFU enthält bereits einen DFUDL-Header, DSP-Firmware nicht
      dfu_prefix.DestAddr &= 0x7FFFFFFF; // Zieladresse im Prefix enthält Bit 31 als DF-Flag, das hier für die Berechnung der Adresse in DataFlash entfernt werden muss
      dfu_prefix.DestAddr *= 2; // Zieladresse war in Worten angegeben, für die Berechnung der Byte-Adresse muss sie mit 2 multipliziert werden
      dfu_prefix.Count *= 2;    // Count war in Worten angegeben
      UploadHasDFUprefix = true;
      //offset_for_dfudl = 0; // kein zusätzlicher Block für DFUDL-Header nötig, da die Datei bereits einen Prefix enthält
      DPRINTF("Detected DFU file with prefix, dest addr: 0x");
      DPRINTLN(dfu_prefix.DestAddr, HEX);
      CurrentQSPIaddress = dfu_prefix.DestAddr; // Berechne die Zieladresse in DataFlash, ggf. mit Offset für DFUDL-Header
      DFUprefixState = COLLECTING_PREFIX; // Start with collecting the DFU prefix
    } else {
      // Datei ist Binary
      dfu_prefix.DestAddr = BLOCK_DSP_DFU * DF_BLOCKSIZE; // Kein Prefix, Zieladresse ist der Startblock für DFU, hier 0x80000000
      dfu_prefix.Count = 0x0800;
      UploadHasDFUprefix = false;
      UploadIsForDSP = false;
      UploadHasDFUDL = false; // Datei enthält keinen DFUDL-Header
      DPRINTLN("No DFU prefix detected, assuming binary file");
      CurrentQSPIaddress = dfu_prefix.DestAddr + DF_BLOCKSIZE; // Berechne die Zieladresse in DataFlash mit Offset für DFUDL-Header
      DFUprefixState = COLLECTING_DATA; // Start with collecting data
    }
    NextQSPIeraseLimit = dfu_prefix.DestAddr; // Reset the next erase limit, ersten Block auf jeden Fall
    memset(df_buffer.byte, 0xFF, DF_BLOCKSIZE); // BlockBuffer zurücksetzen
    DPRINTF("Handled block ");
  }

  for (uint16_t i = 0; i < len; i++) {
    // Daten sammeln, bis der BlockBuffer gefüllt ist, dann in DataFlash schreiben
    bool buf_filled = qspi_collect_stream(data[i]);
    if (buf_filled || (is_final && (i == len - 1)) || (DFUprefixState == SUFFIX_REACHED)) {
      // Write Block when filled or final byte of the file is reached 
      // oder Suffix-Bytes am Ende der Datei erreicht
      CurrentQSPIblock = CurrentQSPIaddress / DF_BLOCKSIZE;
      // Buffer ist voll oder es ist das letzte Byte, schreibe ihn in DataFlash
      Serial.write(27);  // Fortschrittsanzeige
      Serial.write('7'); // save cursor pos
      DPRINTF(" 0x");
      DPRINT(CurrentQSPIblock, HEX);
      if (CurrentQSPIaddress >= NextQSPIeraseLimit) {
        // Erase next block if we have crossed the erase limit
        DPRINTF(" E");
        df_eraseblock(CurrentQSPIblock);
        NextQSPIeraseLimit += dfChip.eraseBlockSize; // Set the next erase limit
      } else {
        DPRINTF("  ");
      }
      Serial.write(27);  // Fortschrittsanzeige
      Serial.write('8'); // restore cursor pos
      df_writeblock(CurrentQSPIblock, DF_BLOCKSIZE, true); // Schreibe BlockBuffer in DataFlash, max. 4096 bytes, mit umgekehrter Byte-Reihenfolge (MSB First)
      if (do_verify && (df_verifyblock(CurrentQSPIblock, DF_BLOCKSIZE, true) > 0)) {
        err = ERR_DF_VERIFY;
      }
      for (uint16_t i = 0; i < 1024; i++) {
        DFUDLdataChecksum += df_buffer.dword[i]; // Update checksum with the bytes that were just written
      }
      memset(df_buffer.byte, 0xFF, DF_BLOCKSIZE); // BlockBuffer zurücksetzen
      DFUblockCount++; // Increment block count for DFUDL header
      CurrentQSPIaddress += DF_BLOCKSIZE; // Zieladresse für den nächsten Block erhöhen
      if (DFUprefixState == SUFFIX_REACHED) {
        // Suffix-Bytes am Ende der Datei erreicht, nicht mehr in DataFlash schreiben
        DPRINTLN();
        DPRINTLN("DFU Suffix reached");
        break;
      }    
    }
  }
  return err;
}

void qspi_flash_dfudl(int16_t file_index, bool exit_bl){
  // Finally flash a DFUDL block at flash range start for single file update if needed.
  // First block has been left unprogrammed to reserve it for the DFUDL header,
  // now we can write the DFUDL header with the correct block count and checksum
  if (UploadHasDFUDL || UploadIsForDSP) {
    if (UploadHasDFUDL) {
      DPRINTF("File already contains DFUDL header");
    }
    if (UploadIsForDSP) {
      DPRINTF("File is for DSP, no DFUDL needed");
    }
    DPRINTF(", blocks: ");
    DPRINT(DFUblockCount);
    DPRINTF(", net bytes: ");
    DPRINTLN(DFUtotalByteCount);
    // Datei enthält bereits einen DFUDL-Header oder ist für DSP, daher ist keine Erstellung eines DFUDL-Blocks mehr nötig
    return; 
  }
  getCurrentFileItem(file_index); // Hole die Informationen zur hochgeladenen Datei, insbesondere die Startadresse im QSPI, um den DFUDL-Header korrekt zu erstellen
  DPRINTF("Flashing DFUDL block for destination 0x");
  DPRINT(currentFileItem.startBlock, HEX);
  DPRINTF(", type code: 0x");
  DPRINT(currentFileItem.partType, HEX);
  DPRINTF(", BlockCount: ");
  DPRINT(DFUblockCount);
  DPRINTF(" (+1), Checksum: ");
  DPRINTLN(DFUDLdataChecksum, HEX);

  memset(df_buffer.byte, 0, DF_BLOCKSIZE); // BlockBuffer mit 0 füllen
  df_buffer.dfudlInfo.Validflag = 0x000055AA;
  df_buffer.dfudlInfo.Datachecksum = DFUDLdataChecksum; // Checksum so that sum of all data bytes including this checksum is 0xFFFFFFFF
  df_buffer.dfudlInfo.BlockCount = ++DFUblockCount; // tatsächlich erhaltene Blöcke plus 1 für DFUDL-Block
  df_buffer.dfudlInfo.Exitflag = exit_bl;
  df_buffer.dfudlInfo.PartBits = 1; // nur 1 Datei
  df_buffer.dfudlInfo.FileEntries[0].FileStartIdx = 1; // Start-Index der Datei in der Blockliste, hier 0 da nur eine Datei
  df_buffer.dfudlInfo.FileEntries[0].FileEntryCount = DFUblockCount; // Anzahl der Blöcke in der Datei, hier gleich der Anzahl der erhaltenen Blöcke
  strcpy(df_buffer.dfudlInfo.FileEntries[0].filename, currentFileItem.filename); // Dateiname

  // nur die unteren 12 Bits für die Blocknummer,
  // obere 4 Bits = Destination Flags,
  // Bit 12 = DF (dest=1), Bit 13 = ReInit-Kennung (dest+2), Bit 14 = STM32 IntFlash Firmware (dest=4), Bit 15 = Failsafe (+8)
  df_buffer.word[0] = 0x1000 | BLOCK_UPDATE_INFO; // Erster Eintrag immer Update Info Block 0x027D, Bit 12 = 1 = DF (dest 1)
  uint16_t dest_block = (currentFileItem.startBlock & 0x0FFF) | ((currentFileItem.partType & 0x000F) << 12); 
  for (uint16_t i = 1; i < DFUblockCount; i++) {
    df_buffer.word[i] = dest_block++; // Einzelne Datei enthält nur fortlaufende Blocks
  }
  // Flash the DFUDL block to DataFlash without offset
  df_writeblock(BLOCK_DSP_DFU, DF_BLOCKSIZE, true); // Schreibe BlockBuffer in DataFlash, max. 4096 bytes
}

// #8300, direkter Befehl ($0..$F) an Bootloader
// z.b. c_blcmd_unpackdfu: Word     = $55A1;

// nur unteres Nibble (0..F) als Befehl!
//  c_blcmd_startdfu: Word      = $55A0;
//  c_blcmd_unpackdfu: Word     = $55A1;
//  c_blcmd_failsafe_fw: Word   = $55A2;
//  c_blcmd_failsafe_fpga: Word = $55A3;
//  c_blcmd_sdload: Word        = $55A4;
//  c_blcmd_stopdfu: Word       = $55A5;  // Serielles Menu
//  c_blcmd_menu: Word          = $55AA;  // LCD-Menu
//  c_blcmd_done: Word          = $55FF;  // von BL zurückgeliefert

err_df_t qspi_notify_update(int16_t file_idx, bool is_dfu) {
  err_df_t err = ERR_DF_OK;
  getCurrentFileItem(file_idx); // nur um die Datei-Nummer zu bekommen, die im Bootloader für den Zugriff auf die Datei in QSPI benötigt wird
  if (is_dfu || (file_idx == 0) || (file_idx == 2)) {
    // Datei ist DFU, FPGA oder Firmware
    DPRINTLNF("Invoke bootloader for DSP/FW/FPGA update");
    spi_xc_binarycmd(8300, 1, 0); // Bootloader Command: Reboot and decode DFU data
  } else {
    DPRINTF("Uploaded binary file ");
    DPRINT(currentFileItem.filename);
    DPRINTLNF(" stored to QSPI");
    // Create string like "8309=filename.bin" for SPI command
    if (file_idx >= 0) {
      spi_xc_binarycmd(8310, 1, 0);  // start flashing
    } else {
      DPRINTF("ERR: File not in list");
      err = ERR_DF_FILE;
    }
  }
  return err;
}


#endif // DATAFLASH_H
