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
#ifndef mystery_y
#define mystery_y

#include "SSD1306.h"
#include "bitmaps.h"

#define SHIP_Y 6
#define NO_SHIP 255
#define SHIP_HIT 254

class Mystery {
  public:
    void init (SSD1306 &screen);
    void move (SSD1306 &screen);
    uint16_t collisionDetect (SSD1306 &screen, uint8_t x, uint8_t y);
    boolean exists ();
    boolean wasHit ();
    void destroy ();
    boolean canCreateNew ();
    
  private:
    uint8_t ship_x = NO_SHIP;
    boolean movingRight;
};

#endif
