
//https://goldenmorninglcd.com/de/tft-Anzeige/2.79-Zoll-142x428-nv3007-t279vj-c10-01/

#define TFT_WIDTH  142
#define TFT_HEIGHT 428
#define TFT_INVERSION_ON
#define TFT_COLUMN_OFFSET 0
#define TFT_ROW_OFFSET 14
 
#define TFT_SDA_READ   // Display has a bidirectional SDA pin
#define CGRAM_OFFSET

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x00 // Not used unless commandlist invoked

#define TFT_MADCTL   0x36
#define TFT_MAD_MY   0x80
#define TFT_MAD_MX   0x40
#define TFT_MAD_MV   0x20
#define TFT_MAD_ML   0x10
#define TFT_MAD_BGR  0x08
#define TFT_MAD_MH   0x04

#define TFT_RGB_ORDER TFT_RGB
#define TFT_MAD_COLOR_ORDER 0x00

#define TFT_NOP      0x00
#define TFT_SWRST    0x01
#define TFT_INVOFF   0x20
#define TFT_INVON    0x21
#define TFT_DISPOFF  0x28
#define TFT_DISPON   0x29
#define TFT_CASET    0x2A
#define TFT_PASET    0x2B
#define TFT_RAMWR    0x2C
#define TFT_RAMRD    0x2E