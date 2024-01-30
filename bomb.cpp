/*******************************************************************************
*                                                                              *
* Author: Nicholaus D. Cranch (N.I.X Consulting)                               *
* e-mail: info@nix-consulting.co.uk                                            *
*                                                                              *
* The following code may be used freely for personal, demonstration and        *
* teaching purposes only. Permission will be required for commercial use, as   *
* stipulated by GNU GPLv3.                                                     *
*                                                                              *
*******************************************************************************/
#include "bomb.h"
/*
 * There are two types of bomb. Slow and fast (wiggly).
 * Bomb objects are pre-declared. The create functions, simply initialise them
 * ready to be dropped.
 */

/*
 * Create a bomb of a random type at the coordinates given
 * Yeah... I know... reuse of code. Why didn't I just call the other function?
 */
void Bomb::create (SSD1306 &screen, uint8_t start_x, uint8_t start_y) {
  x = start_x;
  y = start_y;
  bombType = random (FAST_BOMB, SLOW_BOMB + 1);
  draw (screen);
}

/*
 * Create a bomb of a specified type at the coordinates given
 */
void Bomb::create (SSD1306 &screen, uint8_t start_x, uint8_t start_y, uint8_t bType) {
  x = start_x;
  y = start_y;
  bombType = bType;
  draw (screen);
}

/*
 * Draw the bomb
 */
boolean Bomb::draw (SSD1306 &screen) {
  if (bombType) {
    screen.drawBitmap (pgm_read_byte (&bombTable[bombType][y & 1]), x, y);
    return (true);
  }
  return (false);
}

/*
 * Move the bomb - erases the space where the bomb was, but does not re-draw
 * Returns true if a bomb was moved
 */
boolean Bomb::move (SSD1306 &screen) {
  switch (bombType) {
    case NO_BOMB:
      return (false);
    case SLOW_BOMB:
      if (delay) {
        delay = false;
        return (false);
      }
      delay = true;
  }
  // Clear the space where the bomb was
  screen.clearRect (x, y, 1 + bombType, 5);
  // Move the bomb down a pixel
  y ++;
  // Has it hit the bottom of the screen?
  if (y >= SSD1306_LCDHEIGHT- 4) {
    bombType = NO_BOMB;
  }
  return (true);
}

/*
 * Get coordinates
 * Note - For collision detection, we need the bottom of the bomb
 */
uint8_t Bomb::getX () {
  return (x);
}
uint8_t Bomb::getY () {
  return (y + 4);
}

/*
 * Retrieve the bomb's power
 */
uint8_t Bomb::getPower () {
  return ((bombType + 1) * 2 + 1);
}

/*
 * Does a bomb exists?
 * returns the bomb type / power
 */
boolean Bomb::exists () {
  return (bombType);
}

/*
 * Destroy the bomb
 * Returns the power of the bomb
 */
void Bomb::destroy () {
  bombType = 0;
}
