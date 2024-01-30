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
#ifndef base_h
#define base_h

#include "SSD1306.h"
#include "bitmaps.h"

#define BASE_Y 59
#define BASE_START_X 0
#define LASER_START 59
#define POWER_LASER 2 // The power value of a laser hit


class Base {
  public:
    // Positions the base at the left side of the screen
    void init ();
    // Moves the base if the navigation buttons are pressed - returns true it it moved
    boolean moveBase (SSD1306 &screen, boolean left, boolean right);
    // Fires a laser if the trigger is pressed and no laser already exists - returns true if a laser was released
    boolean fire (SSD1306 &screen, boolean trigger);
    // draws the base
    void drawBase (SSD1306&);
    // clears the area where the base currently resides
    void clearBase (SSD1306&);
    // perform a collision detection at the coordinates provided - returns true if hit
    boolean collisionDetect (SSD1306 &screen, uint8_t x, uint8_t y);
    // Check if the bas is dead
    boolean isDead ();
    // Laser stuff
    
    // Moves the laser if one exists - returns true if the laser was moved
    boolean moveLaser (SSD1306&);
    // Clears the space where the laser currently resides
    void clearLaser (SSD1306&);
    // Get laser Y - if 0, no laser exists
    uint8_t getLaserY ();
    // Get missule X - will get the last X value regardless of whether a laser exists or not
    uint8_t getLaserX ();
    // Draw the laser - return true if the laser was drawn
    boolean drawLaser (SSD1306&);
    // Destroy the base
    boolean destroyBase (SSD1306 &screen);
    // Removes the laser
    void destroyLaser ();
    uint8_t base_x;
    
  private:
//    uint8_t base_x;
    uint8_t laser_x;
    uint8_t laser_y; // Zero if no laser is currently active
    boolean fireButtonReleased;
    boolean dead;
};
#endif
