/*  # Rotary encoder
 *   
 *  Demonstrate the rotary library.
*/ 
#include "rotary.h"

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

