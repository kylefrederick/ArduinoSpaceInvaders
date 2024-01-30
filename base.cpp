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
#include "base.h"

/*
 * Set the base to its starting position
 */
void Base::init () {
  base_x = BASE_START_X;
  dead = false;
}

/*
 * Move the base if either the left or right button is pressed
 */
boolean Base::moveBase (SSD1306 &screen, boolean left, boolean right) {
  if (!dead) {
    uint8_t pos = base_x;
    if (left && pos > 0) pos --;
    if (right && pos < 119) pos ++;
    if (pos != base_x) {
      screen.clearRect (base_x, BASE_Y, 9, 5);
      base_x = pos;
      drawBase (screen);
      return (true);
    }
  }
  return (false);
}

/*
 * Destroy the base - the base has been hit
 * Returns true is the base was destroyed (ie. was alive when hit)
 */
boolean Base::destroyBase (SSD1306 &screen) {
  if (!dead) {
    dead = true;
    clearBase (screen);
    drawBase (screen);
    return (true);
  }
  return (false);
}

void Base::clearBase (SSD1306 &screen) {
  screen.clearRect (base_x, BASE_Y, 9, 5);
}

/*
 * Draw the base
 */
void Base::drawBase (SSD1306 &screen) {
  if (dead) {
    screen.drawBitmap (BM_BASE_DEAD, base_x, BASE_Y);    
  } else {
    screen.drawBitmap (BM_BASE, base_x, BASE_Y);
  }
}

/*
 * Fire a laser from the bas if the fire button has been pressed
 * Only allows one laser to be active at any one time
 * Returns true if a laser was fired
 */
boolean Base::fire (SSD1306 &screen, boolean trigger) {
  // was the trigger pressed
  if (trigger) {
    // Is there no laser currely active and the bas is not dead?
    if (laser_y == 0 && !dead) {
      // Check if the fire button has been released
      if (fireButtonReleased) {
        laser_y = LASER_START;
        laser_x = base_x + 4;
        fireButtonReleased = false;
        drawLaser (screen);
        return (true);
      }
    }
  } else {
    if (laser_y == 0) {
      fireButtonReleased = true;
    }
  }
  return (false);
}

/*
 * Moves a laser, if one exists
 * Returns true if a laser was moved
 * 
 * Note - this function will clear the space left by a laser moving, but will not redraw it.
 */
boolean Base::moveLaser (SSD1306 &screen) {
  // If a laser exists
  if (laser_y) {
    // Clear the screen where it was
    screen.clearRect (laser_x, laser_y, 1, min (4, 59 - laser_y));
    // Move the laser up one pixel
    laser_y --;
    // Did the laser travel off the top of the screen?
    if (laser_y <= 5) {
      // Yes, delete the laser
      laser_y = 0;
    }
    return (true);
  }
  return (false);
}

/*
 * Perform collision detection
 */
boolean Base::collisionDetect (SSD1306 &screen, uint8_t x, uint8_t y) {
  if (y > (BASE_Y + 2)) {
    if (x >= base_x && x <= (base_x + 9)) {
      // Boom!
      return (destroyBase (screen));
    }
  }
  return (false);
}

/*
 * Draws the laser if one exists
 * Returns true if there was a laser drawn
 */
boolean Base::drawLaser (SSD1306 &screen) {
  if (laser_y) {
    screen.drawBitmap (BM_LASER, laser_x, laser_y);
    return (true);
  }
  return (false);
}

/*
 * The laser has hit something
 */
void Base::destroyLaser () {
  laser_y = 0;
}

/*
 * Get laser coordinates
 */
uint8_t Base::getLaserY () {
  return (laser_y);
}
uint8_t Base::getLaserX () {
  return (laser_x);
}

/*
 * Check if dead
 */
boolean Base::isDead () {
  return (dead);
}
