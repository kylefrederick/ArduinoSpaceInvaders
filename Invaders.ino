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
 *
 * Arduino Space Invaders - the smallest space invader game in the world (possibly)
 *
 * @Author: Nick Cranch
 *
 * Designed for Arduino Uno and the SSD1306 128x64 OLED I2C screen
 *
 * FOR DIFFERENT HARDWARE, YOU CAN CHANGE THE PINS IN THE FILE hardware.h
 *
 */
#include "hardware.h"
#include "SSD1306.h"
#include "bitmaps.h"
#include "alien_grid.h"
#include "defence.h"
#include "base.h"
#include "bomb.h"
#include "mystery.h"
#include "sound.h"
#include <EEPROM.h>

// How fast stuff moves (higher values are slower)
#define COUNTDOWN_BASE_MOVE 19 // How fast the base moves in milliseconds per pixel
#define COUNTDOWN_LASER_MOVE 7 // How fast the laser moves
#define COUNTDOWN_MYSTERY_MOVE 47 // How fast the mystery ship moves
#define COUNTDOWN_ALIEN_STEP 49 // Maximim alien speed
// Delays
#define COUNTDOWN_EXPLOSION 99 // How long an alien explosion lasts
#define COUNTDOWN_BOMB_MOVE 21 // How fast the fast alien bombs move (slow bombs move at half the speed)
#define COUNTDOWN_BASE_DEAD 1000 // How long the delay when a base is destroyed
#define COUNTDOWN_INTER_LEVEL 3000 // How long the delay between levels
#define COUNTDOWN_MYSTERY_HIT 503 // How long the mystery ship points remains visible after being shot
// Probabilities
#define MYSTERY_PROBABILITY 20000 // The probability of a mystery ship appearing per iteration of the game loop 1:x
#define BOMB_FACTOR 5 // The probability of an alien dropping a bomb (higher values = lower probability)

#define MAX_BOMBS 4 // TThe maximum number of simultaneous alien bombs
#define BOMB_POINT1 8 // The closer the aliens are to the ground, the more bombs they drop, this is the first point
#define BOMB_POINT2 19 // This is the second

#define EXTRA_LIFE 1000 // Number of points to score to earn an extra life

/*
 * Variables and objects
 * Please note, all objects are predefined and reused in order to avoid heap
 * fragmentation.
 */
SSD1306 screen(-1); // Screen object (No reset line)
AlienGrid aliens; // Create the alien grid object
Base base; // Create the base object
uint16_t score; // Current score value / 10
uint8_t lives; // Lives remaining (including the current one)
Defence defence[4]; // The defence objects
Bomb bombs[MAX_BOMBS]; // The simultaneous bombs
Mystery mystery; // The mystery ship object
Sound sounds; // Sounds object
uint8_t level; // Current level (level zero = demo mode)

// Countdown timers
int alienStepCountdown = 0; // Countdown till the next alien step
int baseMoveCountdown = COUNTDOWN_BASE_MOVE; // Countdown until the base can move again
int laserMoveCountdown = 0; // Countdown until the laser moves again
int bombMoveCountdown = 0; // Countdown until the bombs can move again
int explosionCountdown = COUNTDOWN_EXPLOSION; // Countdown until the alien explosion is removed
int interLevelCountdown = COUNTDOWN_INTER_LEVEL; // Countdown until the next level starts
int baseDeadCountdown = COUNTDOWN_BASE_DEAD; // Countdown until the base explosion sound stops
int mysteryMoveCountdown = COUNTDOWN_MYSTERY_MOVE; // Countdown until the mystery ship can move again
int mysteryHitCountdown = COUNTDOWN_MYSTERY_HIT; // Countdown until the mystery score is removed
int soundCountdown = 30000; // Countdown until the sound stops (just set to a ridiculously high value, because nothing's happening)
boolean fireButtonReleased; // A flag to indicate the the fire button has been released (fire button must be released prior to another shot)
boolean screenUpdateRequired = false; // A flag to indicate that the screen has changed and requires update
// Char buffer for high score name
uint8_t highScoreName[3];

/*
 * Set up
 */
void setup() {
Serial.begin (9600);
  // Button pin initialisation
  pinMode (LEFT_PIN, INPUT_PULLUP);
  pinMode (RIGHT_PIN, INPUT_PULLUP);
  pinMode (FIRE_PIN, INPUT_PULLUP);
  // Let the pins settle
  delay (1);
  // Check the EEPROM for a high score table (If the fire button is held down during power up, the high score table is cleared
  highScoreInit (!digitalRead (FIRE_PIN));
  // Set up the screen
  screen.init();
  screen.clear();
  // Initialise demo mode
  demoStart();
}

/*
 * Main loop
 */
void loop () {
  // If a game is being played
  if (level) {
    gameLoop ();
  // Otherwise, check for new high scores or run the demo
  } else {
    if (score) {
      // If this is set, the game is over, but the high score entry screen has not yet been initialised
      if (fireButtonReleased) {
        // If this is a new high score
        if (isNewHighScore (score)) {
          // Initialise the high score entry
          initHighScoreGrid ();
          fireButtonReleased = false;
        } else {
          // Not a new high score so make sure, we skip this in future and just run the demo
          score = 0;
        }
      } else {
        // Do the high score entry
        highScoreLoop ();
      }
    } else {
      // Demo mode
      demoLoop ();
    }
  }
}

/*
 * Game loop
 * The game plays until either all bases have been destroyed or the aliens reach the bottom.
 */
void gameLoop() {
  // Get the time, so that we can tell how long a single iteration took
  unsigned long timeNow = millis ();

  // Test for fire button press
  if (base.fire (screen, !digitalRead (FIRE_PIN))) {
    // Laser released, update the screen
    screenUpdateRequired = true;
    // ...and start the coundown
    laserMoveCountdown = COUNTDOWN_LASER_MOVE;
    soundCountdown = sounds.laserFire (soundCountdown);
  }

  // Move aliens
  if (alienStepCountdown <= 0) {
    aliens.step (screen);
    // If the base isn't dead, make the sound
    if (!base.isDead ()) soundCountdown = sounds.alienMarch (soundCountdown);
    // If the aliens reach the bottom, then it's game over
    if (aliens.getBottom () >= BASE_Y) {
      gameOver ();
      return;
    }
    // Set the countdown until the next step
    alienStepCountdown = COUNTDOWN_ALIEN_STEP + (aliens.getAlienCount () * 4) + (COUNTDOWN_ALIEN_STEP / level);
    screenUpdateRequired = true;
  }

  // Move mystery
  if (mysteryMoveCountdown <= 0) {
    mystery.move (screen);
    mysteryMoveCountdown = COUNTDOWN_MYSTERY_MOVE;
    screenUpdateRequired = true;
    // If the mystery ship has moved off the screen, stop the sound
    if (!mystery.exists ()) sounds.mysteryStop ();
  }
  
  // Remove alien explosion if present
  if (explosionCountdown <= 0) {
    aliens.clearExplosion ();
    screenUpdateRequired = true;
    // Stop this from repeating unnecessarily, set the countdown ready for the next one
    explosionCountdown = COUNTDOWN_EXPLOSION;
  }

  // Remove mystery score if present
  if (mysteryHitCountdown <= 0) {
    // Clear the whole top line.... fuck it!
    screen.clearRect (0, 6, 128, 5);
    // Set the countdown ready for the next one
    mysteryHitCountdown = COUNTDOWN_MYSTERY_HIT;
    // Mark the myster ship as destroyed
    mystery.destroy ();
    // And stop the sound
    sounds.mysteryStop ();
    screenUpdateRequired = true;
  }
  
  // Move base if left or right button pressed
  if (baseMoveCountdown <= 0) {
    if (base.moveBase (screen, !digitalRead (LEFT_PIN), !digitalRead (RIGHT_PIN))) {
      // If it moved set the screen update flag
      screenUpdateRequired = true;
    }
    baseMoveCountdown = COUNTDOWN_BASE_MOVE;
  }

  // Move laser shot, if there is one
  if (laserMoveCountdown <= 0) {
    // If there was a laser move
    if (base.moveLaser (screen)) {
      // The screen needs updating
      screenUpdateRequired = true;
      // If the laser still exists (it could have gone off the top of the screen)
      if (base.getLaserY ()) {
        // Check for collisions with aliens
        uint16_t hit = aliens.collisionDetect (screen, base.getLaserX (), base.getLaserY ());
        // Did we hit an alien?
        if (hit) {
          // Yay!
          // Remove the laser shot
          base.destroyLaser ();
          // Make the noise of a dying alien
          soundCountdown = sounds.alienKilled (soundCountdown);
          // Update the score with the hit
          updateScore (hit);
          // Start the alien explosion countdown
          explosionCountdown = COUNTDOWN_EXPLOSION;
        } else {   
          // Check for collisions with defences
          if (defenceCollisionDetect (base.getLaserX (), base.getLaserY (), aliens.getBottom (), POWER_LASER)) {
            // Destroy the laser shot
            base.destroyLaser ();
            // Stop the laser sound
            sounds.laserStop ();
          } else {
            // Check for collisions with the mystery ship (if there is one)
            hit = mystery.collisionDetect (screen, base.getLaserX (), base.getLaserY ());
            if (hit) {
              // Wooo!
              // Remove the laser shot
              base.destroyLaser ();
              // Start the mystery killed sound
              soundCountdown = sounds.mysteryKilled (soundCountdown);         
              // Add the mystery value to the score
              updateScore (hit);
              // Set the countdown to remove the mystery score
              mysteryHitCountdown = COUNTDOWN_MYSTERY_HIT;
            } else {
              // Didn't hit anything, so draw it
              base.drawLaser (screen);
            }
          }
        }
      } else {
        // Laser shot has gone off the top, so stop the sound
        sounds.laserStop ();
      }
    }
    // Set the countdown
    laserMoveCountdown = COUNTDOWN_LASER_MOVE;
  }

  // Move bombs if there are any and count them as we go
  if (bombMoveCountdown <= 0) {
    // If any bombs moved, update the screen
    if (moveAndCreateBombs ()) screenUpdateRequired = true;
    bombMoveCountdown = COUNTDOWN_BOMB_MOVE;
  }

  // Create a mystery? (if the aliens are low enough and there are sufficient aliens remaining)
  if ((aliens.getTop () > 11) && mystery.canCreateNew () && (random (MYSTERY_PROBABILITY) == 0) && aliens.getAlienCount () > 7) {
    mystery.init (screen);
    soundCountdown = sounds.mysteryFlyby (soundCountdown);
  }
  
  // Check for end of level delay
  if (interLevelCountdown <= 0) {
    // Next level
    level ++;
    // Initialise
    startLevel ();
    // Reset the countdown, ready for next time
    interLevelCountdown = COUNTDOWN_INTER_LEVEL;
  }

  // If the base is dead and the coundown has reached zero
  if (baseDeadCountdown <= 0) {
    // If a mystery ship exists, wait for it to complete its flyby
    if (mystery.exists ()) {
      // Restart the flyby sound if the explosion sound is still playing
      if (sounds.soundPlaying () == SOUND_BASE_EXPLODE) {
        sounds.soundStop ();
        soundCountdown = sounds.mysteryFlyby (soundCountdown);
      }
    // If there's no mystery ship
    } else {
      // If the explosion sound is still playing
      if (sounds.soundPlaying ()) {
        // Stop it and set the countdown again
        sounds.soundStop ();
        baseDeadCountdown = COUNTDOWN_BASE_DEAD;
      // The sound has been previously switched off, so now we can move on
      } else {
        // Reduce the life count
        lives --;
        if (lives == 0) {
          // End of game  - write message
          gameOver ();
          return;
        }
        // Update the lives on screen
        updateLives ();
        // Remove the base debris from the screen
        base.clearBase (screen);
        // Create a new one
        base.init ();
        // Put it on the screen
        base.drawBase (screen);
        // Reset the countdown ready for next time
        baseDeadCountdown = COUNTDOWN_BASE_DEAD;
        screenUpdateRequired = true;
      }
    }
  }
  
  // Update screen if required
  if (screenUpdateRequired) {
    screen.update ();
    screenUpdateRequired = false;
  }

  // Update sounds
  if (soundCountdown <= 0) {
    soundCountdown = sounds.countdownComplete ();
  }
  
  // Calcualate the time taken again (including screen update time)
  unsigned long timeTaken = millis() - timeNow;
  // If the the iteration took no time, then put in a 1 millisecond delay
  if (timeTaken == 0) {
    delay (1);
    timeTaken = 1;
  }
  
  // Adjust all countdowns
  if (aliens.getAlienCount ()) alienStepCountdown -= timeTaken;
  baseMoveCountdown -= timeTaken;
  bombMoveCountdown -= timeTaken;
  if (base.getLaserY ()) laserMoveCountdown -= timeTaken;
  if (aliens.explosionPresent ()) explosionCountdown -= timeTaken;
  if (aliens.getAlienCount () == 0) interLevelCountdown -= timeTaken;
  if (base.isDead ()) baseDeadCountdown -= timeTaken;
  if (mystery.exists ()) mysteryMoveCountdown -= timeTaken;
  if (mystery.wasHit ()) mysteryHitCountdown -= timeTaken;
  if (sounds.soundPlaying ()) soundCountdown -= timeTaken;
}

/*
 * Find the next available bomb slot
 * WARNING! - Don't use this routine unless you know there's at least one free.
 */
uint8_t findUnusedBomb () {
  for (uint8_t i = 0; i < MAX_BOMBS; i ++) {
    if (!bombs[i].exists()) {
      return (i);
    }
  }
}

/*
 * Add the hit to the score, check for a bonus life and update the screen
 */
void updateScore (uint16_t hit) {
  // Check for a bonus base
  if (score < EXTRA_LIFE && (score + hit) >= EXTRA_LIFE) {
    lives ++;
    updateLives ();
  }
  // Add the hit value to the score
  score += hit;
  // Update the screen
  screen.clearRect (24, 0, 24, 5);
  screen.setCursor (24, 0);
  screen.writeScore (score);
}

/*
 * Udate the number of lives on the screen
 */
void updateLives () {
  screen.clearRect (98, 0, 30, 5);
  for (int i = 1; i < lives; i ++) {
    screen.drawBitmap (BM_BASE, 88 + (i * 10), 0);
  }
}

/*
 * Initialise the necessary variables to start the game
 */
void gameStart () {
  score = 0;
  lives = 3;
  level = 1;
  base.init ();
  startLevel ();
  interLevelCountdown = COUNTDOWN_INTER_LEVEL;
  mysteryMoveCountdown = COUNTDOWN_MYSTERY_MOVE;
  explosionCountdown = COUNTDOWN_EXPLOSION; 
  mysteryHitCountdown = COUNTDOWN_MYSTERY_HIT;
  baseDeadCountdown = COUNTDOWN_BASE_DEAD;  
  alienStepCountdown = 0;
  laserMoveCountdown = 0;
  bombMoveCountdown = 0;
  soundCountdown = 30000;
  fireButtonReleased = false;
}

/*
 * Set up the screen ready for the next level
 */
void startLevel () {
  screen.clear ();
  screen.write (F("SCORE "));
  screen.writeScore (score);
  screen.setCursor (74, 0);
  screen.write (F("LIVES"));
  updateLives ();
  // Create the alien grid (starting position will vary according to level)
  aliens.init (calcAlienStartY());
  // Create the defences
  for (uint8_t i = 0; i < 4; i ++) {
    defence[i].init (screen, 15 + (i * 28));
  }
  // Make sure all the bombs are initialised
  for (int i = 0; i < MAX_BOMBS; i ++) {
    bombs[i].destroy ();
  }
  // The laser too
  base.destroyLaser ();
  // ...and the mystery ship
  mystery.destroy ();
  // Put the base on the screen
  base.drawBase (screen);
}

/*
 *  Check if a bomb or laser hit one of the defences
 *  Returns true if there was a hit
 */
boolean defenceCollisionDetect (uint8_t x, uint8_t y, uint8_t alien_y, uint8_t power) {
  // Go through each defence
  for (int i = 0; i < 4; i ++) {
    // and check for a collision
    if (defence[i].collisionDetect (screen, x, y, alien_y, power)) {
      return (true);
    }
  }
  return (false);
}

/*
 * Calculate the alien grid start position, based on level
 */
uint8_t calcAlienStartY () {
  uint8_t y = (level * 4) + 2;
  if (y > 23) y = 23;
  return (y);
}

/*
 * If there are any bombs in transit move them and check for collisions
 */
boolean moveAndCreateBombs () {
  // The count of currenly active bombs (accumulated as we go)
  uint8_t bombCount = 0;
  // screen update flag
  boolean update = false;
  // Get the bottom of the alien grid
  uint8_t alien_y = aliens.getBottom ();
  // Go through the bomb array
  for (int i = 0; i < MAX_BOMBS; i ++) {
    // If the bomb exists and moved
    if (bombs[i].move (screen)) {
      update = true;
      // If the bombs still exists (if could have hit the bottom of the screen)
      if (bombs[i].exists ()) {
        // check for collisions with the defences
        if (defenceCollisionDetect (bombs[i].getX (), bombs[i].getY (), alien_y, bombs[i].getPower())) {
          bombs[i].destroy ();
        } else {
          // Check for collisions with the base
          if (base.collisionDetect (screen, bombs[i].getX (), bombs[i].getY ())) {
            // Oops! Base hit, set the countdown for the explosion
            baseDeadCountdown = COUNTDOWN_BASE_DEAD;
            // And start the explosion sound
            sounds.baseExplode ();
            // Remove the bomb
            bombs[i].destroy ();
          } else {
            // Didn't hit anything, so just draw it
            bombs[i].draw (screen);
            // Count it
            bombCount ++;
          }
        }
      }
    }
  }
  // If the base is dead, no new bombs, so escape here
  if (base.isDead ()) return (update);
  // Calculate what the current maximum number of bombs is
  uint8_t bombMax = level > 1 ? 1 : 0;
  if (aliens.getTop () > BOMB_POINT1) {
    bombMax ++;
    if (level > 1 && aliens.getTop () > BOMB_POINT2) {
      bombMax ++;
      if (level > 2) {
        bombMax ++;
      }
    }
  }
  // Can we release one?
  if (bombCount < bombMax) {
    // Probability is related to how close the aliens are to the bottom and the number of remaining bombs available
    if (random (0, ((((SSD1306_LCDHEIGHT - aliens.getBottom ()) * BOMB_FACTOR) * (bombCount + 1)) / bombMax)) == 0) {
      // Select a random alien column
      uint8_t col = aliens.getRandomColumn ();
      // Get the y value of that alien
      uint8_t bomb_y = aliens.getColY (col);
      // If there's an alien in this column
      if (bomb_y) {
        // Create a new bomb
        bombs[findUnusedBomb ()].create (screen, aliens.getColX (col), bomb_y);
        update = true;
      }
    }
  }
  return (update);
}

/*
 * Game over
 * Clear an area in the middle of the screen and write GAME OVER
 * Stop any sounds
 * Wait for 5 seconds
 * Tidy up
 * Trigger the demo start
 */
void gameOver () {
  screen.clearRect (38, 22, 52, 19);
  screen.setCursor (45, 29);
  screen.write (F("GAME OVER"));
  screen.update ();
  sounds.soundStop ();
  delay (5000);
  // Get rid of unwanted potential remnants
  mystery.destroy ();
  base.destroyLaser ();
  for (uint8_t i = 0; i < MAX_BOMBS; i ++) {
    bombs[i].destroy ();
  }
  demoStart ();
  fireButtonReleased = true; // This is re-used for checking the score and performing the high score table stuff
}

/*
 * Draw the SPACE INVADERS graphic on the screen
 */
void drawTitle () {
  screen.clear ();
  screen.drawBitmap (BM_TITLE01, 29, 5);
  screen.drawBitmap (BM_TITLE02, 27, 10);
  screen.drawBitmap (BM_TITLE03, 27, 15);
  screen.drawBitmap (BM_TITLE04, 29, 20);
  screen.drawBitmap (BM_TITLE05, 31, 25);
  screen.drawBitmap (BM_TITLE06, 12, 30);
  screen.drawBitmap (BM_TITLE07, 6, 35);
  screen.drawBitmap (BM_TITLE08, 7, 40);
  screen.drawBitmap (BM_TITLE09, 10, 45);
  screen.drawBitmap (BM_TITLE10, 14, 50);
  screen.drawBitmap (BM_TITLE11, 17, 55);
  screen.update ();
}

/**************************************************************************
 * 
 * DEMO
 * 
 * All code below this point relates to the demo mode.
 * There is a lot of variable re-use in order to save valuable RAM
 * These variables have been given aliases by the #defines following this
 * comment.
 * 
 * Yes, the code is a bit clunky, but it's all done to save RAM
 * 
 *************************************************************************/
#define demoStep lives // Reusing the lives variable
#define demoStepCountdown interLevelCountdown // Reusing interLevelCountdown variable
#define slowTypeOffset alienStepCountdown // Reusing alienStepCountdown variable
#define baseDirection soundCountdown // Reusing soundCountdown variable
#define animationStepCountdown mysteryMoveCountdown // Reusing mysteryMoveCountdown variable
#define animationStep mysteryHitCountdown // Reusing mysteryHitCountdown variable
#define alienX alienStepCountdown // Reusing alienStepCountdown variable

/*
 * Trigger the demo start
 */
void demoStart () {
  demoStep = 0; // Set the beginning of the demo sequence
  level = 0; // Setting the level to 0 indicates demo mode
  demoStepCountdown = 0; // Set the coundown
}

void demoLoop () {
  // If the fire button is pressed
  if (!digitalRead (FIRE_PIN)) {
    // Trigger the game to start
    gameStart ();
    return;
  }
  if (demoStepCountdown <= 0) {
    switch (demoStep) {
      case 0:
        // Draw the main title for 5 seconds and move on to the next step
        drawTitle ();
        nextStep (5000);
        break;
      case 1:
        // Slow type the word "PLAY"
        // Clear the screen
        screen.clear ();
        // and set the cursor position for slow type
        slowTypeInit (56, 8);
        // No delay
        nextStep (1);
        break;
      case 2:
        // Slow type the word "PLAY"
        if (slowType (F("PLAY"))) {
          // Finished, move on the the next step
          nextStep (200);
          // Set the cursor position for slow type
          slowTypeInit (34, 20);
        } else {
          // Update the screen, and go round again
          screen.update ();
        }
        break;
      case 3:
      case 13:
        // Slow type "SPACE INVADERS"
        if (slowType (F("SPACE INVADERS"))) {
          // Finished, move to the next step
          nextStep (200);
        } else {
          screen.update ();
        }
        break;
      case 4:
      case 14:
        // Draw the score table
        screen.setCursor (22, 32);
        screen.write (F("*SCORE ADVANCE TABLE*"));
        screen.drawBitmap (BM_MYSTERY, 38, 38);
        screen.drawBitmap (BM_ALIEN30_1, 40, 44);
        screen.drawBitmap (BM_ALIEN20_1, 39, 50);
        screen.drawBitmap (BM_ALIEN10_1, 38, 56);
        screen.update ();
        // Set the cursor position for slow type
        slowTypeInit (48, 38);
        // Move on to the next step
        nextStep (1);
        break;
      case 5:
      case 15:
        // Slow type "=? MYSTERY"
        if (slowType (F("=? MYSTERY"))) {
          // Finished, move to the next step
          nextStep (200);
          // Set the cursor position for slow type
          slowTypeInit (48, 44);
        } else {
          // Update the screen and go round again
          screen.update ();
        }
        break;
      case 6:
      case 16:
        // Slow type "=30 POINTS"
        if (slowType (F("=30 POINTS"))) {
          // Finished, move on to the next step
          nextStep (200);
          // Set the cursor postion for slow type 
          slowTypeInit (48, 50);
        } else {
          // Update the screen and go round again
          screen.update ();
        }
        break;
      case 7:
      case 17:
        // Slow type "=20 POINTS"
        if (slowType (F("=20 POINTS"))) {
          // Finished, move on to the next step
          nextStep (200);
          // Set the cursor position for slow type
          slowTypeInit (48, 56);
        } else {
          // Update the screen and go round again
          screen.update ();
        }
        break;
      case 8:
      case 18:
        // Slow type "=10 POINTS"
        if (slowType (F("=10 POINTS"))) {
          // Finished, move on to the next step
          nextStep (demoStep == 8 ? 5000 : 1000);
        } else {
          // Update the screen and go round again
          screen.update ();
        }
        break;
      case 9:
        // Game play animation
        screen.clear ();
        // Set up the alien grid
        aliens.init (6);
        // and the base
        base.init ();
        // Create defences
        for (uint8_t i = 0; i < 4; i ++) {
          defence[i].init (screen, 15 + (i * 28));
        }
        // Clear all the bombs
        for (uint8_t i = 0; i < MAX_BOMBS; i ++) {
          bombs[i].destroy ();
        }
        // and the laser
        base.destroyLaser ();
        // Draw the base
        base.drawBase (screen);
        // Update the screen
        screen.update ();
        // Set the base movement to stationary
        baseDirection = 0;
        // Set the animation step
        animationStep = 0;
        // Set up the countdown timers
        alienStepCountdown = 200 + COUNTDOWN_ALIEN_STEP + COUNTDOWN_ALIEN_STEP;
        bombMoveCountdown = COUNTDOWN_BOMB_MOVE;
        animationStepCountdown = 50;
        baseDeadCountdown = COUNTDOWN_BASE_DEAD;
        // Move on the the next demo step
        nextStep (100);
        break;
      case 10:
        screenUpdateRequired = false;
        // Animation steps
        if (animationStepCountdown <= 0) {
          animationStep ++;
          animationStepCountdown = 50;
          switch (animationStep) {
            case 4:
              // Base fires and starts moving right
              base.fire (screen, false);
              base.fire (screen, true);
              baseDirection = 1; // Start moving base right
              break;
            case 19:
              // Alien column 10 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (10), aliens.getColY (10), FAST_BOMB);
              break;            
            case 21:
              // Base stops and fires
              baseDirection = 0; // Stop base
              base.fire (screen, false);
              base.fire (screen, true);
              break;
            case 25:
              // Alien column 3 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (3), aliens.getColY (3), SLOW_BOMB);
              break;
            case 26:
              // Base fires
              base.fire (screen, false);
              base.fire (screen, true);
              break;            
            case 29:
              // Alien column 0 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (0), aliens.getColY (0), SLOW_BOMB);
              break;
            case 30:
              // Base fires and starts moving right
              base.fire (screen, false);
              base.fire (screen, true);
              baseDirection = 1; // Start moving right
              break;
            case 35:
              // Alien column 5 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (5), aliens.getColY (5), SLOW_BOMB);
              // Base stops and fires
              base.fire (screen, false);
              base.fire (screen, true);
              baseDirection = 0;
              break;
            case 41:
              // Alien column 6 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (6), aliens.getColY (6), FAST_BOMB);
              break;
            case 45:
              // Base fires and starts moving left
              base.fire (screen, false);
              base.fire (screen, true);
              baseDirection = -1;
              break;
            case 48:
              // Alien column 6 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (6), aliens.getColY (6), SLOW_BOMB);
              break;
            case 54:
              // Base fires and starts to move right
              base.fire (screen, false);
              base.fire (screen, true);
              baseDirection = 1;
              // Alien column 6 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (6), aliens.getColY (7), FAST_BOMB);
              break;
            case 60:
              // Alien column 3 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (3), aliens.getColY (3), SLOW_BOMB);
              break;
            case 66:
              // Alien column 0 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (0), aliens.getColY (0), SLOW_BOMB);
              break;
            case 69:
              // Alien column 2 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (2), aliens.getColY (2), FAST_BOMB);
              // Base stops and fires
              base.fire (screen, false);
              base.fire (screen, true);
              baseDirection = 0;
              break;
            case 73:
              // Base fires and starts to move left
              base.fire (screen, false);
              base.fire (screen, true);
              baseDirection = -1;
              break;
            case 78:
              // Alien column 4 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (4), aliens.getColY (4), SLOW_BOMB);
              // Base fires
              base.fire (screen, false);
              base.fire (screen, true);
              break;
            case 79:
              // Base starts to move right
              baseDirection = 1;
              break;
            case 82:
              // Alien column 0 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (0), aliens.getColY (0), SLOW_BOMB);
              break;
            case 88:
              // Alien column 0 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (0), aliens.getColY (0), FAST_BOMB);
              // Base fires
              base.fire (screen, false);
              base.fire (screen, true);
              break;
            case 100:
              // Base stops
              baseDirection = 0;
              break;
            case 103:
              // Alien column 0 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (0), aliens.getColY (0), FAST_BOMB);
              break;
            case 105:
              // Base fires
              base.fire (screen, false);
              base.fire (screen, true);
              break;
            case 108:
              // Alien column 9 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (9), aliens.getColY (9), SLOW_BOMB);
              break;
            case 112:
              // Alien column 3 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (3), aliens.getColY (3), SLOW_BOMB);
              // Base fires
              base.fire (screen, false);
              base.fire (screen, true);
              break;
            case 114:
              // Alien column 10 drops a bomb
              bombs[findUnusedBomb ()].create (screen, aliens.getColX (10), aliens.getColY (10), SLOW_BOMB);
              break;     
          }
        }
        // Remove explosion if present
        if (explosionCountdown <= 0) {
          aliens.clearExplosion ();
          screenUpdateRequired = true;
          // Stop this from repeating unnecessarily
          explosionCountdown = COUNTDOWN_EXPLOSION;
        }
        // Alien step
        if (alienStepCountdown <= 0) {
          aliens.step (screen);
          alienStepCountdown = 220 + COUNTDOWN_ALIEN_STEP + COUNTDOWN_ALIEN_STEP;
          screenUpdateRequired = true;
        }
        // Move bombs
        if (bombMoveCountdown <= 0) {
          // Go through each bomb
          for (uint8_t i = 0; i < MAX_BOMBS; i ++) {
            // Move it it it's active
            if (bombs[i].move (screen)) {
              // Did it move?
              screenUpdateRequired = true;
              // If it still exists
              if (bombs[i].exists ()) {
                // Check for collisions with the defences
                if (defenceCollisionDetect (bombs[i].getX (), bombs[i].getY (), aliens.getBottom (), bombs[i].getPower ())) {
                  bombs[i].destroy ();
                } else {
                  // Check for collisions with the base
                  if (base.collisionDetect (screen, bombs[i].getX (), bombs[i].getY ())) {
                    baseDeadCountdown = COUNTDOWN_BASE_DEAD;
                  } else {
                    // No collisions, just draw it
                    bombs[i].draw (screen);
                  }
                }
              }
            }
          }
          bombMoveCountdown = COUNTDOWN_BOMB_MOVE;
        }
        // Move the base if it's moving
        if (baseMoveCountdown <= 0) {
          if (base.moveBase (screen, baseDirection < 0, baseDirection > 0)) {
            screenUpdateRequired = true;
          }
          baseMoveCountdown = COUNTDOWN_BASE_MOVE;
        }
        // Move the laser shot if there is one
        if (laserMoveCountdown <= 0) {
          if (base.moveLaser (screen)) {
            // If there was one
            screenUpdateRequired = true;
            // If it's still there
            if (base.getLaserY ()) {
              // Check for collisions with aliens
              if (aliens.collisionDetect (screen, base.getLaserX (), base.getLaserY ())) {
                // Hit, ok, remove the laser shot
                base.destroyLaser ();
                // Start the alien explosion countdown
                explosionCountdown = COUNTDOWN_EXPLOSION;
              } else {
                // Check for collisions with the defences
                if (defenceCollisionDetect (base.getLaserX (), base.getLaserY (), aliens.getBottom (), POWER_LASER)) {
                  // Hit, so remove the laser shot
                  base.destroyLaser ();
                } else {
                  // Otherwise, draw the laser shot
                  base.drawLaser (screen);
                }
              }
            }
          }
          laserMoveCountdown = COUNTDOWN_LASER_MOVE;
        }
        // Update the screen if necessary
        if (screenUpdateRequired) screen.update ();
        // Process all of the countdowns
        alienStepCountdown --;
        baseMoveCountdown --;
        bombMoveCountdown --;
        animationStepCountdown --;
        if (aliens.explosionPresent ()) explosionCountdown --;
        if (base.getLaserY ()) laserMoveCountdown --;
        if (base.isDead ()) baseDeadCountdown --;
        // If the base is dead or the aliens have gotten too low, end the animation
        if (baseDeadCountdown <= 0 || aliens.getBottom () < 35) {
          nextStep (1000);
        } else {
          demoStepCountdown = 1;
        }
        break;
      case 11:
        // Clear the screen and prepare for the slow type
        screen.clear ();
        slowTypeInit (56, 8);
        nextStep (1);
        break;
      case 12:
        // Slow type "PLAY" with the inverted "Y"
        if (slowType (F("PLA@"))) {
          // Finished, move on to the next step
          nextStep (200);
          // Set the cursor position for slow type
          slowTypeInit (34, 20);
        } else {
          screen.update ();
        }
        break;
      case 19:
        // Start the alien at the right side of the screen
        alienX = 122;
        // Draw it
        screen.drawBitmap (BM_ALIEN30_1, alienX, 8);
        // Update the screen
        screen.update ();
        // Move on to the next step
        nextStep (1);
        break;
      case 20:
        // Move the alien left
        // Erase it
        screen.clearRect (alienX, 8, 5, 5);
        // Move left
        alienX --;
        // Has it reached the inverted Y?
        if (alienX <= 71) {
          // Yes, move to the next step
          nextStep (30);
          break;
        }
        // Draw the alien
        screen.drawBitmap (alienX % 2 == 1 ? BM_ALIEN30_2 : BM_ALIEN30_1, alienX, 8);
        screen.update ();
        // Go round again
        demoStepCountdown = 30;
        break;
      case 21:
        // Move the alien and inverted Y right
        // Erase it
        screen.clearRect (alienX - 3, 8, 8, 5);
        // Move right
        alienX ++;
        // Has it reached the edge of the screen?
        if (alienX >= 122) {
          // Yes, move to the next step
          nextStep (500);
          screen.update ();
          break;
        }
        // Draw the inverted Y
        screen.drawBitmap (BM_INVERTED_Y, alienX - 3, 8);
        // Draw the alien
        screen.drawBitmap (alienX % 2 == 1 ? BM_ALIEN30_2 : BM_ALIEN30_1, alienX, 8);
        screen.update ();
        demoStepCountdown = 30;
        // go round again
        break;
      case 22:
        // Move the alien and Y left
        // Did the Y reach its destination?
        if (alienX <= 72) {
          // Yes, so move to the next step
          nextStep (500);
          break;
        }
        // Erase the alien and Y
        screen.clearRect (alienX - 3, 8, 8, 5);
        // Move left
        alienX --;
        // Draw the Y
        screen.drawBitmap (BM_Y, alienX - 3, 8);
        // Draw the alien
        screen.drawBitmap (alienX % 2 == 1 ? BM_ALIEN30_2 : BM_ALIEN30_1, alienX, 8);        
        screen.update ();
        demoStepCountdown = 30;
        // Go round again
        break;
      case 23:
        // Remove the alien
        screen.clearRect (72, 8, 5, 5);
        screen.update ();
        // After a 1 second delay
        nextStep (000);
        break;
      case 24:
        // Show high score table (wait for 5 seconds if anything was displayed
        demoStepCountdown = displayHighScores () ? 5000 : 1;
        // Restart the demo
        demoStep = 0;
    }
  }
  delay (1);
  demoStepCountdown --;
}

/*
 * Move to the next step, setting the delay time
 */
void nextStep (int delay) {
  demoStep ++;
  demoStepCountdown = delay;
}

/*
 * Set the cursor position and offset ready for slow type
 */
void slowTypeInit (uint8_t x, uint8_t y) {
  // Set the offset to the beginning
  slowTypeOffset = 0;
  // and the cursor position
  screen.setCursor (x, y); 
}

/*
 * Slow Type
 * Put each character of the string on the screen individually
 */
boolean slowType (const __FlashStringHelper* s) {
  demoStepCountdown = 200;
  uint8_t ch = pgm_read_byte((char *)s + slowTypeOffset);
  if (ch) {
    screen.write (ch);
    slowTypeOffset ++;
    return (false);
  }
  return (true);
}

/**************************************************************************
 * 
 * HIGH SCORE TABLE STUFF
 * 
 * Everything in this section relates to the high score table stored in
 * EEPROM
 * 
 *************************************************************************/

#define HS_OFFSET 0 // Change this if you want to store the high score table somewhere else in EEPROM
#define HS_START (HS_OFFSET + 2) // Room for the magic number
#define HS_MAX 5 // Maximum number of entries
#define HS_RECORD_SIZE 5 // Records consist of int (2 bytes) + three initials
#define HS_NAME_OFFSET 2
#define HS_END (HS_START + (HS_MAX * HS_RECORD_SIZE))
#define HS_GRID_WIDTH 10
#define HS_GRID_HEIGHT 3
#define HS_GRID_X 24
#define HS_GRID_Y 31
#define HS_GRID_STEP_X 8
#define HS_GRID_STEP_Y 10
#define NAME_X_START 71
#define NAME_Y 19

#define namePtr alienStepCountdown // Reusing alienStepCountdown variable
#define cursorX soundCountdown // Reusing soundCountdown variable
#define cursorY mysteryMoveCountdown // Reusing mysteryMoveCountdown variable
#define flashCountdown mysteryHitCountdown // Reusing mysteryHitCountdown variable
#define buttonReleased explosionCountdown // Reusing explosionCountdown variable
#define nameX baseMoveCountdown  // Reusing baseMoveCountdown variable
#define flashOn laserMoveCountdown // reusing laserMoveCountdown variable


/*
 * Display the high score table
 * Returns true if there is a high score table, false if empty
 */
boolean displayHighScores () {
  uint8_t displayPos = '1';
  uint8_t actualPos = '1';
  uint16_t currentHighScore = 0;
  uint16_t address;
  uint16_t currentScore;
  // Find the highest score
  for (uint16_t i = HS_START; i < HS_END; i += HS_RECORD_SIZE) {
    currentScore = readUint16_t (i);
    if (currentScore > currentHighScore) {
      currentHighScore = currentScore;
      address = i;
    }
  }
  if (currentHighScore == 0) return (false);
  screen.clear ();
  screen.setCursor (42, 7);
  screen.write ("HIGH SCORES");
  for (uint8_t i = 16; i < 16 + (HS_MAX * 8); i += 8) {
    if (currentHighScore == 0) break;
    if (displayPos == actualPos) {
      screen.setCursor (20, i);
      screen.write (displayPos);
      switch (displayPos) {
        case '1':
          screen.write ("ST");
          break;
        case '2':
          screen.write ("ND");
          break;
        case '3':
          screen.write ("RD");
          break;
        default:
          screen.write ("TH");
      }
    }
    screen.setCursor (currentHighScore > 9999 ? 56 : 52, i);
    screen.writeScore (currentHighScore);
    screen.setCursor (96, i);
    screen.write (EEPROM.read (address + 2));
    screen.write (EEPROM.read (address + 3));
    screen.write (EEPROM.read (address + 4));
    address += HS_RECORD_SIZE;
    while (address < HS_END) {
      if (readUint16_t (address) == currentHighScore) {
        break;
      }
      address += HS_RECORD_SIZE;
    }
    actualPos ++;
    if (address >= HS_END) {
      currentScore = 0;
      uint16_t tmpScore;
      for (uint16_t j = HS_START; j < HS_END; j += HS_RECORD_SIZE) {
        tmpScore = readUint16_t (j);
        if (tmpScore < currentHighScore) {
          if (tmpScore > currentScore) {
            currentScore = tmpScore;
            address = j;
          }
        }
      }
      currentHighScore = currentScore;
      displayPos = actualPos;
    }
  }
  screen.update ();
  return (true);
}

void initHighScoreGrid () {
  screen.clear ();
  screen.setCursor (32, 7);
  screen.write ("HIGH SCORE ");
  screen.writeScore (score);
  screen.setCursor (48, NAME_Y);
  screen.write ("NAME");
  uint8_t x;
  uint8_t y = HS_GRID_Y + 2;
  uint8_t c;
  for (uint8_t i = 0; i < HS_GRID_HEIGHT; i ++) {
    x = HS_GRID_X + 5;
    for (uint8_t j = 0; j < HS_GRID_WIDTH; j ++) {
      c = pgm_read_byte (&highScoreCharMap[i][j]);
      screen.setCursor (x - (screen.charWidth (c) / 2), y);
      screen.write (c);
      x += HS_GRID_STEP_X;
    }
    y += HS_GRID_STEP_Y;
  }
  // Set some variables up
  namePtr = 0;
  cursorX = 0;
  cursorY = 0;
  flashCountdown = 0;
  flashOn = false;
  nameX = NAME_X_START;
  buttonReleased = true;
  drawCursor ();
  screen.update ();
}

/*
 * High score loop allows the player to type in their initials
 */
void highScoreLoop () {
  if (buttonReleased) {
    if (!digitalRead (FIRE_PIN)) {
      // Character selected
      highScoreName[namePtr] = pgm_read_byte (&highScoreCharMap[cursorY][cursorX]);
      if (highScoreName[namePtr] == '<' && namePtr > 0) {
        namePtr --;
      } else {
        namePtr ++;
      }
      screen.clearRect (NAME_X_START, NAME_Y, 24, 5);
      screen.setCursor (NAME_X_START, NAME_Y);
      nameX = NAME_X_START;
      for (uint8_t i = 0; i < namePtr; i ++) {
        screen.write (highScoreName[i]);
        nameX += screen.charWidth (highScoreName[i]);
      }
      screen.update ();
      buttonReleased = false;
    } else if (!digitalRead (LEFT_PIN)) {
      // Move cursor left
      removeCursor ();
      cursorX --;
      if (cursorX < 0) {
        cursorX = HS_GRID_WIDTH - 1;
        cursorY --;
        if (cursorY < 0) {
          cursorY = HS_GRID_HEIGHT - 1;
        }
      }
      drawCursor ();
      screen.update ();
      buttonReleased = false;
    } else if (!digitalRead (RIGHT_PIN)) {
      // Move cursor right
      removeCursor ();
      cursorX ++;
      if (cursorX >= HS_GRID_WIDTH) {
        cursorX = 0;
        cursorY ++;
        if (cursorY >= HS_GRID_HEIGHT) {
          cursorY = 0;
        }
      }
      drawCursor ();
      screen.update ();
      buttonReleased = false;
    }
  } else {
    // Check all the buttons have been released
    buttonReleased =digitalRead (FIRE_PIN) && digitalRead (LEFT_PIN) && digitalRead (RIGHT_PIN);
    if (buttonReleased && namePtr > 2) {
      addHighScore (score, highScoreName);
      score = 0;
      demoStart ();
      delay (1000);
      return;
    }
  }
  if (flashCountdown <= 0 && namePtr < 3) {
    if (flashOn) {
      screen.clearRect (nameX, NAME_Y, 6, 5);
      flashOn = false;
    } else {
      screen.setCursor (nameX, NAME_Y);
      screen.write (pgm_read_byte (&highScoreCharMap[cursorY][cursorX]));
      flashOn = true;
    }
    screen.update ();
    flashCountdown = 500;
  }
  flashCountdown --;
  delay (1); // We put a delay in here, to eliminiate key bounce
}

void drawCursor () {
  uint8_t x = HS_GRID_X + (cursorX * HS_GRID_STEP_X);
  uint8_t y = HS_GRID_Y + (cursorY * HS_GRID_STEP_Y);
  screen.drawBitmap (BM_BOX_TOP, x, y - 3);
  screen.drawBitmap (BM_BOX_SIDE, x, y + 2);
  screen.drawBitmap (BM_BOX_SIDE, x + 8, y + 2);
  screen.drawBitmap (BM_BOX_BOTTOM, x, y + 7);
}

void removeCursor () {
  uint8_t x = HS_GRID_X + (cursorX * HS_GRID_STEP_X);
  uint8_t y = HS_GRID_Y + (cursorY * HS_GRID_STEP_Y);
  screen.clearRect (x, y, 9, 2);
  screen.clearRect (x, y + 2, 1, 5);
  screen.clearRect (x + 8, y + 2, 1, 5);
  screen.clearRect (x, y + 7, 9, 2);
}

/*
 * Check if the high score table exists and if not, create it
 */
void highScoreInit (boolean reset) {
  // Read the magic from the EEPROM
  if (reset || EEPROM.read (HS_OFFSET) != 'S' || EEPROM.read (HS_OFFSET + 1) != 'I') {
    // No magic, so let's initialise the high score table
    for (uint8_t i = 0; i < HS_MAX * HS_RECORD_SIZE; i ++) {
      EEPROM.update (HS_START + i, 0);
    }
    EEPROM.update (HS_OFFSET, 'S');
    EEPROM.update (HS_OFFSET + 1, 'I');
  }
}

/*
 * Check if the score is a new high score
 */
boolean isNewHighScore (uint16_t score) {
  for (uint16_t i = 0; i < HS_MAX * HS_RECORD_SIZE; i ++) {
    if (score > readUint16_t (i)) {
      return (true);
    }
  }
  return (false);
}

/*
 * Add a new score to the high score table
 */
void addHighScore (uint16_t score, char *name) {
  uint16_t address = findLowestScore ();
  if (address) {
    EEPROM.update (address ++, score >> 8);
    EEPROM.update (address ++, score);
    EEPROM.update (address ++, name[0]);
    EEPROM.update (address ++, name[1]);
    EEPROM.update (address ++, name[2]);
  }
}

/*
 * Find the (first) lowest score in the table and return its address
 */
uint16_t findLowestScore () {
  // Look for the lowest score
  uint16_t lowScore = 65535;
  uint16_t currentScore = 0;
  uint16_t lowAddress = 0;
  for (uint16_t i = HS_START; i < HS_END; i += HS_RECORD_SIZE) {
    currentScore = readUint16_t (i);
    if (currentScore == 0) return (i);
    if (currentScore < lowScore) {
      lowScore = currentScore;
      lowAddress = i;
    }
  } 
  return (lowAddress);
}

uint16_t readUint16_t (uint16_t address) {
  return (EEPROM.read (address) << 8 | EEPROM.read (address + 1));  
}
