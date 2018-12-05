#include "arduino.h"
#include "rotary.h" 

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
static unsigned long alarm_ticks = 0;
static unsigned long alarm_timeout = 0;
static void (*alarm_callback)(void);

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

void isr_rotaryupdated();

// Clicks of rotary switch
static rotary_position_t rotary_position=0;
// Transitions of gray code (4 to a click).
// Set to 2 so that you have the same amount of quarters to go 
// in either direction to effect a change. 
static uint16_t rotary_quarters=2;
// bits 0 and 1 are the A and B of the rotary. Initialized from pins in setup
static uint16_t rotary_last_graycode;
// callback when the rotary has changed position
static void (*rotary_change_callback)(rotary_position_t);

void initRotary( void (*callback)(rotary_position_t) )
{
  rotary_change_callback = callback;
  pinMode(PIN_ROTARY_A,INPUT_PULLUP);
  pinMode(PIN_ROTARY_B,INPUT_PULLUP);
  attachInterrupt( digitalPinToInterrupt( PIN_ROTARY_A ),
                   isr_rotaryupdated,
                   CHANGE );
  attachInterrupt( digitalPinToInterrupt( PIN_ROTARY_B ),
                   isr_rotaryupdated,
                   CHANGE );
  rotary_last_graycode = ( PIND & 12 ) >>2;

  pinMode( PIN_ROTARY_PUSH, INPUT );

  initAlarmService();
}

// Callback when no gray code changes have occured in the 
// last ROTARY_TIMEOUT milliseconds. It divides the 
// rotary rotary_quarters by 4 to figure out the absolute 
// rotation, and if it has changed, calls the cbk_rotarychange.
void cbk_rotary_quiescent()
{
  uint16_t new_position = rotary_quarters >> 2;
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
static const uint16_t ROTATION_TABLE[] = {  0, (ROTARY_MODULO<<2)-1, 1, 0, 1, 0,  0, (ROTARY_MODULO<<2)-1,
                               (ROTARY_MODULO<<2)-1,  0, 0, 1, 0, 1, (ROTARY_MODULO<<2)-1,  0 };
void isr_rotaryupdated(){
  int sample = PIND & 12;
  rotary_quarters = ( rotary_quarters + 
                      ROTATION_TABLE[ sample | rotary_last_graycode ] ) % ( ROTARY_MODULO << 2 );
  rotary_last_graycode = sample >> 2;
  setAlarm( ROTARY_TIMEOUT, cbk_rotary_quiescent );
}

