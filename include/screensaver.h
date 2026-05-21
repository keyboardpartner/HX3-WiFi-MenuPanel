#ifndef screensaver_h
#define screensaver_h


#include <Arduino.h>
#include "global_vars.h"
#include "drawing.h"

// #############################################################################
// ANALOG CLOCK SCREENSAVER
// #############################################################################

float sec_x = 0, sec_y = 1, min_x = 1, min_y = 0, hour_x = -1, hour_y = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osec_x=120, osec_y=120, omin_x=120, omin_y=120, ohour_x=120, ohour_y=120;  // Saved H, M, S x & y coords
uint16_t x0=0, x1=0, yy0=0, yy1=0;
uint32_t targetTime = 0;                    // for next 1 second timeout

uint8_t hh=0, mm=0, ss=0, ss_old = 99;  // Get H, M, S from compile time

bool initial = 1;
#define CLOCK_RADIUS 80
#define HOUR_LEN 45
#define MIN_LEN 60
#define SEC_LEN 70

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

void analogClock() {
  time(&now);
  timeinfo = localtime(&now);

  hh = timeinfo->tm_hour;
  mm = timeinfo->tm_min;  
  ss = timeinfo->tm_sec;

  tft.fillScreen(TFT_CHARCOAL);
  // Draw clock face
  tft.fillCircle(DISPLAY_CENTER_X, DISPLAY_CENTER_Y, CLOCK_RADIUS + 4, TFT_MEDGREY);
  tft.fillCircle(DISPLAY_CENTER_X, DISPLAY_CENTER_Y, CLOCK_RADIUS, TFT_BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sec_x = cos((i-90)*0.0174532925);
    sec_y = sin((i-90)*0.0174532925);
    x0 = sec_x*(CLOCK_RADIUS - 8)+DISPLAY_CENTER_X;
    yy0 = sec_y*(CLOCK_RADIUS -8)+DISPLAY_CENTER_Y;
    x1 = sec_x*CLOCK_RADIUS + DISPLAY_CENTER_X;
    yy1 = sec_y*CLOCK_RADIUS + DISPLAY_CENTER_Y;
    tft.drawLine(x0, yy0, x1, yy1, TFT_MEDGREY);
    // Draw main quadrant dots
  }

  // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sec_x = cos((i-90)*0.0174532925);
    sec_y = sin((i-90)*0.0174532925);
    x0 = sec_x*(CLOCK_RADIUS - 3)+DISPLAY_CENTER_X;
    yy0 = sec_y*(CLOCK_RADIUS - 3)+DISPLAY_CENTER_Y;
    // Draw minute markers
    tft.drawPixel(x0, yy0, TFT_WHITE);
    if(i==0 || i==180 || i==90 || i==270) tft.fillCircle(x0, yy0, 3, TFT_WHITE);
  }
  tft.fillCircle(DISPLAY_CENTER_X, DISPLAY_CENTER_Y, 3, TFT_WHITE);
  targetTime = millis() + 1000; 
  
  while (!encoder.getButtons()) {
    delay(10);
    getLocalTime(timeinfo);
    ss = timeinfo->tm_sec;
    if (ss_old != ss) {
      hh = timeinfo->tm_hour;
      mm = timeinfo->tm_min;  
      ss_old = ss;
      // Pre-compute hand degrees, x & y coords for a fast screen update
      sdeg = ss*6;                  // 0-59 -> 0-354
      mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
      hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
      hour_x = cos((hdeg-90)*0.0174532925);    
      hour_y = sin((hdeg-90)*0.0174532925);
      min_x = cos((mdeg-90)*0.0174532925);    
      min_y = sin((mdeg-90)*0.0174532925);
      sec_x = cos((sdeg-90)*0.0174532925);    
      sec_y = sin((sdeg-90)*0.0174532925);

      if (ss==0 || initial) {
        initial = 0;
        // Erase hour and minute hand positions every minute
        tft.drawLine(ohour_x, ohour_y, DISPLAY_CENTER_X, DISPLAY_CENTER_Y, TFT_BLACK);
        ohour_x = hour_x*HOUR_LEN+DISPLAY_CENTER_X;    
        ohour_y = hour_y*HOUR_LEN+DISPLAY_CENTER_Y;
        tft.drawLine(omin_x, omin_y, DISPLAY_CENTER_X, DISPLAY_CENTER_Y, TFT_BLACK);
        omin_x = min_x*MIN_LEN+DISPLAY_CENTER_X;    
        omin_y = min_y*MIN_LEN+DISPLAY_CENTER_Y;
      }
      // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
      tft.drawLine(osec_x, osec_y, DISPLAY_CENTER_X, DISPLAY_CENTER_Y, TFT_BLACK);
      osec_x = sec_x*SEC_LEN+DISPLAY_CENTER_X;    
      osec_y = sec_y*SEC_LEN+DISPLAY_CENTER_Y;
      tft.drawLine(osec_x, osec_y, DISPLAY_CENTER_X, DISPLAY_CENTER_Y, TFT_RED);
      tft.drawLine(ohour_x, ohour_y, DISPLAY_CENTER_X, DISPLAY_CENTER_Y, TFT_WHITE);
      tft.drawLine(omin_x, omin_y, DISPLAY_CENTER_X, DISPLAY_CENTER_Y, TFT_WHITE);
      tft.drawLine(osec_x, osec_y, DISPLAY_CENTER_X, DISPLAY_CENTER_Y, TFT_RED);
      tft.fillCircle(DISPLAY_CENTER_X, DISPLAY_CENTER_Y, 3, TFT_RED);
    }
  }
}


// #############################################################################
// MANDELBROT SCREENSAVER
// #############################################################################

// Based on sketch here:
// https://github.com/OpenHDZ/Arduino-experimentation

const uint16_t MAX_ITERATION = 300; // Nombre de couleurs

void drawJulia(float c_r, float c_i, float zoom) {
  tft.setCursor(0,0);
  float new_r = 0.0, new_i = 0.0, old_r = 0.0, old_i = 0.0;
  /* Pour chaque pixel en X */
  for(int16_t x = DISPLAY_W/2 - 1; x >= 0; x--) { // Rely on inverted symmetry
    /* Pour chaque pixel en Y */
    if (encoder.getButtons()) break;
    for(uint16_t y = 0; y < DISPLAY_H; y++) {      
      old_r = 1.5 * (x - DISPLAY_CENTER_X) / (0.5 * zoom * DISPLAY_W);
      old_i = (y - DISPLAY_CENTER_Y) / (0.5 * zoom * DISPLAY_H);
      uint16_t i = 0;
      while ((old_r * old_r + old_i * old_i) < 4.0 && i < MAX_ITERATION) {
        new_r = old_r * old_r - old_i * old_i ;
        new_i = 2.0 * old_r * old_i;
        old_r = new_r+c_r;
        old_i = new_i+c_i;        
        i++;
      }
      /* Affiche le pixel */
      if (i < 100) {
        tft.drawPixel(x,y,tft.color565(255,255,map(i,0,100,255,0)));
        tft.drawPixel(DISPLAY_W - x - 1,DISPLAY_H - y - 1,tft.color565(255,255,map(i,0,100,255,0)));
      } else if(i<200) {
        tft.drawPixel(x,y,tft.color565(255,map(i,100,200,255,0),0));
        tft.drawPixel(DISPLAY_W - x - 1,DISPLAY_H - y - 1,tft.color565(255,map(i,100,200,255,0),0));
      } else {
        tft.drawPixel(x,y,tft.color565(map(i,200,300,255,0),0,0));
        tft.drawPixel(DISPLAY_W - x - 1,DISPLAY_H - y - 1,tft.color565(map(i,200,300,255,0),0,0));
      }
    }
  }
}

void mandelbrot() {
  static float zoom = 1;
  while (!encoder.getButtons()) {
    drawJulia(-0.8,+0.1561, zoom);
    zoom *= 1.5;
    if (zoom > 100) zoom = 1;
  }
}

// #############################################################################
// GAME OF LIFE SCREENSAVER
// #############################################################################

#define CELLXY 3
#define GRIDX DISPLAY_W / CELLXY
#define GRIDY DISPLAY_H / CELLXY
#define GEN_DELAY 20
//Current grid
uint8_t grid[GRIDX][GRIDY];
//The new grid for the next generation
uint8_t newgrid[GRIDX][GRIDY];
//Number of generations
#define NUMGEN 600

//Initialise LIFE Grid
void initGrid(void) {
  for (int16_t x = 0; x < GRIDX; x++) {
    for (int16_t y = 0; y < GRIDY; y++) {
      newgrid[x][y] = 0;
      if (x == 0 || x == GRIDX - 1 || y == 0 || y == GRIDY - 1) {
        grid[x][y] = 0;
      } else {
        if (random(3) == 1)
          grid[x][y] = 1;
        else
          grid[x][y] = 0;
      }
    }
  }
}

// Check the Moore neighbourhood
int getNumberOfNeighbors(int x, int y) {
  return grid[x - 1][y] + grid[x - 1][y - 1] + grid[x][y - 1] + grid[x + 1][y - 1] + grid[x + 1][y] + grid[x + 1][y + 1] + grid[x][y + 1] + grid[x - 1][y + 1];
}

//Compute the CA. Basically everything related to CA starts here
void computeCA() {
  for (int16_t x = 1; x < GRIDX; x++) {
    for (int16_t y = 1; y < GRIDY; y++) {
      int neighbors = getNumberOfNeighbors(x, y);
      if (grid[x][y] == 1 && (neighbors == 2 || neighbors == 3 )) {
        newgrid[x][y] = 1;
      }
      else if (grid[x][y] == 1)  newgrid[x][y] = 0;
      if (grid[x][y] == 0 && (neighbors == 3)) {
        newgrid[x][y] = 1;
      }
      else if (grid[x][y] == 0) newgrid[x][y] = 0;
    }
  }
}


//Draws the grid on the display
void drawGrid(void) {
  uint16_t color = TFT_WHITE;
  for (int16_t x = 1; x < GRIDX - 1; x++) {
    for (int16_t y = 1; y < GRIDY - 1; y++) {
      if ((grid[x][y]) != (newgrid[x][y])) {
        if (newgrid[x][y] == 1) color = TFT_MEDGREY; //random(0xFFFF);
        else color = 0;
        tft.fillRect(CELLXY * x, CELLXY * y, CELLXY, CELLXY, color);
      }
    }
  }
}

void gameOfLife() {
  //Display a simple splash screen
  while (!encoder.getButtons()) {
    tft.fillScreen(TFT_BLACK);
    initGrid();
    drawGrid();
    //Compute generations
    for (int gen = 0; gen < NUMGEN; gen++) {
      computeCA();
      drawGrid();
      delay(GEN_DELAY);
      for (int16_t x = 1; x < GRIDX-1; x++) {
        for (int16_t y = 1; y < GRIDY-1; y++) {
          grid[x][y] = newgrid[x][y];
        }
      }
      if (encoder.getButtons()) break;
    }
  }
}

// #############################################################################
// STARFIELD SCREENSAVER
// #############################################################################



// With 1024 stars the update rate is ~65 frames per second
#define NSTARS 512

uint8_t sx[NSTARS] = {};
uint8_t sy[NSTARS] = {};
uint8_t sz[NSTARS] = {};

uint8_t za, zb, zc, zx;

// Fast 0-255 random number generator from http://eternityforest.com/Projects/rng.php:
uint8_t rng()
{
  zx++;
  za = (za^zc^zx);
  zb = (zb+za);
  zc = ((zc+(zb>>1))^za);
  return zc;
}

void drawPixelOutsideMenu(int16_t x, int16_t y, uint16_t color, bool checkMenuBoxes) {
  if (checkMenuBoxes) {
    if (x >= MENU_LEFT && x <= MENU_LEFT + MENU_W && y >= MENU_TOP && y <= MENU_TOP + MENU_LINE_H) {
      return; // Pixel liegt im Menübereich, nicht zeichnen
    }
    if (x >= MENU_LEFT && x <= MENU_LEFT + MENU_W && y >= SUBMENU_TOP && y <= SUBMENU_TOP + SUBMENU_H) {
      return; // Pixel liegt im Submenübereich, nicht zeichnen
    }
    if (x >= VALUEBOX_LEFT && x <= VALUEBOX_LEFT + VALUEBOX_W && y >= VALUEBOX_TOP && y <= VALUEBOX_TOP + VALUEBOX_H) {
      return; // Pixel liegt im Werteboxbereich, nicht zeichnen
    }
    if (x >= ORGANBOX_LEFT && x <= ORGANBOX_LEFT + ORGANBOX_W && y >= ORGANBOX_TOP && y <= ORGANBOX_TOP + ORGANBOX_H) {
      return; // Pixel liegt im Organboxbereich, nicht zeichnen
    }
  }
  tft.drawPixel(x, y, color);
}

void screenSaverInit() {
  za = random(256);
  zb = random(256);
  zc = random(256);
  zx = random(256);
  tft.fillScreen(TFT_BLACK);
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.setTextPadding(50);
  // tft.setTextFont(2);
  initGrid();
}

void starFieldUpdate(bool checkMenuBoxes) {
  uint8_t spawnDepthVariation = 255;
  for(int i = 0; i < NSTARS; ++i)  {
    if (sz[i] <= 1) {
      sx[i] = DISPLAY_CENTER_X - 120 + rng();
      sy[i] = DISPLAY_CENTER_Y - 120 + rng();
      sz[i] = spawnDepthVariation--;
    } else {
      int old_screen_x = ((int)sx[i] - DISPLAY_CENTER_X) * 256 / sz[i] + DISPLAY_CENTER_X;
      int old_screen_y = ((int)sy[i] - DISPLAY_CENTER_Y) * 256 / sz[i] + DISPLAY_CENTER_Y;
      // This is a faster pixel drawing function for occasions where many single pixels must be drawn
      drawPixelOutsideMenu(old_screen_x, old_screen_y, TFT_BLACK, checkMenuBoxes);
      sz[i] -= 2;
      if (sz[i] > 1) {
        int screen_x = ((int)sx[i] - DISPLAY_CENTER_X) * 256 / sz[i] + DISPLAY_CENTER_X;
        int screen_y = ((int)sy[i] - DISPLAY_CENTER_Y) * 256 / sz[i] + DISPLAY_CENTER_Y;
  
        if (screen_x >= 0 && screen_y >= 0 && screen_x < DISPLAY_W && screen_y < DISPLAY_H) {
          uint8_t r, g, b;
          r = g = b = 255 - sz[i];
          drawPixelOutsideMenu(screen_x, screen_y, tft.color565(r,g,b), checkMenuBoxes);
        } else
          sz[i] = 0; // Out of screen, die.
      }
    }
  }
}

void starField() {
  while (!encoder.getButtons()) {
    // unsigned long t0 = micros();
    starFieldUpdate(false);
    // unsigned long t1 = micros();
    // Calculate frames per second
    // tft.drawNumber(1.0/((t1 - t0)/1000000.0), 10, 10);
    delay(20);
  }
}
#endif