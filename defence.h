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
#ifndef defence_h
#define defence_h

#include "SSD1306.h"

#define DEFENCE_TOP 54
#define DEFENCE_BOTTOM (DEFENCE_TOP + 5)
#define DEFENCE_WIDTH 14

class Defence {
  public:
    void init (SSD1306&, uint8_t); // Set the x coord and draw the defence
    boolean collisionDetect (SSD1306&, uint8_t, uint8_t, uint8_t, uint8_t); // x and y coords, impact strength, bottom of alien grid

  private:
    boolean withinBounds (uint8_t, uint8_t, uint8_t);
    uint8_t defence_x;
};
#endif
