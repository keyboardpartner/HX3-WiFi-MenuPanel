#ifndef PARSER_H
#define PARSER_H

#include <Arduino.h>
#include <ctype.h>
#include "global_vars.h"
#include "fpga_config.h"
#include "spi_xchange.h"
#include "dataflash.h"

typedef struct {
    const char *mnemonic;
    uint16_t index;
} MnemonicPair;

const MnemonicPair mnemonicPairs[] = {
    {"STR", 255}, // Status Request 255
    {"IDN", 254}, // Identify Version number 254
    {"VAL", 0},   // 0..9999
    {"NOP", 0},
};

// -----------------------------------------------------------------------------
// Parser: Wert holen
// -----------------------------------------------------------------------------

void parseGetValue(uint16_t index) {
    // Hier können die Werte für die GET-Kommandos zurückgegeben werden, z.B. über Serial.print()
    if (index <= 3) {
        uint32_t spi_result = spi_read32(index);
        Serial.print(spi_result);
        Serial.print(" [");
        Serial.print(spi_result, HEX);
        Serial.println("]");
    } else if (index >= 1000) {
        int16_t resp = spi_xc_binaryreq(index); // Sende Binary Request an MCU
        Serial.println(resp);
    } else {
        uint8_t new_status;
        switch (index) {
            case 247:
                new_status = spi_xc_getstatus();
                spi_print_status(new_status);
                break;
            case 400:
                showDirectory();
                break;
            case 401:
                _LED_ON;
                break;
            case 402:
                _LED_OFF;
                break;

#ifdef DEBUG_SPI_TESTS
            case 500:
                spi_xc_request_editArray();
                break;
            case 501:
                spi_xc_request_extendedArray();
                break;
            // case 520:
            //   spi_xc_fifotest();
            //   break;
            case 555:
                spi_xc_sendstringcmd("1001=55", 100);
                break;
            case 600:
            case 601:
            case 602:
            case 603:
            case 604:
            case 605:
            case 606:
            case 607:
                index -= 600;
                spi_dump_xchgbuf(index * 256, 256);
                break;
            case 608:
                spi_dump_buf((uint8_t *)hx3EditArray, 0, 64);
                break;
            case 609:
                spi_dump_buf((uint8_t *)hx3ExtendedArray, 0, 64);
                break;
#endif
            case 610:
                DPRINTLNF("Activate QSPI for ESP");
                spi_xc_qspi_activate();
                break;
            case 611:
                DPRINTLNF("Cancel QSPI, reboot DSP");
                spi_xc_qspi_deactivate();
                DPRINTLNF("Done.");
                break;
            case 612:
                df_getChipParams();
                break;
            case 613:
                df_reset();
                break;
            case 700:
                spi_xc_sendstringcmd("1000=85");
                break;
            case 701:
                spi_xc_sendstringcmd("1000?");
                break;
            case 710:
                DPRINTLNF("Force QSPI for ESP");
                spi_xc_setcmd(XCMD_BUS_TO_ESP32);
                spi_xc_setcmd(XCMD_ENABLE_QSPI);
                break;
            case 711:
                DPRINTLNF("Bus to MCU, disable QSPI for ESP");
                spi_xc_setcmd(XCMD_BUS_TO_MCU);
                DPRINTLNF("Done.");
                break;

            default:
                Serial.print("Invalid index: ");
                Serial.println(index);
                break;
        }
    }
}

void parseGetValueMnemonic(const char *mnemonic, uint8_t offset) {
    // convert mnemonic to uppercase for case-insensitive comparison
    char upperMnemonic[16];
    size_t i;
    for (i = 0; i < sizeof(upperMnemonic) - 1 && mnemonic[i] != '\0'; i++) {
        upperMnemonic[i] = toupper(mnemonic[i]);
    }
    upperMnemonic[i] = '\0';
    // Search for the mnemonic in the mnemonicPairs array and call parseGetValue with the corresponding index
    for (size_t i = 0; i < sizeof(mnemonicPairs) / sizeof(MnemonicPair); i++) {
        if (strcmp(upperMnemonic, mnemonicPairs[i].mnemonic) == 0) {
            parseGetValue(mnemonicPairs[i].index + offset);
            return;
        }
    }
    Serial.println("/ Invalid mnemonic");
}

// -----------------------------------------------------------------------------
// Parser: Wert setzen
// -----------------------------------------------------------------------------

void parseSetValue(uint16_t index, uint32_t value) {
    // Hier können die Werte für die SET-Kommandos gesetzt werden
    if (index <= 240) {
        spi_write32(index, value);
    } else {
        if (index >= 1000) {
            spi_xc_binarycmd(index, value); // Sende einfachen Binary Command an FPGA, siehe parser.mpas
        } else {
            switch (index) {
                case 556:
                    spi_dump_xchgbuf(value, 64);
                    break;
                case 620:
                    df_getChipParams();
                    df_dump_block(value, 256);
                    break;
                case 621:
                    df_getChipParams();
                    df_dump_block(value, 4096);
                    break;
                case 9999:
                    resetRequested = true;
                    break;
                default:
                    Serial.print("/ Invalid index: ");
                    Serial.println(index);
                    break;
            }
        }
    }
}

void parseSetValueMnemonic(const char *mnemonic, uint8_t offset, uint32_t value) {
    // convert mnemonic to uppercase for case-insensitive comparison
    char upperMnemonic[16];
    size_t i;
    for (i = 0; i < sizeof(upperMnemonic) - 1 && mnemonic[i] != '\0'; i++) {
        upperMnemonic[i] = toupper(mnemonic[i]);
    }
    upperMnemonic[i] = '\0';
    // Search for the mnemonic in the mnemonicPairs array and call parseSetValue with the corresponding index
    for (size_t i = 0; i < sizeof(mnemonicPairs) / sizeof(MnemonicPair); i++) {
        if (strcmp(upperMnemonic, mnemonicPairs[i].mnemonic) == 0) {
            parseSetValue(mnemonicPairs[i].index + offset, value);
            return;
        }
    }
    Serial.println("/ Invalid mnemonic");
}

typedef struct {
    char firstMnemonic[8];   // Optional: Ein kurzer String zur Identifikation des Kommandos
    char secondMnemonic[16]; // Optional: Ein kurzer String zur Identifikation des Wertes
    uint16_t first;          // Erster Wert, z.B. Index oder Parameter
    uint8_t first_offset;    // Optional: Offset für den ersten Wert wenn Mnemonic genutzt wird, z.B. "EDT 5 = 1234" könnte Index 1005 ansprechen
    uint32_t second;         // Optional: Zweiter Wert, z.B. Wert zum Setzen
    bool firstIsMnemonic;    // Gibt an, ob der erste Wert als Mnemonic interpretiert werden soll
    bool secondIsMnemonic;   // Gibt an, ob der zweite Wert als Mnemonic interpretiert werden soll
    bool hasSecond;
} ParseResult;

ParseResult parseCommand(const char *input) {
    ParseResult result = {{0}, {0}, 0, 0, 0, false, false, false};
    if (input == NULL || input[0] == '\0') {
        return result;
    }

    int index = 0;

    // Skip leading whitespace
    while (input[index] == ' ' || input[index] == '\t') {
        index++;
    }

    // Parse first value (number or mnemonic)
    if (isdigit(input[index])) {
        // Parse as number
        uint16_t firstNum = 0;
        while (input[index] != '\0' && isdigit(input[index])) {
            firstNum = firstNum * 10 + (input[index] - '0');
            index++;
        }
        result.first = firstNum;
        result.firstIsMnemonic = false;
    } else if (isalpha(input[index])) {
        // Parse as mnemonic
        int mnemonicIndex = 0;
        while (input[index] != '\0' && isalnum(input[index]) &&
               mnemonicIndex < sizeof(result.firstMnemonic) - 1) {
            result.firstMnemonic[mnemonicIndex++] = input[index++];
        }
        result.firstMnemonic[mnemonicIndex] = '\0';
        result.firstIsMnemonic = true;

        // Skip whitespace after mnemonic
        while (input[index] == ' ' || input[index] == '\t') {
            index++;
        }

        // Check for optional offset (number after mnemonic, e.g., "EDT 5")
        if (isdigit(input[index])) {
            uint8_t offsetNum = 0;
            while (input[index] != '\0' && isdigit(input[index])) {
                offsetNum = offsetNum * 10 + (input[index] - '0');
                index++;
            }
            result.first_offset = offsetNum;
        }
    }

    // Skip whitespace before delimiter
    while (input[index] == ' ' || input[index] == '\t') {
        index++;
    }

    // Check for delimiter '='
    if (input[index] == '=') {
        index++;
        result.hasSecond = true;

        // Skip whitespace after delimiter
        while (input[index] == ' ' || input[index] == '\t') {
            index++;
        }

        // Parse second value (number or mnemonic)
        if (isdigit(input[index])) {
            // Parse as number
            uint32_t secondNum = 0;
            while (input[index] != '\0' && isdigit(input[index])) {
                secondNum = secondNum * 10 + (input[index] - '0');
                index++;
            }
            result.second = secondNum;
            result.secondIsMnemonic = false;
        } else if (isalpha(input[index])) {
            // Parse as mnemonic
            int mnemonicIndex = 0;
            while (input[index] != '\0' &&
                   isalnum(input[index]) && mnemonicIndex < sizeof(result.secondMnemonic) - 1) {
                result.secondMnemonic[mnemonicIndex++] = input[index++];
            }
            result.secondMnemonic[mnemonicIndex] = '\0';
            result.secondIsMnemonic = true;
        }
    }

    return result;
}

void checkSerialCommand() {
    static char buffer[64] = {0};
    static int bufferIndex = 0;
    ParseResult result = {{0}, {0}, 0, 0, 0, false, false, false};

    while (Serial.available()) {
        char c = Serial.read();
        if (c == 0x1B) { // Escape = Cancel Command
            bufferIndex = 0;
            buffer[0] = '\0';
            Serial.println("/ Command input cancelled");
            return;
        }

        if (c == '\n' || c == '\r') {
            if (bufferIndex > 0) {
                buffer[bufferIndex] = '\0';
                result = parseCommand(buffer);
                bufferIndex = 0;
                if (result.hasSecond) {
                    if (result.firstIsMnemonic) {
                        // Handle first value as mnemonic with optional offset
                        parseSetValueMnemonic(result.firstMnemonic, result.first_offset, result.second);
                    } else {
                        parseSetValue(result.first, result.second);
                    }
                } else {
                    if (result.firstIsMnemonic) {
                        // Handle first value as mnemonic with optional offset
                        parseGetValueMnemonic(result.firstMnemonic, result.first_offset);
                    } else {
                        parseGetValue(result.first);
                    }
                }
            }
            return;
        } else if (c == 127 || c == '\b') { // DEL or backspace
            if (bufferIndex > 0) {
                bufferIndex--;
                buffer[bufferIndex] = '\0';
            }
        } else if (bufferIndex < sizeof(buffer) - 1) {
            buffer[bufferIndex++] = c;
        }
    }
}

#endif // PARSER_H