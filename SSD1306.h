/*
 *  Adafruit SSD1306 library modified by William Greiman for
 *  unbuffered LiquidCrystal character mode.
 *
 * -- Further modified by JBoyton to support character scaling
 * and to allow horizontal positioning of text to any pixel.
 * Vertical text position is still limited to a row.
 * Edited to make specific to Uno R3 and 128x64 size.
 * Also added H/W SPI and I2C support to the existing soft SPI.
 * 
 * -- Further modified by N. Cranch specifically for the Space
 * Invaders game.
 * Remove the soft SPI
 * Whack up the I2C speed
 * Add a screen buffer 
 * Allow the absolute positioning of bitmaps
 * Replace the 5x7 font with an even smaller 5 pixel high font
 */
#ifndef SSD1306_h
#define SSD1306_h 
#include "Arduino.h"

#define SSD1306_LCDWIDTH  128
#define SSD1306_LCDHEIGHT  64
#define SSD1306_PAGES (SSD1306_LCDHEIGHT / 8)

#define SSD1306_I2C_ADDRESS   0x3C  // 011110+SA0+RW - 0x3C or 0x3D

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_SETSTARTPAGE 0XB0
#define SSD1306_MEMORYMODE 0x20

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

class SSD1306 {

 public:
  SSD1306(int8_t rst) : rst_(rst) {}

  void init();
  void clear();
  void update ();
  void setCursor(uint8_t row, uint8_t col);
  size_t write(uint8_t c);
  size_t write(const char* s);
  size_t write(const __FlashStringHelper* s);
  void writeInt(uint16_t i);
  void writeScore (uint16_t);
  uint8_t charWidth (uint8_t c);

  void sendCommand(uint8_t c);
  void sendData(uint8_t c);
  void sendFromBuffer (uint8_t *, int, int);
  void drawBitmap (uint16_t, uint8_t, uint8_t);
  void clearRect (uint8_t, uint8_t, uint8_t, uint8_t);
  boolean readPixel (uint8_t x, uint8_t y);
  void clearPixel (uint8_t, uint8_t);
  void dumpBuffer ();

 private:
  void updatePage (int);
  void setUpdateArea (uint8_t, uint8_t, uint8_t);
  
  int8_t cursor_x, cursor_y;    // cursor position
  int8_t data_, clk_, dc_, rst_, cs_; // OLED pins

  volatile uint8_t *mosiport, *clkport, *csport, *dcport;
  uint8_t mosipinmask, clkpinmask, cspinmask, dcpinmask;
  
  void spiWrite(uint8_t c);
  uint8_t buffer[SSD1306_PAGES][SSD1306_LCDWIDTH];   // screen buffer
  uint8_t updateArea [SSD1306_PAGES][2]; // beginning and end positions of screen page updates areas
};
#endif
