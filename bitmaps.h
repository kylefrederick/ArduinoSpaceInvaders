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
 * Mini bitmaps
 * Each bitmap is 5 bits high, but can be any width
 * the MSB of each byte indicates the end of the bitmap
 */

#ifndef bitmaps_h
#define bitmaps_h
/* 
 * Bitmap offsets
 */
#define BM_EXCLAIM 0
#define BM_STAR (BM_EXCLAIM + 3)
#define BM_HYPHEN (BM_STAR + 3)
#define BM_DOT (BM_HYPHEN + 3)
#define BM_0 (BM_DOT + 3)
#define BM_1 (BM_0 + 3)
#define BM_2 (BM_1 + 3)
#define BM_3 (BM_2 + 3)
#define BM_4 (BM_3 + 3)
#define BM_5 (BM_4 + 3)
#define BM_6 (BM_5 + 3)
#define BM_7 (BM_6 + 3)
#define BM_8 (BM_7 + 3)
#define BM_9 (BM_8 + 3)
#define BM_EQUALS (BM_9 + 3)
#define BM_QUESTION (BM_EQUALS + 3)
#define BM_A (BM_QUESTION + 4)
#define BM_B (BM_A + 3)
#define BM_C (BM_B + 3)
#define BM_D (BM_C + 3)
#define BM_E (BM_D + 3)
#define BM_F (BM_E + 3)
#define BM_G (BM_F + 3)
#define BM_H (BM_G + 3)
#define BM_I (BM_H + 3)
#define BM_J (BM_I + 3)
#define BM_K (BM_J + 3)
#define BM_L (BM_K + 3)
#define BM_M (BM_L + 3)
#define BM_N (BM_M + 5)
#define BM_P (BM_N + 4)
#define BM_Q (BM_P + 3)
#define BM_R (BM_Q + 3)
#define BM_S (BM_R + 3)
#define BM_T (BM_S + 3)
#define BM_U (BM_T + 3)
#define BM_V (BM_U + 3)
#define BM_W (BM_V + 3)
#define BM_X (BM_W + 5)
#define BM_Y (BM_X + 3)
#define BM_Z (BM_Y + 3)
#define BM_INVERTED_Y (BM_Z + 3)
#define BM_BACK (BM_INVERTED_Y + 3)
#define BM_MYSTERY (BM_BACK + 3)
#define BM_ALIEN30_1 (BM_MYSTERY + 9)
#define BM_ALIEN30_2 (BM_ALIEN30_1 + 5)
#define BM_ALIEN20_1 (BM_ALIEN30_2 + 5)
#define BM_ALIEN20_2 (BM_ALIEN20_1 + 7)
#define BM_ALIEN10_1 (BM_ALIEN20_2 + 7)
#define BM_ALIEN10_2 (BM_ALIEN10_1 + 8)
#define BM_BASE (BM_ALIEN10_2 + 8)
#define BM_BASE_DEAD (BM_BASE + 9)
#define BM_DEFENCE (BM_BASE_DEAD + 9)
#define BM_EXPLOSION (BM_DEFENCE + 14)
#define BM_LASER (BM_EXPLOSION + 7)
#define BM_BOMB_1_1 (BM_LASER + 1)
#define BM_BOMB_1_2 (BM_BOMB_1_1 + 2)
#define BM_BOMB_2_1 (BM_BOMB_1_2 + 2)
#define BM_BOMB_2_2 (BM_BOMB_2_1 + 3)
#define BM_BOX_TOP (BM_BOMB_2_2 + 3)
#define BM_BOX_SIDE (BM_BOX_TOP + 9)
#define BM_BOX_BOTTOM (BM_BOX_SIDE + 1)
#define BM_TITLE01 (BM_BOX_BOTTOM + 9)
#define BM_TITLE02 (BM_TITLE01 + 72)
#define BM_TITLE03 (BM_TITLE02 + 76)
#define BM_TITLE04 (BM_TITLE03 + 71)
#define BM_TITLE05 (BM_TITLE04 + 69)
#define BM_TITLE06 (BM_TITLE05 + 66)
#define BM_TITLE07 (BM_TITLE06 + 105)
#define BM_TITLE08 (BM_TITLE07 + 116)
#define BM_TITLE09 (BM_TITLE08 + 115)
#define BM_TITLE10 (BM_TITLE09 + 106)
#define BM_TITLE11 (BM_TITLE10 + 101)

/*
 * Bitmap definitions
 * All bitmaps are coded in vertical strips 5 bits high. LSB at the top.
 * This suits the format of the SSD1306 display.
 * Bitmaps are of variable width, with the MSB set high to indicate the end of the bitmap.
 * Not all ASCII characters are provided, as only a limited set is required for the game.
 */
const uint8_t bitmaps[] PROGMEM = {
  B00000000, B00011101, B10000000, // !
  B00001010, B00011111, B10001010, // *
  B00000100, B00000100, B10000100, // -
  B00000000, B00010000, B10000000, // .
  B00001110, B00010001, B10001110, // 0
  B00010010, B00011111, B10010000, // 1
  B00011001, B00010101, B10010010, // 2
  B00010101, B00010101, B10001010, // 3
  B00001111, B00001000, B10011100, // 4
  B00010111, B00010101, B10001001, // 5
  B00001110, B00010101, B10001001, // 6
  B00000001, B00011101, B10000011, // 7
  B00001010, B00010101, B10001010, // 8 
  B00010010, B00010101, B10001110, // 9
  B00001010, B00001010, B10001010, // =
  B00000010, B00000001, B00010101, B10000010, // ?
  B00011110, B00000101, B10011110, // A
  B00011111, B00010101, B10001010, // B
  B00001110, B00010001, B10001010, // C
  B00011111, B00010001, B10001110, // D
  B00011111, B00010101, B10010001, // E
  B00011111, B00000101, B10000001, // F
  B00001110, B00010001, B10011010, // G
  B00011111, B00000100, B10011111, // H
  B00010001, B00011111, B10010001, // I
  B00010001, B00011111, B10000001, // J
  B00011111, B00000100, B10011011, // K
  B00011111, B00010000, B10010000, // L
  B00011111, B00000010, B00000100, B00000010, B10011111, // M 
  B00011111, B00000010, B00000100, B10011111, // N
  B00011111, B00000101, B10000010, // P
  B00001110, B00010001, B10011110, // Q
  B00011111, B00000101, B10011010, // R
  B00010010, B00010101, B10001001, // S
  B00000001, B00011111, B10000001, // T
  B00011111, B00010000, B10011111, // U
  B00001111, B00010000, B10001111, // V
  B00001111, B00010000, B00001110, B00010000, B10001111, // W
  B00011011, B00000100, B10011011, // X
  B00000011, B00011100, B10000011, // Y
  B00011001, B00010101, B10010011, // Z
  B00011000, B00000111, B10011000, // Inverted Y
  B00000100, B00001110, B10011111, // Back symbol
  B00001100, B00011010, B00001110, B00001011, B00011110, B00001011, B00001110, B00011010, B10001100, // Mystery ship - 9
  B00010100, B00001010, B00010111, B00001010, B10010100, // 30pt alien position 1 - 5
  B00000100, B00011010, B00000111, B00011010, B10000100, // 30pt alien position 2 - 5
  B00011000, B00001101, B00011010, B00001110, B00011010, B00001101, B10011000, // 20pt alien position 1 - 7
  B00001100, B00011101, B00001010, B00001110, B00001010, B00011101, B10001100, // 20pt alien position 2 - 6
  B00010110, B00011111, B00001101, B00000111, B00000111, B00001101, B00011111, B10010110, // 10pt alien position 1 - 8
  B00001110, B00011111, B00010101, B00000111, B00000111, B00010101, B00011111, B10001110, // 10pt alien position 2 - 8 
  B00011000, B00011100, B00011100, B00011100, B00011111, B00011100, B00011100, B00011100, B10011000, // Base - 9
  B00010100, B00011001, B00011000, B00011100, B00011101, B00011000, B00011010, B00010001, B10010100, // Base destroyed - 9
  B00011100, B00011110, B00011111, B00011111, B00001111, B00000111, B00000111, B00000111, B00000111, B00001111, B00011111, B00011111, 
  B00011110, B10011100, // Defence - 14
  B00000101, B00010010, B00001100, B00000001, B00000100, B00001010, B10010001, // Explosion - 7
  B10001111, // Laser
  B00010101, B10001010, // Bomb 1 position 1
  B00001010, B10010101, // Bomb 1 position 2
  B00001000, B00011111, B10001000, // Bomb 2 position 1
  B00000010, B00011111, B10000010, // Bomb 2 position 1
  B00011000, B00001000, B00001000, B00001000, B00001000, B00001000, B00001000, B00001000, B10011000, // Box top - 9
  B10011111, // Box side - 1
  B00000011, B00000010, B00000010, B00000010, B00000010, B00000010, B00000010, B00000010, B10000011, // Box bottom - 9
  // Title row 1 - 72
  B00010000, B00001000, B00001010, B00001001, B00001001, B00001001, B00001001, B00001001, B00001001, B00010001, B00000010,
  B00000100, B00001000, B00000000, B00000000, B00011000, B00001011, B00001001, B00001001, B00001001, B00001001, B00001001,
  B00001001, B00001001, B00001001, B00010010, B00000100, B00001000, B00000000, B00000000, B00000000, B00011000, B00001011,
  B00001001, B00001001, B00001001, B00001001, B00001001, B00001011, B00011000, B00000000, B00000000, B00000000, B00000000,
  B00001000, B00000100, B00010010, B00001001, B00001001, B00001001, B00001001, B00001001, B00001001, B00001001, B00001010,
  B00010000, B00000000, B00000000, B00000000, B00000011, B00011001, B00001001, B00001001, B00001001, B00001001, B00001001,
  B00001001, B00001001, B00001001, B00001011, B00001000, B10011000,
  // Title row 2 - 76
  B00011000, B00011100, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011100,
  B00011000, B00010001, B00000010, B00000000, B00000000, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, 
  B00011110, B00011110, B00011110, B00011110, B00011100, B00011000, B00010001, B00000110, B00000000, B00000000, B00000000, 
  B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00000000, B00000000, 
  B00000000, B00000000, B00000110, B00010001, B00011000, B00011100, B00011110, B00011110, B00011110, B00011110, B00011110, 
  B00011110, B00011110, B00011110, B00011100, B00011000, B00000000, B00000000, B00000000, B00011110, B00011110, B00011110, 
  B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B00011110, B10001110,
  // Title row 3 - 71
  B00000011, B00001111, B00011111, B00011111, B00011111, B00011111, B00011101, B00000001, B00000111, B00001111, B00001111, 
  B00001111, B00001111, B00001111, B00000000, B00000000, B00000011, B00011111, B00011111, B00011111, B00011111, B00011111,
  B00000000, B00000001, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00000000, B00000000, B00011000, 
  B00011111, B00011111, B00011111, B00011111, B00001111, B00011111, B00011111, B00011111, B00011111, B00011000, B00000000, 
  B00000000, B00000000, B00000000, B00011111, B00011111, B00011111, B00011111, B00011111, B00000001, B00000000, B00001111, 
  B00001111, B00001111, B00001111, B00001111, B00001111, B00000000, B00000000, B00011111, B00011111, B00011111, B00011111, 
  B00011111, B00000111, B00000001, B00000100, B10001100,
  // Title row 4 - 69
  B00000011, B00000111, B00001111, B00011111, B00011111, B00011110, B00011100, B00011100, B00011100, B00011100, B00011100, 
  B00011000, B00010010, B00000100, B00000000, B00000011, B00011111, B00011111, B00011111, B00011111, B00011100, B00011000,
  B00011111, B00011111, B00011111, B00011111, B00011111, B00001111, B00000000, B00000000, B00011111, B00011111, B00011111, 
  B00011111, B00011111, B00000000, B00011111, B00011111, B00011111, B00011111, B00011111, B00000000, B00000000, B00000000,
  B00011000, B00011111, B00011111, B00011111, B00011111, B00000011, B00000000, B00000100, B00000100, B00000100, B00000100,
  B00001100, B00000000, B00000000, B00010000, B00011111, B00011111, B00011111, B00011111, B00011111, B00001111, B00001111,
  B00001111, B00001111, B10000111,
  // Title row 5 - 66
  B00011000, B00011000, B00011001, B00011001, B00011001, B00010001, B00000001, B00011111, B00011111, B00011111, B00011111, 
  B00011111, B00010000, B00000000, B00000011, B00011111, B00011111, B00011111, B00011111, B00000011, B00000011, B00000011, 
  B00000011, B00000001, B00000000, B00000000, B00000000, B00011100, B00011111, B00011111, B00011111, B00011111, B00001111,
  B00001110, B00001111, B00011111, B00011111, B00011111, B00011111, B00011100, B00000000, B00000000, B00011111, B00011111, 
  B00011111, B00011111, B00011111, B00010000, B00011000, B00011111, B00011111, B00011111, B00011111, B00001111, B00000000,
  B00011000, B00011111, B00011111, B00011111, B00011111, B00011111, B00010001, B00010000, B00010010, B00010110, B10010000,
  // Title row 6 - 105
  B00001100, B00000100, B00000100, B00000100, B00001100, B00000000, B00000100, B00001100, B00000100, B00000100, B00000100,
  B00001000, B00000000, B00001100, B00000100, B00000100, B00000100, B00001100, B00000000, B00001000, B00000001, B00000011,
  B00000011, B00000111, B00000111, B00000111, B00000111, B00000111, B00000111, B00000111, B00000011, B00000001, B00001100,
  B00000000, B00000111, B00000111, B00000111, B00000111, B00000111, B00000000, B00000100, B00000100, B00000100, B00001100,
  B00000000, B00000000, B00000111, B00000111, B00000111, B00000111, B00000111, B00000000, B00000100, B00000000, B00000111, 
  B00000111, B00000111, B00000111, B00000111, B00000000, B00000000, B00000011, B00000111, B00000111, B00000111, B00000111, 
  B00000111, B00000111, B00000111, B00000111, B00000011, B00000001, B00000100, B00000000, B00000111, B00000111, B00000111, 
  B00000111, B00000111, B00000111, B00000111, B00000111, B00000111, B00000111, B00000011, B00000000, B00000100, B00000100, 
  B00000100, B00001000, B00000000, B00000000, B00000000, B00000000, B00010000, B00001000, B00000100, B00000100, B00000100, 
  B00000100, B00000100, B00000100, B00000100, B00000100, B10001000,
  // Title row 7 - 116
  B00011000, B00011000, B00011000, B00011011, B00011001, B00011001, B00000001, B00000001, B00001011, B00011000, B00011000, 
  B00011001, B00011011, B00010001, B00000001, B00000001, B00011010, B00011000, B00011011, B00011001, B00011001, B00011001, 
  B00000011, B00000000, B00011000, B00011011, B00011001, B00011001, B00011001, B00010001, B00000011, B00001100, B00010000, 
  B00000000, B00011011, B00011001, B00011001, B00011001, B00011001, B00000011, B00000000, B00000000, B00011011, B00011001, 
  B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011011, B00000000, B00000000, B00000000, B00000000, 
  B00000000, B00011011, B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, 
  B00011001, B00010010, B00000100, B00000000, B00000000, B00000000, B00011011, B00011001, B00011001, B00011001, B00011001, 
  B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011011, B00011000, B00000000, B00000011, B00011001, 
  B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011001, B00011010, B00011000, 
  B00010000, B00000000, B00000100, B00000010, B00000010, B00010001, B00010001, B00011001, B00011001, B00011001, B00011001, 
  B00011001, B00011001, B00011001, B00011010, B00011000, B10011000,
  // Title row 8 - 115
  B00000001, B00000111, B00001111, B00011111, B00011111, B00011111, B00011110, B00011000, B00000001, B00000011, B00001111,
  B00011111, B00011111, B00011111, B00011110, B00011000, B00010011, B00000111, B00011111, B00011111, B00011111, B00011111, 
  B00011100, B00000000, B00000011, B00001111, B00011111, B00011111, B00011111, B00011110, B00010000, B00000001, B00000000,
  B00001111, B00011111, B00011111, B00011111, B00011111, B00011000, B00000000, B00000000, B00011111, B00011111, B00011111, 
  B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011110, B00000000, B00000000, B00000000, B00000000,
  B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00000001, B00011111, B00011111, B00011111, B00011111, 
  B00011111, B00011111, B00000000, B00000000, B00011100, B00011111, B00011111, B00011111, B00011111, B00011111, B00000011,
  B00000001, B00000101, B00000101, B00001101, B00000001, B00011001, B00000000, B00011000, B00011110, B00011111, B00011111, 
  B00011111, B00011111, B00000111, B00000001, B00011001, B00011111, B00011111, B00011111, B00011111, B00001111, B00000011,
  B00010000, B00011100, B00011110, B00011111, B00011111, B00011111, B00001111, B00000011, B00010001, B00011101, B00011111, 
  B00011111, B00011111, B00011111, B00000111, B10000001,
  // Title row 9 - 106
  B00000001, B00000011, B00001111, B00011111, B00011111, B00011111, B00011110, B00011000, B00000001, B00000111, B00001111,
  B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011111, B00011000,
  B00000000, B00000001, B00000111, B00011111, B00011111, B00011111, B00011110, B00011000, B00000000, B00000111, B00011111, 
  B00011111, B00011111, B00011111, B00000000, B00000000, B00000001, B00011111, B00011111, B00011111, B00011111, B00000001,
  B00001111, B00011111, B00011111, B00011111, B00011111, B00010000, B00000000, B00000000, B00011111, B00011111, B00011111, 
  B00011111, B00011111, B00000111, B00000000, B00011111, B00011111, B00011111, B00011111, B00011111, B00000011, B00000000,
  B00011000, B00011111, B00011111, B00011111, B00011111, B00011111, B00001111, B00001111, B00001111, B00001111, B00001111,
  B00000011, B00000000, B00010000, B00011110, B00011111, B00011111, B00011111, B00011111, B00001111, B00011100, B00011100,
  B00011111, B00011111, B00010011, B00000011, B00001011, B00001001, B00000000, B00000110, B00001111, B00011111, B00011111, 
  B00011111, B00011111, B00011101, B00011100, B00011100, B00011100, B10011000,
  // Title row 10 - 101
  B00000011, B00000111, B00001111, B00011111, B00011111, B00011110, B00011000, B00000001, B00000111, B00011111, B00011111, 
  B00011111, B00011110, B00011001, B00000011, B00000111, B00001111, B00011111, B00011111, B00011100, B00010000, B00000000,
  B00000011, B00001111, B00011111, B00011111, B00011111, B00011110, B00011111, B00011111, B00011111, B00011111, B00011110,
  B00000000, B00000000, B00011111, B00011111, B00011111, B00011111, B00001110, B00000110, B00000111, B00011111, B00011111, 
  B00011111, B00011111, B00011000, B00000000, B00011111, B00011111, B00011111, B00011111, B00011111, B00011000, B00011000,
  B00011111, B00011111, B00011111, B00011111, B00001111, B00000000, B00010000, B00011111, B00011111, B00011111, B00011111, 
  B00011111, B00011001, B00011000, B00011001, B00011001, B00011011, B00000000, B00010000, B00011100, B00011111, B00011111, 
  B00011111, B00011111, B00000011, B00010000, B00011100, B00011111, B00011111, B00011111, B00001111, B00000011, B00011000, 
  B00011100, B00011110, B00011110, B00011110, B00011110, B00010010, B00011000, B00011100, B00011111, B00011111, B00001111, 
  B00000111, B10000011,
  // Title row 11 - 93
  B00000001, B00000001, B00000001, B00000001, B00000001, B00000000, B00000000, B00000001, B00000001, B00000001, B00000001, 
  B00000001, B00000000, B00000000, B00000001, B00000001, B00000001, B00000001, B00000000, B00000000, B00000000, B00000001, 
  B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000000, B00000000, B00000001, 
  B00000001, B00000001, B00000001, B00000000, B00000000, B00000000, B00000001, B00000001, B00000001, B00000001, B00000001, 
  B00000000, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, 
  B00000000, B00000000, B00000000, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, B00000001, 
  B00000001, B00000001, B00000000, B00000000, B00000001, B00000001, B00000001, B00000001, B00000001, B00000000, B00000000, 
  B00000001, B00000001, B00000001, B00000001, B00000001, B00000000, B00000000, B00000000, B00000001, B00000001, B00000001, 
  B00000001, B00000001, B00000001, B00000001, B10000001
};

/*
 * ASCII character offsets
 * An offset of 0xFF indicates a space
 * Unused characters will be marked as spaces
 */
const uint8_t charTable[] PROGMEM = {
  0xff, BM_EXCLAIM, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, BM_STAR, 0xff, 0xff, BM_HYPHEN, BM_DOT, 0xff, // space -> /
  BM_0, BM_1, BM_2, BM_3, BM_4, BM_5, BM_6, BM_7, BM_8, BM_9, // 0 -> 9
  0xff, 0xff, BM_BACK, BM_EQUALS, 0xff, BM_QUESTION, BM_INVERTED_Y,  // : -> @
  BM_A, BM_B, BM_C, BM_D, BM_E, BM_F, BM_G, BM_H, BM_I, BM_J, BM_K, BM_L, BM_M, // A -> M
  BM_N, BM_0, BM_P, BM_Q, BM_R, BM_S, BM_T, BM_U, BM_V, BM_W, BM_X, BM_Y, BM_Z, // N -> Z (O is substituted with zero)
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // [ -> `
  BM_A, BM_B, BM_C, BM_D, BM_E, BM_F, BM_G, BM_H, BM_I, BM_J, BM_K, BM_L, BM_M, // a -> m (mapped to the uppercase)
  BM_N, BM_0, BM_P, BM_Q, BM_R, BM_S, BM_T, BM_U, BM_V, BM_W, BM_X, BM_Y, BM_Z, // n -> z (O is substituted with zero)
  0xff, 0xff, 0xff, 0xff // { -> ~
};

/*
 * Bomb table
 * 
 * Bomb 1 = fast wiggly position 1
 * Bomb 1 = fast wiggly position 2
 * Bomb 2 = big slow position 1
 * Bomb 2 = big slow postion 2
 */
const uint8_t bombTable[][2] PROGMEM = {
  { 0, 0 },
  { BM_BOMB_1_1, BM_BOMB_1_2 },
  { BM_BOMB_2_1, BM_BOMB_2_2 }
};

const uint8_t highScoreCharMap[][10] PROGMEM = {
  { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' },
  { 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T' },
  { 'U', 'V', 'W', 'X', 'Y', 'Z', '.', '-', '!', '<' }
};

#endif