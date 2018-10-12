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
#define PIN_NOT_BLUE 11 
// Switches in rotary
#define PIN_ROTARY_A 2
#define PIN_ROTARY_B 3
#define PIN_ROTARY_PUSH 4


////////////////////////////////////////////////////////////////////////////////
// Driver for RGB LEDs in rotary
////////////////////////////////////////////////////////////////////////////////
// Bitmasks for getting colors out of the RGB LEDs
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
#define ROTARY_MODULO 8

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

#if ( PIN_ROTARY_A != 2 )
#warning PIN_ROTARY_A is not 2. I hope you used a pin with interrupt support.
#endif
#if ( PIN_ROTARY_B != 3 )
#warning PIN_ROTARY_B is not 3. I hope you used a pin with interrupt support.
#endif

void initRotary( void (*callback)(int) )
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
// APPLICATION CODE
////////////////////////////////////////////////////////////////////////////////

// A sequence of colors to show the rotary working
const int sequence[] = { RGB_BLACK, RGB_RED, RGB_GREEN, RGB_BLUE, RGB_LELLOW, RGB_MAGENTA, RGB_CYAN, RGB_WHITE };
const int SEQ_LENGTH=8;

int current_rotary=0;
int last_rotary=0;

// Callback when the rotary has moved one or more clicks.
// change this for your application
void cbk_rotarychange( int pos )
{
  setRGB( sequence[ pos ] );
  current_rotary=pos;
}

void setup() {
  // LEDs in rotary switch
  initRGB();
  setRGB( RGB_MAGENTA );
  
  // ROTARY
  initRotary( cbk_rotarychange );  

  // ALARMS
  initAlarmService();
  
  // SERIAL
  // for debugging
  Serial.begin(9600);
}

int last_push=0;

void loop() 
{ 
  int push;
  
  if ( last_rotary != current_rotary )
  {
    last_rotary=current_rotary;
    Serial.println( current_rotary );
  }
  push = digitalRead( PIN_ROTARY_PUSH );
  if ( push != last_push )
  {
    last_push=push;

    if (push)
    {
      Serial.println( "push" );
    }
  }
  delay( 25 );
}

