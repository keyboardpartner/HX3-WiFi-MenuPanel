#ifndef FILES_H
#define FILES_H

#include <Arduino.h>
#include <LittleFS.h>
#include "global_vars.h"
#include "fpga_config.h"

typedef struct {
  char filename[16];
  uint16_t startBlock; // Ziel-Block-Nummer im DataFlash
  uint16_t blockCount;  // hier nicht benutzt, über (File-Länge / 4096) berechnet
  uint16_t partType;   // Bit 0 = DF, 1 = ReInit-Kennung, 2 = Firmware, 3 = Failsafe
} updateFileType;

updateFileType currentFileItem;

#define LC_NONE 255

const updateFileType filelist[42] PROGMEM = {
  {"fpgamain.bin", BLOCK_FPGA, 304, 3},  // 1: FPGA-Bitstream, wird direkt an FPGA gesendet
  {"fpgabkup.bin", BLOCK_FPGA, 304, 9},  // 2: FPGA-Bitstream Backup
  {"firmware.bin", BLOCK_FIRMWARE, 128, 4},  // 3: Firmware, nur Bootloader!
  {"fwbackup.bin", BLOCK_FAILSAFE_FW, 128, 9}, // 4: Firmware Backup, wird direkt von der MCU verarbeitet
  {"scan.dat",     BLOCK_SCAN, 2, 1},  // 5: Scan-Treiber
  {"scanbkup.dat", BLOCK_SCAN, 2, 9},  // 6: Scan-Treiber
  {"scanmidi.dat", BLOCK_SCAN, 2, 1},  // 7: Scan-Treiber
  {"scanfatr.dat", BLOCK_SCAN, 2, 1},  // 8: Scan-Treiber
  {"scanft61.dat", BLOCK_SCAN, 2, 1},  // 9: Scan-Treiber
  {"scanft73.dat", BLOCK_SCAN, 2, 1},  // 10: Scan-Treiber
  {"scansr61.dat", BLOCK_SCAN, 2, 1},  // 11: Scan-Treiber
  {"scanpl61.dat", BLOCK_SCAN, 2, 1},  // 12: Scan-Treiber
  {"scanxb25.dat", BLOCK_SCAN, 2, 1},  // 13: Scan-Treiber
  {"taper1.dat",   BLOCK_TAPER_0, 1, 1}, // 14: Taperings
  {"taper2.dat",   BLOCK_TAPER_1, 1, 1}, // 15: Taperings
  {"taper3.dat",   BLOCK_TAPER_2, 1, 1}, // 16: Taperings
  {"taper4.dat",   BLOCK_TAPER_3, 1, 1}, // 17: Taperings
  {"waveset0.bin", BLOCK_WAVESET_0, 4, 1}, // 18: Wavesets
  {"waveset1.bin", BLOCK_WAVESET_1, 4, 1}, // 19: Wavesets
  {"waveset2.bin", BLOCK_WAVESET_2, 4, 1}, // 20: Wavesets
  {"waveset3.bin", BLOCK_WAVESET_3, 4, 1}, // 21: Wavesets
  {"waveset4.bin", BLOCK_WAVESET_4, 4, 1}, // 22: Wavesets
  {"waveset5.bin", BLOCK_WAVESET_5, 4, 1}, // 23: Wavesets
  {"waveset6.bin", BLOCK_WAVESET_6, 4, 1}, // 24: Wavesets
  {"waveset7.bin", BLOCK_WAVESET_7, 4, 1}, // 25: Wavesets
  {"fir_coe.dat",  BLOCK_FIR_COEFF, 1, 1}, // 26: FIR Coefficients
  {"voices.dat",   BLOCK_VOICE, 1, 1}, // 27: Voice Data
  {"organs.dat",   BLOCK_ORGAN_MODEL_BASE, 16, 1}, // 28: Organ Defaults
  {"speakers.dat", BLOCK_SPEAKER_MODEL_BASE, 16, 1}, // 29: Speaker Models
  {"presets.dat",  BLOCK_PRESET_BASE, 128, 1}, // 30: Presets
  {"defaults.dat", BLOCK_DEFAULTS, 1, 1}, // 31: Defaults
  {"ccset0.dat",   BLOCK_CCSET_0, 1, 1}, // 32: MIDI CC Sets
  {"ccset1.dat",   BLOCK_CCSET_1, 1, 1}, // 33: MIDI CC Sets
  {"ccset2.dat",   BLOCK_CCSET_2, 1, 1}, // 34: MIDI CC Sets
  {"ccset3.dat",   BLOCK_CCSET_3, 1, 1}, // 35: MIDI CC Sets
  {"ccset4.dat",   BLOCK_CCSET_4, 1, 1}, // 36: MIDI CC Sets
  {"ccset5.dat",   BLOCK_CCSET_5, 1, 1}, // 37: MIDI CC Sets
  {"ccset6.dat",   BLOCK_CCSET_6, 1, 1}, // 38: MIDI CC Sets
  {"ccset7.dat",   BLOCK_CCSET_7, 1, 1}, // 39: MIDI CC Sets
  {"ccset8.dat",   BLOCK_CCSET_8, 1, 1}, // 40: MIDI CC Sets
  {"ccset9.dat",   BLOCK_CCSET_9, 1, 1}, // 41: MIDI CC Sets
  {"ccset10.dat",  BLOCK_CCSET_10, 1, 1}, // 42: MIDI CC Sets
};

int16_t lookupFileName(const char* filename) {
  for (uint8_t i = 0; i < sizeof(filelist) / sizeof(filelist[0]); i++) {
    memcpy_P (&currentFileItem, &filelist[i], sizeof(currentFileItem));
    if (strcmp(currentFileItem.filename, filename) == 0) {
      return i;
    }
  }
  memset(&currentFileItem, 0, sizeof(currentFileItem)); // clear currentFileItem if not found
  return -1; // not found
}

void getCurrentFileItem(int16_t idx) {
  if (idx >= 0 && idx < sizeof(filelist) / sizeof(filelist[0])) {
    memcpy_P(&currentFileItem, &filelist[idx], sizeof(currentFileItem));
    return;
  }
  memset(&currentFileItem, 0, sizeof(currentFileItem)); // clear currentFileItem if not found
}


void showDirectory() {
  // Hilfsfunktion, um alle Dateien im LittleFS-Verzeichnis auf der seriellen Konsole anzuzeigen
  dividerLine();
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  String filename;
  while (file) {
    Serial.print(file.name());
    Serial.print(" (");
    Serial.print(file.size());
    Serial.println(" bytes)");
    file = root.openNextFile();
  }
  root.close();
  dividerLine();
}

#endif  // FILES_H