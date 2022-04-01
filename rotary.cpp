#include "arduino.h"
#include "rotary.h" 

#ifndef ESP32
// ESP32 can trigger interrupts from any pin Uno only has a couple pins that can trigger interrupts
#if ( PIN_ROTARY_A != 2 )
#warning PIN_ROTARY_A is not 2. I hope you used a pin with interrupt support.
#endif
#if ( PIN_ROTARY_B != 3 )
#warning PIN_ROTARY_B is not 3. I hope you used a pin with interrupt support.
#endif
#endif

#if ( PIN_ROTARY_A == PIN_ROTARY_B - 1 )
#define ROTARY_SHIFT PIN_ROTARY_A
#elif ( PIN_ROTARY_B == PIN_ROTARY_A - 1 )
#define ROTARY_SHIFT PIN_ROTARY_B
#else
#error PIN_ROTARY_A and _B must be adjacent.
#endif

#ifndef ESP32
// On ESP32 this is an attribute needed for ISRs; it's null in Arduino
#define IRAM_ATTR
#endif

// On ESP32, attachInterrupt just takes the pin number, but in Uno you need to call digitalPinToInterrupt
#ifdef ESP32
#define PIN_TO_INTERRUPT(x) x
#else
#define PIN_TO_INTERRUPT(x) digitalPinToInterrupt(x)
#endif

// On ESP32, you need to call REG_READ to get the input port register; not so on Uno
#ifdef ESP32
#define PIND REG_READ(GPIO_IN_REG)
#endif

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
  setRGB( RGB_BLACK );
}

////////////////////////////////////////////////////////////////////////////////
// Alarm service
// Use setAlarm to ask for a callback to fire after n milliseconds. There is
// only one alarm, and a call to setAlarm before the alarm has expired results
// in the old alarm being canceled and the new alarm set.
////////////////////////////////////////////////////////////////////////////////
#ifdef ESP32
hw_timer_t * timer = NULL;
#endif
volatile static unsigned long alarm_ticks = 0;
static unsigned long alarm_timeout = 0;
static void (*alarm_callback)(void);

#ifdef ESP32
void IRAM_ATTR isr_1ms(void)
#else
ISR(TIMER0_COMPA_vect) 
#endif
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
#ifdef ESP32
  timer=timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &isr_1ms, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);
#else
  OCR0A = 0xAF; // Compare Register
  TIMSK0 |= _BV(OCIE0A); // Mask
  TCCR1B |= _BV(WGM12);  // I think this sets CTC mode
#endif
}

void IRAM_ATTR isr_rotaryupdated();

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
  pinMode(PIN_ROTARY_A, INPUT_PULLUP);
  pinMode(PIN_ROTARY_B, INPUT_PULLUP);
  attachInterrupt( PIN_TO_INTERRUPT( PIN_ROTARY_A ),
                   isr_rotaryupdated,
                   CHANGE );
  attachInterrupt( PIN_TO_INTERRUPT( PIN_ROTARY_B ),
                   isr_rotaryupdated,
                   CHANGE );
  rotary_last_graycode = (PIND & (3 << ROTARY_SHIFT)) >> ROTARY_SHIFT;
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

void IRAM_ATTR isr_rotaryupdated() 
{
  int sample = ( PIND & (3<<ROTARY_SHIFT)) >> (ROTARY_SHIFT-2);
  rotary_quarters = ( rotary_quarters + 
                      ROTATION_TABLE[ sample | rotary_last_graycode ] ) % ( ROTARY_MODULO << 2 );
  rotary_last_graycode = sample >> 2;
  setAlarm( ROTARY_TIMEOUT, cbk_rotary_quiescent );
}
