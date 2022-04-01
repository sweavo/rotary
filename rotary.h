#ifndef ROTARY_H
#define ROTARY_H

/*  # Rotary encoder
 *   
 *  See https://github.com/sweavo/rotary
 */


////////////////////////////////////////////////////////////////////////////////
// BEGIN user configuration
////////////////////////////////////////////////////////////////////////////////

// Pin definitions
// ===============

// Negative logic for LEDs,  i.e. LED is on when pin is low

#define PIN_NOT_RED 13  
#define PIN_NOT_GREEN 12
#define PIN_NOT_BLUE 10 

// Switches in rotary
// ==================

#define PIN_ROTARY_A 12
#define PIN_ROTARY_B 13
#define PIN_ROTARY_PUSH 11

// Debouncing
// ==========

// How long must there have been no changes to the gray code before we consider
// firing a rotary change event?

#define ROTARY_TIMEOUT  4

// How many unique values rotary_position can take

#define ROTARY_MODULO (16384)


////////////////////////////////////////////////////////////////////////////////
// END user configuration
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// RGS LEDs help
////////////////////////////////////////////////////////////////////////////////

// Bitmasks for getting colors out of the RGB LEDs

#define RGB_BLACK   0
#define RGB_RED     1
#define RGB_GREEN   2
#define RGB_BLUE    4
#define RGB_YELLOW  3
#define RGB_MAGENTA 5
#define RGB_CYAN    6
#define RGB_WHITE   7

// Set the pin directions and initialize to off

void initRGB();

// Given a color with bits 0, 1 and 2 representing R, G, B,
// write to the RGB pins and change the color of the rotary

void setRGB( int color);


////////////////////////////////////////////////////////////////////////////////
// ROTARY handling
////////////////////////////////////////////////////////////////////////////////

typedef uint16_t rotary_position_t;

void initRotary( void (*callback)(rotary_position_t) );

#endif /* ROTARY_H */

