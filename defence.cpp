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
/*
 * The defences in Space Invaders seems to be a bit strange.
 * It is unclear as to whether damage occuring on a defence is handled by a
 * selection of predefined bitmaps, or whether each defence is kept as a
 * separate bitmap.
 * In either scenario, the contraints of the Arduino architecture are
 * limiting factors.
 * The Arduino has insufficient ROM space for all possible damage scenarios
 * and has insufficient RAM to make a separate bitmap for each base as well
 * as the screen buffer.
 * As such, I use the screen buffer itself for collision detection.
 * This does however, cause an issue when the aliens descend below the top
 * of the defences.
 * The solution to this is to tell the defences to ignore the pixels above
 * the bottom of the alien grid.
 */
#include "defence.h"
#include "bitmaps.h"

/*
 * Set up the defence as the new undamaged bitmap
 */
void Defence::init (SSD1306 &screen, uint8_t x) {
  defence_x = x;
  screen.drawBitmap (BM_DEFENCE, x, DEFENCE_TOP);
}

/*
 * Check the provided coordinates for a collision
 * The alien y argument provides the bottom y coordinate of the alien grid.
 * It is assumed that defences have been erased by the aliens at this point.
 * This is to stop accidental misidentification of defence pixels and alien
 * pixels when the aliens reach the defences. This will however, produce the
 * occasional glitch where lasers and bombs may pass directly through the
 * remnants of a defence without detecting a collsion. It was deemed that
 * the rarity of such an event and that of the player noticing would make it
 * insignificant. Plus, if the aliens get down that far, you're probably
 * screwed anyway ;-).
 * The power argument provides an upper limit to the damage created if a
 * collision is detected.
 */
boolean Defence::collisionDetect (SSD1306 &screen, uint8_t x, uint8_t y, uint8_t alien_y, uint8_t power) {
  int boom_x, boom_y;
  if (withinBounds (x, y, alien_y)) {
    // Now we need to see if there's a pixel at the coordinate
    if (screen.readPixel (x, y)) {
      // Collision detected, now blow up some of the defence
      int pixels = 4 + random (power * 3);
      for (int i = 0; i < pixels; i ++) {
         boom_x = x + ((random (-power, power) + random (-power, power)) / 2);
         boom_y = y + ((random (-power, power) + random (-power, power)) / 2);
         if (withinBounds (boom_x, boom_y, alien_y)) {          
           screen.clearPixel (boom_x, boom_y);
         }
      }
      return (true);
    }
  }
  return (false);
}

/*
 * Check to see if a pixel is within the defence bounds
 */
boolean Defence::withinBounds (uint8_t x, uint8_t y, uint8_t alien_y) {
  // Firstly, is the y coord withing range?
  if (y >= DEFENCE_TOP && y < DEFENCE_BOTTOM) {
    // And check the x coord
    if (x >= defence_x && x <= (defence_x + DEFENCE_WIDTH)) {
      // Just check if it's below the bottom of the aliens (we don't want to detect alien pixels)
      return (y > alien_y);
    }
  }
  return (false);
}

