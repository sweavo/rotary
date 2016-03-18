/* Driving the spark fun RGB rotary encoder.
 *  
 *  http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6HSPEC.pdf
 *  http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6H.pdf
 *  
 *  LEDs:
 *  Connect pin 13, 12, 11 to R,G,B (pins 1,2,4 of the Rotary package) through 
 *  330Ohm resistors and connect +5v to pin 5. Pin 3 of the rotary package, 
 *  the push button, connects through 10K current-limiter to arduino pin 3. 
 * 
 *  Encoder:
 *  Connect pin C to +5v. Pin A to Pin 2 and Pin B to Pin 3 (because they are both
 *  interrupts) Both should use a 10K pulldown.
 *  
 *  TODO:
 *   Document the pinout in the comment above
 *   Set up notes playable by index rather than frequency
 *   Sort out the flow of control between the rotary and the playing of notes... it resets the game when played!
 */
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
// Pin definitions.
////////////////////////////////////////////////////////////////////////////////

// Negative logic for LEDs,  i.e. LED is on when pin is low
#define PIN_NOT_RED 13  
#define PIN_NOT_GREEN 12
#define PIN_NOT_BLUE 11 
// Switches in rotary
#define PIN_ROTARY_PUSH 10 
#define PIN_ROTARY_A 2
#define PIN_ROTARY_B 3
// Separate LEDs
#define PIN_LED_LELLOW 7
#define PIN_LED_BLUE 8
#define PIN_LED_GREEN 9
#define PIN_LED_RED 10
// Buttons
#define PIN_BUTTON_RED 5
#define PIN_BUTTON_GREEN 4
#define PIN_BUTTON_BLUE A4
#define PIN_BUTTON_LELLOW A5

////////////////////////////////////////////////////////////////////////////////
// RGB
////////////////////////////////////////////////////////////////////////////////
// Symbolic constants for getting colors out of the RGB leds
#define RGB_BLACK   0
#define RGB_RED     1
#define RGB_GREEN   2
#define RGB_BLUE    4
#define RGB_LELLOW  3
#define RGB_MAGENTA 5
#define RGB_CYAN    6
#define RGB_WHITE   7

// Given a color with bits 0, 1 and 2 representing R, G, B,
// write to the RGB pins and change the color of the rotary
void setRGB( int color)
{
  digitalWrite(PIN_NOT_RED, !(color & 1));
  digitalWrite(PIN_NOT_GREEN, !(color & 2));  
  digitalWrite(PIN_NOT_BLUE, !(color & 4));
}

void initRGB()
{
  pinMode(PIN_NOT_RED, OUTPUT);
  pinMode(PIN_NOT_GREEN, OUTPUT);
  pinMode(PIN_NOT_BLUE, OUTPUT);
}

////////////////////////////////////////////////////////////////////////////////
// Alarm service
// Use setAlarm to ask for a callback to fire after n milliseconds. There is
// only one alarm, and a call to setAlarm before the alarm has expired results
// in the old alarm being canceled and the new alarm set.
////////////////////////////////////////////////////////////////////////////////
unsigned long alarm_ticks = 0;
unsigned long alarm_timeout = 0;
void (*alarm_callback)(void);

ISR(TIMER0_COMPA_vect) 
{
  alarm_ticks++;
  if ( ( alarm_ticks == alarm_timeout ) && alarm_timeout )
  {
    alarm_timeout=0;
    (*alarm_callback)();
  }
}

void setAlarm( unsigned long milliseconds, void (*callback)(void) )
{
  alarm_timeout=milliseconds;
  alarm_ticks=0;
  alarm_callback=callback;
}

void initAlarmService()
{
  // Timer0 cycles at approx. 1 millisecond. We will put a CTC
  // interrupt midway through its range.
  OCR0A = 0xAF; // Compare Register
  TIMSK0 |= _BV(OCIE0A); // Mask
  TCCR1B |= _BV(WGM12);  // I think this sets CTC mode
}

////////////////////////////////////////////////////////////////////////////////
// ROTARY handling
////////////////////////////////////////////////////////////////////////////////

// BEGIN user configuration

// How long must there have been no changes to the gray code before we consider
// firing a rotary change event?
#define ROTARY_TIMEOUT  4

// How many unique values rotary_position can take
#define ROTARY_MODULO 5

// END user configuration

// Clicks of rotary switch
int rotary_position=0; 
// Transitions of gray code (4 to a click). 
// Set to 2 so that you have the same amount of quarters to go 
// in either direction to effect a change. 
int rotary_quarters=2;
// bits 0 and 1 are the A and B of the rotary. Initialized from pins in setup
int rotary_last_graycode;
// callback when the rotary has changed position
void (*rotary_change_callback)(int);

void initRotary( void (*callback)(int) )
{
  rotary_change_callback = callback;
  pinMode(PIN_ROTARY_A,INPUT);
  pinMode(PIN_ROTARY_B,INPUT);
  attachInterrupt( digitalPinToInterrupt( PIN_ROTARY_A ),
                   isr_rotaryupdated,
                   CHANGE );
  attachInterrupt( digitalPinToInterrupt( PIN_ROTARY_B ),
                   isr_rotaryupdated,
                   CHANGE );
  assert( ( PIN_ROTARY_A == 2 ) && ( PIN_ROTARY_B == 3 ) );
  rotary_last_graycode = ( PIND & 12 ) >>2;
}

// Callback when no gray code changes have occured in the 
// last ROTARY_TIMEOUT milliseconds. It divides the 
// rotary rotary_quarters by 4 to figure out the absolute 
// rotation, and if it has changed, calls the cbk_rotarychange.
void cbk_rotary_quiescent()
{
  int new_position = rotary_quarters >> 2;
  if ( new_position != rotary_position )
  {
    (*rotary_change_callback)( new_position );
    rotary_position = new_position;
  }
}

// To decode the gray code, we simply look up the direction
// in this table. The index into the table is 0bAABB where 
// AA is the pins A and B just read, and BB is the pins A 
// and B from the previous read. Switch chatter and bounce
// then actually results in multiple interrupts, but this
// lookup can repeatedly update the quarters variable and 
// wind up with the correct value. Each time an interrupt
// is received we start an alarm so that we will only update
// rotary_clicks after any bouncing has settled down.
const int ROTATION_TABLE[] = {  0, (ROTARY_MODULO<<2)-1, 1, 0, 1, 0,  0, (ROTARY_MODULO<<2)-1,
                               (ROTARY_MODULO<<2)-1,  0, 0, 1, 0, 1, (ROTARY_MODULO<<2)-1,  0 };
void isr_rotaryupdated(){
  int sample = PIND & 12;
  rotary_quarters = ( rotary_quarters + 
                      ROTATION_TABLE[ sample | rotary_last_graycode ] ) % ( ROTARY_MODULO << 2 );
  rotary_last_graycode = sample >> 2;
  setAlarm( ROTARY_TIMEOUT, cbk_rotary_quiescent );
}
////////////////////////////////////////////////////////////////////////////////
//  SEPARATE LEDS
////////////////////////////////////////////////////////////////////////////////

void initLED() {
  pinMode( PIN_LED_RED, OUTPUT );
  pinMode( PIN_LED_GREEN, OUTPUT );
  pinMode( PIN_LED_BLUE, OUTPUT );
  pinMode( PIN_LED_LELLOW, OUTPUT );
}

void setLED( int index ){
  digitalWrite( PIN_LED_RED, index == 1 );
  digitalWrite( PIN_LED_GREEN, index == 2 );
  digitalWrite( PIN_LED_BLUE, index == 3 );
  digitalWrite( PIN_LED_LELLOW, index == 4 );
}

////////////////////////////////////////////////////////////////////////////////
// Buttons
////////////////////////////////////////////////////////////////////////////////

 
void initButtons() {
  pinMode( PIN_BUTTON_RED, INPUT );
  pinMode( PIN_BUTTON_GREEN, INPUT );
  pinMode( PIN_BUTTON_BLUE, INPUT );
  pinMode( PIN_BUTTON_LELLOW, INPUT );
}

int button_last_state = 0;
int readButtonsRaw() {
  // Returns the index of the pushed button, not a bitmask.
  return digitalRead( PIN_BUTTON_RED ) ? 1
        : digitalRead( PIN_BUTTON_GREEN ) ? 2 
        : digitalRead( PIN_BUTTON_BLUE ) ? 3 
        : digitalRead( PIN_BUTTON_LELLOW ) ? 4 :0;
}
bool readButtonsBlocking() {
  if ( readButtonsRaw() != button_last_state ) 
  {
    delay( 10 );
    int readButtons = readButtonsRaw();
    if ( readButtons != button_last_state ) 
    {
      button_last_state = readButtons;
      return true;
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Melody
////////////////////////////////////////////////////////////////////////////////
#include "pitches.h"
void play_melody( int pin, int period, const int melody[][2], void (*callback)(int) ) {
  // iterate over the notes of the melody:
  for (int thisNote = 0; melody[thisNote][1]; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = period / melody[thisNote][1];
    tone(pin, melody[thisNote][0], noteDuration);
    if ( callback ) {
      (*callback)(melody[thisNote][0]);
    }
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(pin);
    if ( callback ) {
      (*callback)(0);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
// APPLICATION CODE
////////////////////////////////////////////////////////////////////////////////
#include "tunes.h"

// A sequence of colors to show the rotary working
const int sequence[] = { RGB_BLACK, RGB_RED, RGB_GREEN, RGB_BLUE, RGB_LELLOW };
const int SEQ_LENGTH=5;

int current_rotary=0;
int last_rotary=0;

// Callback when the rotary has moved one or more clicks.
// change this for your application
void cbk_rotarychange( int pos )
{
  setRGB( sequence[ pos ] );
  setLED( pos );
  current_rotary=pos;
  Serial.print( pos );
  Serial.print( '\n' );
}

// Note values are disappointingly nonrandom when it comes to lighting LEDs.
// perturb the values here to get more variety
void cbk_hashnotes( int note )
{
  if (note) {
    cbk_rotarychange( 1  + ((note ^ (note >> 4 ) ^ (note >> 8) ^ (note >> 12)) % (SEQ_LENGTH-1)) );
  } else {
    cbk_rotarychange( 0 );
  }
}
void setup() {
  // LEDs in rotary switch
  initRGB();
  setRGB( RGB_MAGENTA );

  initLED();
  setLED( 0 );
  
  // ROTARY
  initRotary( cbk_rotarychange );  

  // ALARMS
  initAlarmService();
  
  // SERIAL
  // for debugging
  Serial.begin(9600);
}

#define STATE_MENU 0
#define STATE_SIMON_START 1
#define STATE_SIMON 2
#define STATE_SIMON_WAIT 3
#define STATE_PLAYER 4

const int simon_pitch_palette[] = { NOTE_C3, NOTE_D3, NOTE_E3, NOTE_F3 };
#define SIMON_PALETTE_LENGTH 4
#define SIMON_WIN_LENGTH 32
int simon_melody[SIMON_WIN_LENGTH];
int simon_current_length=0;
int simon_cursor;
void initSimon(){
  simon_current_length=1;
  for ( int i = 0; i < SIMON_WIN_LENGTH; i++ ) {
    simon_melody[i] = random(SIMON_PALETTE_LENGTH);
  }
}
void simonTone( int index )
{
  tone(6,simon_pitch_palette[simon_melody[index]],200);
  cbk_hashnotes( simon_pitch_palette[simon_melody[index]]);
  delay(350);
  cbk_hashnotes( 0 );
  delay(150);
}

int buttonToLed( int button ){
  return 1<<(button-1);
}

int state=STATE_MENU;
int last_state=10;
void loop() 
{ 
  if ( last_rotary != current_rotary )
  {
    state = STATE_MENU;
    last_rotary=current_rotary;
  }
  random();
  if (state != last_state){
    last_state=state;
    Serial.print("state ");
    Serial.print(state);
    Serial.print("\n");
  }

  bool button_change = readButtonsBlocking();
  if ( button_change ){
    Serial.print("Buttons ");
    Serial.print(button_last_state);
    Serial.print("\n");
  }
  switch ( state ){
    case STATE_MENU:
      if ( button_change && button_last_state  ) {
        switch (current_rotary )
        {
          case 1:
            state=STATE_SIMON_START;
            Serial.print("Simon\n");
            break;
          case 2:
            state=STATE_PLAYER;
            Serial.print("Player\n");
            break;
          default:
            Serial.print("Invalid Rotary\n");
        }
      }
      break;
    
    case STATE_SIMON_START:
      play_melody( 6,1000,START, cbk_hashnotes );
      delay(1000);
      initSimon();
      state=STATE_SIMON;
      break;
      
    case STATE_SIMON:
      for ( int i = 0; i < simon_current_length; i++ ){
        simonTone( i );    
      }
      simon_cursor=0;
      state=STATE_SIMON_WAIT;
      break;
    
    case STATE_SIMON_WAIT:
      if (button_change && button_last_state) {
        if ( button_last_state == simon_melody[simon_cursor] )
        {
          Serial.print("Correct ");
          Serial.print(simon_cursor);
          Serial.print("\n");
          simonTone( simon_cursor ++ );
          if ( simon_cursor == simon_current_length )
          {
            simon_current_length++;
            play_melody( 6,1000,VICTORY, cbk_hashnotes );
            delay(1000);
            state=STATE_SIMON;
          }
        }
        else
        {
          play_melody( 6, 1000, HAIRCUT, cbk_hashnotes );
          delay(1500);
          state=STATE_SIMON_START;
        }
      }
      break;
    case STATE_PLAYER:
      if (button_change ) {
        setLED( button_last_state);
      }
  }
  
}

