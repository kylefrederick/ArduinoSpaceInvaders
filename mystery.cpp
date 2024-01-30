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
#include "mystery.h"

void Mystery::init (SSD1306 &screen) {
  // Pick a randon direction
  movingRight = random (2);
  // Set the start position
  ship_x = movingRight ? 0 : 119;
  // Draw the ship on the screen
  screen.drawBitmap (BM_MYSTERY, ship_x, SHIP_Y);
}

boolean Mystery::exists () {
  return (ship_x < SHIP_HIT);
}

boolean Mystery::canCreateNew () {
  return (ship_x == NO_SHIP);
}

void Mystery::move (SSD1306 &screen) {
  // Clear the space where the ship was
  screen.clearRect (ship_x, SHIP_Y, 9, 5);
  // Move the ship 1 pixel in its chosen direction
  ship_x += movingRight ? 1 : -1;
  // If the ship hits the right edge of the screen (NOTE. Ships moving left will automatically hit the NO_SHIP value)
  if (ship_x > 119) {
    // The ship has gone
    ship_x = NO_SHIP;
  } else {
    // Draw the ship in its new position
    screen.drawBitmap (BM_MYSTERY, ship_x, SHIP_Y);
  }
}

uint16_t Mystery::collisionDetect (SSD1306 &screen, uint8_t x, uint8_t y) {
  // If the ship exists and the laser Y coordinate is in the mystery fly zone
  if (ship_x < SHIP_HIT && y < SHIP_Y + 5) {
    // If the laser X coordinate is wihin the mystery ship's position
    if (x >= ship_x && x <= ship_x + 9) {
      // Erase the ship
      screen.clearRect (ship_x, SHIP_Y, 9, 5);
      // Generate a random score between 50 and 300
      uint16_t score = random (1, 7) * 50;
      // Set the cursor to the ship's position
      screen.setCursor (ship_x + (score > 99 ? max (-ship_x, -2) : 0), SHIP_Y);
      // Put the score on the screen
      screen.writeInt (score);
      // Mark the ship as hit
      ship_x = SHIP_HIT;
      // Return the shore
      return (score);
    }
  }
  return (0);
}

boolean Mystery::wasHit () {
  return (ship_x == SHIP_HIT);
}

void Mystery::destroy () {
  ship_x = NO_SHIP;
}
