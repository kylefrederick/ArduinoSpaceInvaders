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
 * The sound capabilities of the Arduino are somewhat limited (without
 * additional hardware). Single channel PWM allows us to produce single beeps
 * of various frequencies with no control over amplitude. Having said that,
 * the original Space Invaders game, didn't have much better, although the
 * simple sound chips of that era were capable of multi-channel, plus white
 * noise.
 * I could have used several PWM pins and then, mixed the sound with external
 * circuitry. Sod that! I opted for the next best thing. The same technique
 * used in MP3 encoding... well... sort of. Use the human brain's limited 
 * capacity for processing.
 * Ok, that's a bit of a stretch, but the principle is the same. The system
 * here only plays one sound at a time, based on a priority. The most
 * important / "loudest" sounds have a higher priority. When there are several
 * sounds required simultaneously, only the highest priority one is played.
 * It works quite well considering the limitations.
 */
#include "sound.h"

/*
 * The classic alien march sound... well as faithfully reproduced as possible
 * on a tinny piezo buzzer.
 */
int Sound::alienMarch (int countdown) {
  if (currentSound <= SOUND_ALIEN_MARCH) {
    switch (currentFrequency) {
      case SOUND_MARCH1_FREQ:
        currentFrequency = SOUND_MARCH2_FREQ;
        break;
      case SOUND_MARCH2_FREQ:
        currentFrequency = SOUND_MARCH3_FREQ;
        break;
      case SOUND_MARCH3_FREQ:
        currentFrequency = SOUND_MARCH4_FREQ;
        break;
      default:
        currentFrequency = SOUND_MARCH1_FREQ;
    }
    tone (SOUND_PIN, currentFrequency, SOUND_MARCH_COUNTDOWN);
    return (SOUND_MARCH_COUNTDOWN);
  }
  return (countdown);
}

/*
 * The sound of the laser - pew pew
 */
int Sound::laserFire (int countdown) {
  if (currentSound <= SOUND_LASER_FIRE) {
    tone (SOUND_PIN, SOUND_LASER_FREQ);
    currentSound = SOUND_LASER_FIRE;
    return (SOUND_LASER_COUNTDOWN);
  }
  return (countdown);
}

/*
 * Starts a decending tone
 */
int Sound::alienKilled (int countdown) {
  if (currentSound <= SOUND_ALIEN_KILLED) {
    tone (SOUND_PIN, SOUND_ALIEN_HIGH_FREQ);
    currentSound = SOUND_ALIEN_KILLED;
    currentFrequency = SOUND_ALIEN_HIGH_FREQ;
    return (SOUND_ALIEN_COUNTDOWN);
  }
  return (countdown);
}

/*
 * The mystery ship creates an undulating tone
 */
int Sound::mysteryFlyby (int countdown) {
  if (currentSound <= SOUND_MYSTERY_FLYBY) {
    tone (SOUND_PIN, SOUND_MYSTERY_FLY_LOW_FREQ);
    currentSound = SOUND_MYSTERY_FLYBY;
    currentFrequency = SOUND_MYSTERY_FLY_LOW_FREQ;
    ascending = true;
    return (SOUND_MYSTERY_FLY_COUNTDOWN);
  }
  return (countdown);
}

/*
 * Which speeds up and get lower when they're shot
 */
int Sound::mysteryKilled (int countdown) {
  if (currentSound <= SOUND_MYSTERY_KILLED) {
    tone (SOUND_PIN, SOUND_MYSTERY_KILL_LOW_FREQ);
    currentSound = SOUND_MYSTERY_KILLED;
    currentFrequency = SOUND_MYSTERY_KILL_LOW_FREQ;
    ascending = true;
    return (SOUND_MYSTERY_KILL_COUNTDOWN);
  }
  return (countdown);
}

/*
 * Start the explosion noise
 */
int Sound::baseExplode () {
  tone (SOUND_PIN, random (SOUND_EXPLODE_LOW_FREQ, SOUND_EXPLODE_HIGH_FREQ));
  currentSound = SOUND_BASE_EXPLODE;
  return (random (1, 5));
}

/*
 * Stop all sounds
 */
void Sound::soundStop () {
  currentSound = SOUND_NONE;
  noTone(SOUND_PIN);
}

/*
 * This function is called periodically, according to the required delay
 * It alters the sound if necessary.
 */
int Sound::countdownComplete () {
  switch (currentSound) {
    case SOUND_LASER_FIRE:
      // The laser sound is quite high frequency with a small random element.
      // This gives the sound a sort of hiss, like the original game.
      tone (SOUND_PIN, SOUND_LASER_FREQ + random (200));
      return (SOUND_LASER_COUNTDOWN);
    case SOUND_ALIEN_KILLED:
      // When aliens are shot they just produce a decending tone
      currentFrequency -= SOUND_ALIEN_FREQ_STEP;
      // Which stops when it drops below the threshold
      if (currentFrequency <= SOUND_ALIEN_LOW_FREQ) {
        noTone (SOUND_PIN);
        currentSound = SOUND_NONE;
      } else {
        tone (SOUND_PIN, currentFrequency);
        return (SOUND_ALIEN_COUNTDOWN);
      }
      break;
    case SOUND_MYSTERY_FLYBY:
      // Mystery ships go woo woo woo
      if (ascending) {
        currentFrequency += SOUND_MYSTERY_FLY_FREQ_STEP;
        if (currentFrequency >= SOUND_MYSTERY_FLY_HIGH_FREQ) {
          ascending = false;
        }
      } else {
        currentFrequency -= SOUND_MYSTERY_FLY_FREQ_STEP;
        if (currentFrequency <= SOUND_MYSTERY_FLY_LOW_FREQ) {
          ascending = true;
        }
      }
      tone (SOUND_PIN, currentFrequency);
      return (SOUND_MYSTERY_FLY_COUNTDOWN);
    case SOUND_MYSTERY_KILLED:
      // And then go wawawa when shot
      if (ascending) {
        currentFrequency += SOUND_MYSTERY_KILL_FREQ_STEP;
        if (currentFrequency >= SOUND_MYSTERY_KILL_HIGH_FREQ) {
          ascending = false;
        }
      } else {
        currentFrequency -= SOUND_MYSTERY_KILL_FREQ_STEP;
        if (currentFrequency <= SOUND_MYSTERY_KILL_LOW_FREQ) {
          ascending = true;
        }
      }
      tone (SOUND_PIN, currentFrequency);
      return (SOUND_MYSTERY_KILL_COUNTDOWN);
    case SOUND_BASE_EXPLODE:
      // White noise is produced by a sequence of random frequencies over random short durations
      tone (SOUND_PIN, random (SOUND_EXPLODE_LOW_FREQ, SOUND_EXPLODE_HIGH_FREQ), random (2, 6));
      return (SOUND_EXPLODE_COUNTDOWN);
    case SOUND_ALIEN_MARCH:
      // The march sound is started by its start function and simply stopped here
      currentSound = SOUND_NONE;
  }
  // Default action. Turn off sound.
  noTone (SOUND_PIN);
  return (SOUND_MAX_COUNTDOWN);
}

/*
 * Identify the current sound
 */
uint8_t Sound::soundPlaying () {
  return (currentSound);
}

/*
 * The laser sound has no duration, so has to be stopped when it hits a target
 * or goes off the screen.
 */
void Sound::laserStop () {
  if (currentSound == SOUND_LASER_FIRE) {
    currentSound = SOUND_NONE;
    noTone (SOUND_PIN);
  }
}

/*
 * It's the same for both mystery sounds, and believe me you really do want it
 * stop. It's really irritating when you forget to.
 */
void Sound::mysteryStop () {
  if (currentSound == SOUND_MYSTERY_FLYBY || currentSound == SOUND_MYSTERY_KILLED) {
    currentSound = SOUND_NONE;
    noTone (SOUND_PIN);
  }
}
