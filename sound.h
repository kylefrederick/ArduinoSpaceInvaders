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
#ifndef sound_h
#define sound_h
#include <Arduino.h>
#include "hardware.h"

// Sound priorites
#define SOUND_NONE 0
#define SOUND_ALIEN_MARCH 1
#define SOUND_LASER_FIRE 2
#define SOUND_ALIEN_KILLED 3
#define SOUND_MYSTERY_FLYBY 4
#define SOUND_MYSTERY_KILLED 5
#define SOUND_BASE_EXPLODE 6

// The alien march sounds
// No, these are not the real frequencies from the game, which were around 200Hz
// But they sounded pants on a piezo.
#define SOUND_MARCH1_FREQ 51
#define SOUND_MARCH2_FREQ 36
#define SOUND_MARCH3_FREQ 48
#define SOUND_MARCH4_FREQ 64
#define SOUND_MARCH_COUNTDOWN 40

// Laser frequency
#define SOUND_LASER_FREQ 3400
#define SOUND_LASER_COUNTDOWN 1

// Alien death
#define SOUND_ALIEN_HIGH_FREQ 2590
#define SOUND_ALIEN_LOW_FREQ 365
#define SOUND_ALIEN_DURATION 300
#define SOUND_ALIEN_FREQ_STEP ((SOUND_ALIEN_HIGH_FREQ - SOUND_ALIEN_LOW_FREQ) / (SOUND_ALIEN_DURATION / SOUND_ALIEN_COUNTDOWN))
#define SOUND_ALIEN_COUNTDOWN 2

// Mystery ship
#define SOUND_MYSTERY_FLY_HIGH_FREQ 4000
#define SOUND_MYSTERY_FLY_LOW_FREQ 800
#define SOUND_MYSTERY_FLY_COUNTDOWN 2
#define SOUND_MYSTERY_FLY_FREQ_STEP ((SOUND_MYSTERY_FLY_HIGH_FREQ - SOUND_MYSTERY_FLY_LOW_FREQ) / (SOUND_MYSTERY_FLY_SWEEP / SOUND_MYSTERY_FLY_COUNTDOWN))
#define SOUND_MYSTERY_FLY_SWEEP 60

// Mystery ship shot
#define SOUND_MYSTERY_KILL_HIGH_FREQ 2000
#define SOUND_MYSTERY_KILL_LOW_FREQ 400
#define SOUND_MYSTERY_KILL_COUNTDOWN 2
#define SOUND_MYSTERY_KILL_FREQ_STEP ((SOUND_MYSTERY_KILL_HIGH_FREQ - SOUND_MYSTERY_KILL_LOW_FREQ) / (SOUND_MYSTERY_KILL_SWEEP / SOUND_MYSTERY_KILL_COUNTDOWN))
#define SOUND_MYSTERY_KILL_SWEEP 80

// Base explosion
#define SOUND_EXPLODE_LOW_FREQ 200
#define SOUND_EXPLODE_HIGH_FREQ 3000
#define SOUND_EXPLODE_COUNTDOWN 1

// Countdown limit
#define SOUND_MAX_COUNTDOWN 32767

class Sound {
  public:
    int alienMarch (int countdown);
    int laserFire (int countdown);
    int alienKilled (int countdown);
    int mysteryFlyby (int countdown);
    int mysteryKilled (int countdown);
    int baseExplode ();
    int countdownComplete ();
    void soundStop ();
    void laserStop ();
    void mysteryStop ();
    uint8_t soundPlaying ();

  private:
    uint8_t currentSound;
    uint16_t currentFrequency;
    boolean ascending;
};





#endif
