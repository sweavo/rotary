#ifndef ROTARY_H
#define ROTARY_H
/*  # Rotary encoder
 *   
 *  This project's purpose is to drive the spark fun RGB rotary encoder only.
 *  
 *  ## REFERENCE:
 *  
 *  http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6HSPEC.pdf
 *  http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6H.pdf
 *  
 *  ## Wiring
 *  
 *  The pin names given here are defined in `Pin Definitions` below.
 *    
 *  ### Encoder:
 *  
 *  ROTARY_PIN_A and ROTARY_PIN_B should both be interrupt pins (e.g. 2 and 3 on an Uno).
 *  
 *  We don't need pullup resistors because this sketch turns on the internal pullups.
 *  
 *  Connect
 *  * pin C directly to GND
 *  * pin A to ROTARY_PIN_A.
 *  * pin B to ROTARY_PIN_B.
 *  
 *  ### LEDs:
 *  
 *  * Connect R,G,B (pins 1,2,4 of the Rotary package) through 330Ohm current-limiting resistors to PIN_NOT_RED, PIN_NOT_GREEN, PIN_NOT_BLUE on arduino.
 *  * Connect pin 5 (+5v) to +5v. 
 *  
 *  ### Push button:
 *  Pin 3 of the rotary package, the push button, connects to PIN_ROTARY_PUSH with a 10k pulldown resistor
 */
 
////////////////////////////////////////////////////////////////////////////////
// Pin definitions.
////////////////////////////////////////////////////////////////////////////////
// Negative logic for LEDs,  i.e. LED is on when pin is low
#define PIN_NOT_RED 13  
#define PIN_NOT_GREEN 12
#define PIN_NOT_BLUE 10 
// Switches in rotary
#define PIN_ROTARY_A 12
#define PIN_ROTARY_B 13
#define PIN_ROTARY_PUSH 11

////////////////////////////////////////////////////////////////////////////////
// Driver for RGB LEDs in rotary
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

void initRGB();

// Given a color with bits 0, 1 and 2 representing R, G, B,
// write to the RGB pins and change the color of the rotary
void setRGB( int color);

////////////////////////////////////////////////////////////////////////////////
// ROTARY handling
////////////////////////////////////////////////////////////////////////////////

// BEGIN user configuration

// How long must there have been no changes to the gray code before we consider
// firing a rotary change event?
#define ROTARY_TIMEOUT  4

// How many unique values rotary_position can take
#define ROTARY_MODULO (16384)

// END user configuration


typedef uint16_t rotary_position_t;

void initRotary( void (*callback)(rotary_position_t) );

#endif /* ROTARY_H */
