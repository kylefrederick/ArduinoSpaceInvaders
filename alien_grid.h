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
#ifndef alien_grid_h
#define alien_grid_h
#include "SSD1306.h"
#include "bitmaps.h"
#include <Arduino.h>

#define AG_COLWIDTH 9       // The alien grid column width in pixels
#define AG_ROWHEIGHT 6      // ... row height in pixels
#define AG_ROWS 5           // The initial number of rows
#define AG_COLS 11          // ... and columns
#define AG_START_X 9        // starting x coordinate 

class AlienGrid {
  public:
    void init (uint8_t start_y);        // Initialise the grid
    void draw (SSD1306 &screen);        // Draw the aliens into the screen buffer
    void step (SSD1306 &screen);        // Move the grid one step left or right
    int collisionDetect (SSD1306 &screen, uint8_t x, uint8_t y);    // Test the provided coordinate for collisions
    uint8_t getTop ();                  // Get the top coordinate of the grid
    uint8_t getBottom ();               // ... the bottom coordinate
    uint8_t getLeft ();                 // ... the left coordinate
    uint8_t getRight ();                // ... and the right
    void clearArea (SSD1306 &screen);   // clear the grid area in the screen buffer
    void clearExplosion ();             // clear the explosion graphic from the screen buffer
    uint8_t getAlienCount ();           // count the remaining aliens
    uint8_t getCols ();                 // get the number of columns of aliens remaining
    uint8_t getColX (uint8_t);          // get the x coordinate of the specified column
    uint8_t getColY (uint8_t);          // ... and the y coordinate
    uint8_t getRandomColumn ();         // Choose a random column from those remaining
    boolean explosionPresent ();        // Check if the explosion graphic is still displayed
    
  private:
    void normaliseGrid ();              // Shift the alien grid bits to their left most (least significant) position
    uint16_t grid[AG_ROWS];
    uint8_t grid_x; 
    uint8_t grid_y;
    uint8_t cols;              
    uint8_t rows;
    boolean movingRight;                // True if the grid is moving right, false = left
    boolean removeExplosion;            // A flag used to decide if an explosion needs to be removed
    int8_t bang_col;                    // Explosion grid column number (-1 if no explosion is present)
    int8_t bang_row;                    // ... row
    uint8_t alienCount;
};

/*
 * An array to hold the pointers to the bitmaps
 * There are 5 rows, corresponding to the rows in the grid. If you wish to
 * increase the number of rows, this array will have to be extended.
 * The two columns in the array correspond to the bitmaps used as the aliens
 * "walk" across the screen.
 */
const uint8_t alienBitmap[][2] PROGMEM = {
  { BM_ALIEN30_1, BM_ALIEN30_2 },
  { BM_ALIEN20_1, BM_ALIEN20_2 },
  { BM_ALIEN20_1, BM_ALIEN20_2 },
  { BM_ALIEN10_1, BM_ALIEN10_2 },
  { BM_ALIEN10_1, BM_ALIEN10_2 }
};

// The score values of each row of aliens (score values are in multiples of 10)
const uint8_t alienScore[] PROGMEM = { 3, 2, 2, 1, 1 };

// For collision detection (aliens get thinner as you go up the grid)
const uint8_t alienBounds[][2] PROGMEM = {
  { 1, 5 }, { 0, 6 }, { 0, 6 }, { 0, 7 }, { 0, 7 }
};
#endif
