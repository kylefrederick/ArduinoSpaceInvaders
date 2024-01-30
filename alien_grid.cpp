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
 * Code responsible for maintaining the alien grid
 *
 * The alien grid is held as 5 x 16 bit integers, used as bitfields.
 * Out of the 16 bits, only 11 are used because we only have 11 columns
 * of aliens. These are held in the right most least significant bits
 * and each bit represents the presence of an alien going backwards
 * from left to right. (ie. the LSB in the bitfield is the left most
 * alien on screen).
 * When an alien is killed, the corresponding bit is cleared.
 * When all least significant bits from all rows are clear, the bitfields
 * are normalised by rotating right, so that the grid always starts at
 * bit 0.
 * Exploding aliens are also controlled here. When an alien is killed
 * it's row and column are stored, so that when the screen is updated
 * it knows where to put the explosion. This is erased on the next
 * movement step. 
 */
#include "alien_grid.h"

/*
 * Set up the alien grid
 */
void AlienGrid::init (uint8_t start_y) {
  grid[0] = grid[1] = grid[2] = grid[3] = grid[4] = 0x07FF;
  grid_x = AG_START_X;
  grid_y = start_y;
  movingRight = true;
  cols = AG_COLS;
  rows = AG_ROWS;
  bang_col = -1;
  alienCount = (AG_ROWS * AG_COLS);
}

/*
 * Draw the aliens and explosion, is there is one
 */
void AlienGrid::draw (SSD1306 &screen) {
  uint8_t x = grid_x + 1; // First row of aliens needs to be offset by 1 pixel
  uint8_t y = grid_y;
  uint16_t mask;
  uint8_t bm;
  for (uint8_t i = 0; i < rows; i ++) {
    mask = 1;
    bm = pgm_read_byte (&(alienBitmap[i][x & 1]));
    for (uint8_t j = 0; j < cols; j ++) {
      // Does an alien exist at this location?
      if (grid[i] & mask) {
        screen.drawBitmap (bm, x, y);
      }
      x += AG_COLWIDTH;
      mask = mask << 1;
    }
    y += AG_ROWHEIGHT;
    x = grid_x;
  }
  // Draw the explosion if there is one
  if (bang_col != -1) {
    screen.drawBitmap (BM_EXPLOSION, grid_x + (bang_col * AG_COLWIDTH), grid_y + (bang_row * AG_ROWHEIGHT));
  }
}

/*
 * Flag the explosion for removal on next step
 */
void AlienGrid::clearExplosion () {
  removeExplosion = true;
}

/*
 * Check it there is an explosion being displayed
 */
boolean AlienGrid::explosionPresent () {
  return (bang_col != -1);
}

/*
 * Clear the alien grid area, move the aliens one step and then re-draw
 */
void AlienGrid::step (SSD1306 &screen) {
  clearArea (screen);
  if (alienCount) {
    if (removeExplosion) {
      normaliseGrid ();
      bang_col = -1;
      removeExplosion = false;
    }
    if (movingRight) {
      if ((grid_x + (cols * AG_COLWIDTH)) >= 128) {
        movingRight = false;
        grid_y += 3;
      }
    } else {
      if (grid_x <= 0) {
        movingRight = true;
        grid_y += 3;
      }
    }
    if (movingRight) {
      grid_x ++;
    } else {
      grid_x --;
    }
  }
  draw (screen);
}

/*
 * Clear the alien grid area
 */
void AlienGrid::clearArea (SSD1306 &screen) {
  screen.clearRect (grid_x, grid_y, cols * AG_COLWIDTH, rows * AG_ROWHEIGHT);
}

/*
 * Perform a collision detection with the aliens
 * If an alien is hit, it is removed from the grid and explosion put in its place
 * Returns - the number of points for the alien hit, 0 if none
 */
int AlienGrid::collisionDetect (SSD1306 &screen, uint8_t x, uint8_t y) {
  // Is the laser within the alien grid?
  if (x >= grid_x && x < getRight () && y >= grid_y && y < getBottom ()) {
    // Which row?
    uint8_t row_y = grid_y;
    for (uint8_t i = 0; i < rows; i ++) {
      row_y += AG_ROWHEIGHT;
      if (y < row_y) { // by check for < rather than <=, we can avoid the gaps between rows
        // Now check the column
        uint8_t tmp_x = (x - grid_x) % 9;
        if (tmp_x >= pgm_read_byte (&(alienBounds[i][0])) && tmp_x <= pgm_read_byte (&(alienBounds[i][1]))) {
          // Not in the gap, so check if the alien exists
          tmp_x = (x - grid_x) / 9;
          if (grid[i] & (1 << tmp_x)) {
            // Oooh, we hit one - delete it from the grid
            grid[i] = grid[i] & ~(1 << tmp_x);
            alienCount --;
            // If there is already an explosion present
            if (bang_col != -1) {
              // Remove it
              screen.clearRect (grid_x + (bang_col * AG_COLWIDTH), grid_y + (bang_row * AG_ROWHEIGHT), AG_COLWIDTH, AG_ROWHEIGHT);
            }
            // Set the location of the explosion
            bang_col = tmp_x;
            bang_row = i;
            return (pgm_read_byte (&(alienScore[i])));
          }
        }
      }
    }
  }
  return (0);
}

/*
 * Rearranges the grid if necessary, after a hit.
 * The grid gets smaller as aliens are removed.
 */
void AlienGrid::normaliseGrid () {
  // Check to see if the grid has gotten smaller
  // Was the hit on the bottom row?
  if (bang_row == (rows - 1)) {
    // Check to see if the row is now empty.... and subsequent rows
    while (rows) {
      if (grid[rows - 1] == 0) {
        rows --; 
      } else {
        break;
      }
    }
  }
  // Was the alien in the left most column?
  if (bang_col == 0) {
    // If so, check for empty columns on the left
    uint16_t allCols = grid[0] | grid[1] | grid[2] | grid[3] | grid[4];
    uint8_t shift = 0;
    while ((allCols & 1) == 0) {
      shift ++;
      allCols = allCols >> 1;
      grid_x += AG_COLWIDTH;
    }
    if (shift) {
      for (int j = 0; j < rows; j ++) {
        grid[j] = grid[j] >> shift;
      }
      cols -= shift;
      bang_col -= shift;
    }
  } else if (bang_col == (cols - 1)) {
    // check for empty columns on the right
    uint16_t allCols = grid[0] | grid[1] | grid[2] | grid[3] | grid[4];
    uint16_t mask = 1 << (cols - 1);
    while ((allCols & mask) == 0) {
      cols --;
      mask = mask >> 1;
    }
  }
}

/*
 * The number of aliens remaining
 */
uint8_t AlienGrid::getAlienCount () {
  return (alienCount);
}

/*
 * Get the coordinates
 */
uint8_t AlienGrid::getTop () {
  return (grid_y);
}

uint8_t AlienGrid::getBottom () {
  return (grid_y + (rows * AG_ROWHEIGHT));
}

uint8_t AlienGrid::getLeft () {
  return (grid_x);
}

uint8_t AlienGrid::getRight () {
  return (grid_x + (cols * AG_COLWIDTH));
}

/*
 * Get the number of columns left in the grid
 */
uint8_t AlienGrid::getCols () {
  return (cols);
}

/*
 * Returns the x coord of the middle of the specified column
 */
uint8_t AlienGrid::getColX (uint8_t col) {
  return (grid_x + (col * AG_COLWIDTH) + (AG_COLWIDTH / 2));
}

/*
 * Returns the y coord of the lowest alien in the specified column
 * Zero if colummn is empty
 */
uint8_t AlienGrid::getColY (uint8_t col) {
  uint16_t mask = 1 << col;
  for (int8_t i = rows - 1; i >= 0; i --) {
    if (grid[i] & mask) {
      return (grid_y + (i * AG_ROWHEIGHT) + AG_ROWHEIGHT);
    }
  }
  return (0);
}

/*
 * Find a random column with aliens in it
 * Returns the column number
 */
uint8_t AlienGrid::getRandomColumn () {
  if (alienCount == 0) return (0);
  uint16_t all = grid[0] | grid[1] | grid[2] | grid[3] | grid[4];
  uint8_t col = random (cols);
  uint16_t mask = 1 << col;
  if (all & mask) return (col);
  for (uint8_t shift = 1;; shift ++) {
    if (all & (mask << shift)) return (col - shift);
    if (all & (mask >> shift)) return (col + shift);
  }
}
