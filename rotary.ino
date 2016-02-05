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
 *  Connect pin C to +5v. Pin A to Pin 2 (because it can be an interrupt pin)
 *  and pin B to pin 9 (it doesn't need an interrupt). Both should use a 10K
 *  pulldown.
 *  
 */

// Wiring the hardware to arduino
const int PIN_NOT_RED=13; // i.e. LED is on when pin is low
//const int PIN_NOT_GREEN=12;
//const int PIN_NOT_BLUE=11;
//const int PIN_ROTARY_PUSH=10;
const int PIN_ROTARY_A=2; // connect to pin A 
const int PIN_ROTARY_B=3;

// Symbolic constants for getting colors out of the RGB leds
const int RGB_BLACK=0;
const int RGB_RED=1;
const int RGB_GREEN=2;
const int RGB_BLUE=4;
const int RGB_YELLOW=3;
const int RGB_MAGENTA=5;
const int RGB_CYAN=6;
const int RGB_WHITE=7;

int interrupt_count=0;
const int ROTARY_TIMEOUT = 2;
int rotary_position=0; // Clicks of rotary switch
int rotary_quarters=0; // Transitions of gray code (4 to a click) 
int rotary_last_graycode; // initialized from pins in setup

// Callback when the rotary has moved one or more clicks.
// change this for your application
void cbk_rotarychange( int pos )
{
  Serial.print( pos );
  Serial.print( ", " );
  Serial.print( interrupt_count );
  Serial.print( '\n' );
  interrupt_count=0;

}

// Callback when no gray code changes have occured in the 
// last ROTARY_TIMEOUT milliseconds. It divides the 
// rotary rotary_quarters by 4 to figure out the rotation, and 
// if it has changed, calls the cbk_rotarychange.
void cbk_alarm()
{
  int new_position = rotary_quarters >> 2;
  if ( new_position != rotary_position )
  {
    cbk_rotarychange( new_position );
    rotary_position = new_position;
  }
}

unsigned long alarm_ticks = 0;
unsigned long alarm_timeout = 0;
// Support for timeout alarm. This should be ticked every 
// 'millisecond'.
ISR(TIMER0_COMPA_vect) 
{
  alarm_ticks++;
  if ( ( alarm_ticks == alarm_timeout ) && alarm_timeout )
  {
    alarm_timeout=0;
    cbk_alarm();
  }
}

void setAlarm( unsigned long milliseconds )
{
  alarm_timeout=milliseconds;
  alarm_ticks=0;
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
const int ROTATION_TABLE[] = {  0, -1, 1, 0, 1, 0,  0, -1,
                               -1,  0, 0, 1, 0, 1, -1,  0 };
void isr_rotaryupdated(){
  int sample = PIND & 12;
  rotary_quarters += ROTATION_TABLE[ sample | rotary_last_graycode ] ;
  rotary_last_graycode = sample >> 2;
  interrupt_count++;
  setAlarm( ROTARY_TIMEOUT );
}

void setup() {
  // LED
  pinMode(PIN_NOT_RED, OUTPUT);

  // ROTARY
  pinMode(PIN_ROTARY_A,INPUT);
  pinMode(PIN_ROTARY_B,INPUT);
  
  attachInterrupt( digitalPinToInterrupt( PIN_ROTARY_A ),
                   isr_rotaryupdated,
                   CHANGE );
  attachInterrupt( digitalPinToInterrupt( PIN_ROTARY_B ),
                   isr_rotaryupdated,
                   CHANGE );
  rotary_last_graycode = ( PIND & 12 ) >>2;

  // Alarms
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
  TCCR1B |= _BV(WGM12);

  // Serial
  Serial.begin(9600);
}

void loop() 
{
}

