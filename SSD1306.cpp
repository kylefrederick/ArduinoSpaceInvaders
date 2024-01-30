/*  Adafruit SSD1306 library modified by William Greiman for
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
#include <avr/pgmspace.h>
#include "SSD1306.h"
#include "bitmaps.h"
#include <SPI.h>
#include <Wire.h>

/*
 * Initialise the display
 */
void SSD1306::init() {
  cursor_x = 0;
  cursor_y = 0;

  // set pin directions
  if (rst_ != -1) pinMode(rst_, OUTPUT);

  // Set up I2C
  Wire.begin();
  Wire.setClock (800000); // Super fast 800 KHz

  // Reset
  if (rst_ > 0) {
    digitalWrite(rst_, HIGH);
    delay(1);
    digitalWrite(rst_, LOW);
    delay(10);
    digitalWrite(rst_, HIGH);
  }

  // Init sequence for 128x64 OLED module
  sendCommand(SSD1306_DISPLAYOFF);          // 0xAE
  sendCommand(SSD1306_SETDISPLAYCLOCKDIV);  // 0xD5
  sendCommand(0x80);                        // the suggested ratio 0x80
  sendCommand(SSD1306_SETMULTIPLEX);        // 0xA8
  sendCommand(0x3F);
  sendCommand(SSD1306_SETDISPLAYOFFSET);    // 0xD3
  sendCommand(0x0);                         // no offset
  sendCommand(SSD1306_SETSTARTLINE | 0x0);  // line #0
  sendCommand(SSD1306_CHARGEPUMP);          // 0x8D
  sendCommand(0x14);
  sendCommand(SSD1306_MEMORYMODE);          // 0x20
  sendCommand(0x00);        // was: 0x2 page mode
  sendCommand(SSD1306_SEGREMAP | 0x1);
  sendCommand(SSD1306_COMSCANDEC);
  sendCommand(SSD1306_SETCOMPINS);          // 0xDA
  sendCommand(0x12);
  sendCommand(SSD1306_SETCONTRAST);         // 0x81
  sendCommand(0xCF);
  sendCommand(SSD1306_SETPRECHARGE);        // 0xd9
  sendCommand(0xF1);
  sendCommand(SSD1306_SETVCOMDETECT);       // 0xDB
  sendCommand(0x40);
  sendCommand(SSD1306_DISPLAYALLON_RESUME); // 0xA4
  sendCommand(SSD1306_NORMALDISPLAY);       // 0xA6
  
  sendCommand(SSD1306_DISPLAYON);//--turn on oled panel
}


/* 
 * Clear the screen
 */
void SSD1306::clear() {
  sendCommand(SSD1306_COLUMNADDR);
  sendCommand(0);   // Column start address (0 = reset)
  sendCommand(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

  sendCommand(SSD1306_PAGEADDR);
  sendCommand(0); // Page start address (0 = reset)
  sendCommand(7); // Page end address

  // Clear all but the last byte
  for (int i = 0; i < (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8 / 16); i++) {
    Wire.beginTransmission(SSD1306_I2C_ADDRESS);
    // I2C buffer size is 32 bytes
    for (int j = 0; j < 15; j ++) {
      Wire.write(0xC0);
      Wire.write(0x00);
    }
    Wire.write(0x40);
    Wire.write(0x00);
    Wire.endTransmission();
  }
  sendCommand(SSD1306_COLUMNADDR);
  sendCommand(0);   // Column start address (0 = reset)
  sendCommand(SSD1306_LCDWIDTH-1); // Column end address (127 = reset)

  sendCommand(SSD1306_PAGEADDR);
  sendCommand(0); // Page start address (0 = reset)
  sendCommand(7); // Page end address
  // Clear the buffer and update area
  memset (buffer, 0, sizeof(buffer));
  memset (updateArea, 0, sizeof(updateArea));
  cursor_x = cursor_y = 0;
}

/*
 * Update the screen, only sending the changes
 */

void SSD1306::update () {
  for (int i = 0; i < SSD1306_PAGES; i ++) {
    updatePage (i);
  }
}

/*
 * Update a single page, if required
 */
void SSD1306::updatePage (int page) {
  if (updateArea[page][1] + updateArea[page][0] != 0) {
    // Set display cursor address
    sendCommand(SSD1306_SETLOWCOLUMN | (updateArea[page][0] & 0XF));
    sendCommand(SSD1306_SETHIGHCOLUMN | (updateArea[page][0] >> 4));
    sendCommand(SSD1306_SETSTARTPAGE | page);
    sendFromBuffer ((uint8_t*)buffer[page], updateArea[page][0], updateArea[page][1]);
    updateArea[page][0] = updateArea[page][1] = 0;
  }
}

/*
 * Set the cursor position from the top left corner = 0, 0
 */
void SSD1306::setCursor(uint8_t x, uint8_t y) {
  if (y >= SSD1306_LCDHEIGHT) {
    y = SSD1306_LCDHEIGHT - 1;
  }
  if (x >= SSD1306_LCDWIDTH) {
    x = SSD1306_LCDWIDTH - 1;
  }
  cursor_y = y; // row is 1 pixels tall
  cursor_x = x; // col is 1 pixel wide
}
//------------------------------------------------------------------------------
/*
 * Write a single character to the display
 */
size_t SSD1306::write(uint8_t c) {
  if (cursor_x >= SSD1306_LCDWIDTH) return 0;
  // If printable
  if (c >= 32 && c <= 126) {
    // Pull the bitmap offset from the character table
    uint8_t o = pgm_read_byte (charTable + c - 32);
    // is it a "space" (0xFF)
    if (o != 0xFF) {
      // No, display it
      drawBitmap (o, cursor_x, cursor_y);
    }
  }
  // Move the cursor
  cursor_x += charWidth (c);
  if (cursor_x > SSD1306_LCDWIDTH) {
    cursor_x = cursor_x % SSD1306_LCDWIDTH;
    cursor_y += 6;
    if (cursor_y > SSD1306_LCDHEIGHT) {
      cursor_y = cursor_y % SSD1306_LCDHEIGHT;
    }
  }
  return 1;
}

/*
 * Although it's a variable width charset, only M, N & W are not 3 pixels wide
 * So rather than use a width table, use this ugly hack ;-) 
 */
uint8_t SSD1306::charWidth (uint8_t c) {
  switch (c) {
    case 'M':
    case 'W':
      return (6);
    case 'N':
    case '?':
      return (5);
  }
  return (4);
}

//------------------------------------------------------------------------------
/*
 * Write a string to the display
 */
size_t SSD1306::write(const char* s) {
  size_t n = strlen(s);
  for (size_t i = 0; i < n; i++) {
    write(s[i]);
  }
  return n;
}

size_t SSD1306::write(const __FlashStringHelper* s) {
  size_t n = 0;
  uint8_t c;
  while (true) {
    c = pgm_read_byte ((char *)s + n);
    if (c) {
      write (c);
      n ++;
    } else {
      return (n);
    }
  }
}

//------------------------------------------------------------------------------
/*
 * write an integer to the display
 */
void SSD1306::writeInt(uint16_t i) {  // slightly smaller than system print()
    char buffer[7];
    itoa(i, buffer, 10);
    write(buffer);
}

/*
 * Write the score to the display (zero padded with trailing zero
 */
void SSD1306::writeScore (uint16_t s) {
  if (s < 1000) write ('0');
  if (s < 100) write ('0');
  if (s < 10) write ('0');
  writeInt (s);
  write ('0');
}
/*
 * Send a command to the display
 */
void SSD1306::sendCommand(uint8_t c) {
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    Wire.beginTransmission(SSD1306_I2C_ADDRESS);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
}
//------------------------------------------------------------------------------
/*
 * Send bitmap data to the display
 */
void SSD1306::sendData(uint8_t c) {
    uint8_t control = 0x40;   // Co = 0, D/C = 1
    Wire.beginTransmission(SSD1306_I2C_ADDRESS);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
}

/*
 * Send bitmap data to the display from a buffer
 */
void SSD1306::sendFromBuffer (uint8_t* addr, int start, int stop) {
  int i2cCount = 0;
  stop --; // Decrement the count to speed up last byte detection
  Wire.beginTransmission(SSD1306_I2C_ADDRESS);
  for (int i = start; i <= stop; i ++) { 
    // If we're at the end of the bitmap, flag end of data
    if (i == stop) {
      // End of bitmap, send end of data flag
      Wire.write (0x40);
      Wire.write (addr[i]);
    } else {
      // Are we at the end of the I2C buffer?
      if (i2cCount == 15) {
        // send end of data flag
        Wire.write (0x40);
        Wire.write (addr[i]);
        // End the transmission
        Wire.endTransmission ();
        // And start a new one
        Wire.beginTransmission (SSD1306_I2C_ADDRESS);
        // Reset i2c buffer count
        i2cCount = 0;
      } else {
        Wire.write (0xC0);
        Wire.write (addr[i]);
        // increment i2c buffer count
        i2cCount ++;
      }
    }
  }
  Wire.endTransmission();
}

/*
 * Copy a bitmap from the bitmaps stored in progmem into the screen buffer
 * at the coordinate provided. Bitmaps are overlaid, leaving any pixels previous
 * set.
 * All bitmaps are 5 pixels high and are terminated when the MSB is set.
 */
void SSD1306::drawBitmap (uint16_t bitmapOffset, uint8_t x, uint8_t y) {
  uint8_t page1 = y / 8;
  uint8_t page2 = page1 + 1;
  uint8_t shift1 = y % 8;
  uint8_t shift2 = 8 - shift1;
  uint8_t b1, b2;
  uint8_t* pointer = bitmaps + bitmapOffset;
  boolean notFinished = true;
  uint8_t x2 = x;  // take a copy of x

  do {
    // Get a slice of the bitmap and increment the pointer
    b1 = b2 = pgm_read_byte (pointer ++);
    // Is the end marker set?
    if (b1 >= 128) {
      // Strip the marker
      b1 = b2 = b1 & B00011111;
      // and set the escape flag
      notFinished = false;
    }
    // Shift the bitmap down if necessary
    b1 = b1 << shift1;
    // put it into the buffer
    buffer[page1][x2] = b1 | buffer[page1][x2];
    // Did we shift more than 3? - if so, we need to update the next page too
    if (shift1 > 3) {
      // shift the 2nd copy of the slice the opposite way
      b2 = b2 >> shift2;
      // put it in the buffer
      buffer[page2][x2] = b2 | buffer[page2][x2];
    }
    x2 ++;
  } while (notFinished);
  setUpdateArea (page1, x, x2);
  if (shift1 > 3) {
    setUpdateArea (page2, x, x2);
  }
}

/*
 * Clear rectangle in the buffer
 */
void SSD1306::clearRect (uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  uint8_t y2 = y + h;
  uint8_t shift1 = y % 8;
  uint8_t shift2 = y2 % 8;
  uint8_t page1 = y / 8;
  uint8_t page2 = y2 / 8;
  uint8_t mask1 = ~(0xFF << shift1);
  uint8_t mask2 = ~(0xFF >> (8 - shift2));
  uint8_t x2 = x + w;
  if (page1 == page2) {
    mask1 = mask1 | mask2;
  }
  for (uint8_t i = x; i < x2; i ++) {
    buffer[page1][i] = buffer[page1][i] & mask1;
  }
  setUpdateArea (page1, x, x2);  
  if (page1 == page2) return;
  uint8_t p2m1 = page2 - 1;
  if (p2m1 != page1) {
    for (uint8_t i = page1 + 1; i < page2; i ++) {
      for (uint8_t j = x; j < x2; j ++) {
        buffer[i][j] = 0;
      }
      setUpdateArea (i, x, x2);
    }
  }
  for (uint8_t i = x; i < x2; i ++) {
    buffer[page2][i] = buffer[page2][i] & mask2;
  }
  setUpdateArea (page2, x, x2);
}

/*
 * Dump the contents of the buffer to the serial port
 */
void SSD1306::dumpBuffer () {
  for (int i = 0; i < SSD1306_PAGES; i ++) {
    for (uint8_t b = 0; b < 8; b ++) {
      for (int j = 0; j < SSD1306_LCDWIDTH; j ++) {
        Serial.print ((buffer[i][j] & (1 << b)) ? "#" : " ");
      }
      Serial.println ();
    }
  }
}

void SSD1306::setUpdateArea (uint8_t page, uint8_t x1, uint8_t x2) {
  if ((updateArea[page][0] + updateArea[page][1]) == 0) {
    updateArea[page][0] = x1;
  } else {
    updateArea[page][0] = min (updateArea[page][0], x1);
  }
  updateArea[page][1] = max (updateArea[page][1], x2);
}

/*
 * returns true if the pixel is set, otherwise, false
 */
boolean SSD1306::readPixel (uint8_t x, uint8_t y) {
  return ((1 << (y % 8)) & buffer[y / 8][x]);
}

/*
 * Clear pixel
 */
void SSD1306::clearPixel (uint8_t x, uint8_t y) {
  uint8_t page = y / 8;
  buffer[page][x] = buffer[page][x] & ~(1 << (y % 8));
  setUpdateArea (page, x, x);
}
