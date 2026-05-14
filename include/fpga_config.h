#ifndef FPGA_CONFIG_h
#define FPGA_CONFIG_h

// ################################################################################
//
//     ######  ########  ####    ##     ## ######## ######## ########  
//    ##    ## ##     ##  ##      ##   ##  ##       ##       ##     ## 
//    ##       ##     ##  ##       ## ##   ##       ##       ##     ## 
//     ######  ########   ##        ###    ######   ######   ########  
//          ## ##         ##       ## ##   ##       ##       ##   ##   
//    ##    ## ##         ##      ##   ##  ##       ##       ##    ##  
//     ######  ##        ####    ##     ## ##       ######## ##     ## 
//
// ################################################################################

#include <Arduino.h>
#include <LittleFS.h>
#include "global_vars.h"
#include <SPI.h>

//uninitialized pointer to SPI objects
SPIClass *hspi = NULL;

// define a union of the same array in byte and word representation
// for easier access to the 4KByte block buffer
// Double size to prevent overflow on HTML uploads
union {
  uint8_t byte[DF_BLOCKSIZE];
  uint16_t word[DF_BLOCKSIZE / 2];
  int16_t integer[DF_BLOCKSIZE / 2];
  uint32_t dword[DF_BLOCKSIZE / 4];
} spi_blockbuffer;


// siehe
// https://randomnerdtutorials.com/esp32-spi-communication-arduino/#spi-multiple-bus-hspi-vspi

// FPGA Pin 126 = MISO an ESP32 Pin 16 (GPIO16, HSPI MISO, optional, für Rückmeldungen vom FPGA, z.B. Statusregister)
// FPGA Pin 127 = MOSI an ESP32 Pin 13 (GPIO13, HSPI MOSI, optional, für Befehle ans FPGA, z.B. Statusregister)
// FPGA Pin 132 = SCK an ESP32 Pin 14 (GPIO14, HSPI Clock, optional
// FPGA Pin 128 = RS an ESP32 Pin 4 (GPIO4, Register Select für FPGA-Kommunikation, LOW für Registeradresse, HIGH für Daten)
// FPGA Pin 129 = DS an ESP32 Pin 5 (GPIO5, Data Select für FPGA-Kommunikation, LOW für Daten, HIGH für Ende)

void init_FPGAcomm() {
  // Init HSPI for FPGA communication (Registerzugriff, Datenübertragung)
  // FPGA Communication Pins
  pinMode(ESP_RS, OUTPUT);
  pinMode(ESP_DS, OUTPUT);
  _RS_OFF;
  _DS_OFF;
  hspi = new SPIClass(HSPI);
  hspi->begin(ESP_SCK, ESP_MISO, ESP_MOSI, -1); // SCK, MISO, MOSI, SS (nicht verwendet)
}

// #############################################################################
// Low-Level-Funktionen für SPI-Kommunikation mit FPGA
// hspi->beginTransaction(spiSettings) muss vorher erfolgt sein!
// hspi->endTransaction() muss hinterher erfolgen
// #############################################################################

uint8_t spi_xfer8_ds(uint8_t data) {
  // Ein Byte über SPI senden und empfangen
  _DS_ON; // Data Select LOW für Daten
  uint8_t rxbyte = hspi->transfer(data);
  _DS_OFF; // Data Select HIGH für Daten-Ende
  return rxbyte;
}

uint16_t spi_xfer16_ds(uint16_t data) {
  // union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } in, out;
  uint16_t rxword;
  _DS_ON; // Data Select LOW für Daten
  rxword  = hspi->transfer16(data);
  _DS_OFF; // Data Select HIGH für Daten-Ende
  return rxword;
}

uint32_t spi_xfer32_ds(uint32_t data) {
  uint32_t rxlong;
  _DS_ON; // Data Select LOW für Daten
  rxlong  = hspi->transfer32(data);
  _DS_OFF; // Data Select HIGH für Daten-Ende
  return rxlong;
}

void spi_sendreg_rs(uint8_t reg) {
  // Ein Byte über SPI senden und empfangen
  _RS_ON; // Register Select LOW für Kommando
  hspi->transfer(reg);
  _RS_OFF; // Register Select HIGH für Ende
}

// #############################################################################

void spi_sendreg(uint8_t reg) {
  // Ein Byte über SPI senden und empfangen
  hspi->beginTransaction(spiSettings);
  _RS_ON; // Register Select LOW für Kommando
  hspi->transfer(reg);
  _RS_OFF; // Register Select HIGH für Ende
  hspi->endTransaction();
}

void spi_write32(uint8_t reg, uint32_t data) {
  hspi->beginTransaction(spiSettings);
  spi_sendreg_rs(reg); // Register auswählen
  spi_xfer32_ds(data);
  hspi->endTransaction();
}

uint32_t spi_read32(uint8_t reg) {
  hspi->beginTransaction(spiSettings);
  spi_sendreg_rs(reg); // Register auswählen
  uint32_t data = spi_xfer32_ds(0);
  hspi->endTransaction();
  return data;
}

void spi_write8(uint8_t spi_reg, uint8_t data) {
  hspi->beginTransaction(spiSettings);
  spi_sendreg_rs(spi_reg); // Write-Flag 0, Register senden
  spi_xfer8_ds(data);
  hspi->endTransaction();
}

uint8_t spi_read8(uint8_t spi_reg) {
  hspi->beginTransaction(spiSettings);
  spi_sendreg_rs(spi_reg); // Write-Flag 0, Register senden
  uint8_t data = spi_xfer8_ds(0x00);
  hspi->endTransaction();
  return data;
}



#endif //FPGA_CONFIG_h