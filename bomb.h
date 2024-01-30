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
#ifndef bomb_h
#define bomb_h

#include "SSD1306.h"
#include "bitmaps.h"

#define NO_BOMB 0
#define FAST_BOMB 1
#define SLOW_BOMB 2

class Bomb {
  public:
    void create (SSD1306 &screen, uint8_t start_x, uint8_t start_y);
    void create (SSD1306 &screen, uint8_t start_x, uint8_t start_y, uint8_t bType);
    boolean move (SSD1306 &screen);
    uint8_t getX ();
    uint8_t getY ();
    void destroy ();
    boolean draw (SSD1306 &screen);
    boolean exists ();
    uint8_t getPower ();
    
  private:
    uint8_t bombType; // 0 - no bomb, 1 - fast wiggly, 2 - slow big
    uint8_t x;
    uint8_t y;
    boolean delay; // delay toggle switch for slow bombs
};

#endif
