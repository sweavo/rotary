/*
   Rotary demo
*/ 
#include "rotary.h"

#define DELAY_CONST 200

// Callback when the rotary has moved one or more clicks.
// change this for your application
void cbk_rotarychange( uint16_t pos )
{
  Serial.print("\nRotary is at position ");
  Serial.println(pos);
  setRGB( pos % 8 );
}

void setup() {

  // Start comms
  Serial.begin(115200);
  
  // ROTARY
#ifndef ESP32
  initRGB();
#endif
  initRotary( cbk_rotarychange );  
}

void loop() {
  delay( DELAY_CONST );
#ifndef ESP32
  if ( digitalRead( PIN_ROTARY_PUSH ) )
    {
    Serial.println("Push");
  }
#endif
}

