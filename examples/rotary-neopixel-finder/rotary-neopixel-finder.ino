/*
   This sketch uses the rotary encoder to help you figure out how to segment your neopixel strip.

   It blinks one LED. Turn the encoder to change which LED is blinking. Push the button to show 
   you (in the serial monitor) the index of that LED.
*/
#include <Adafruit_NeoPixel.h>
#include <rotary.h>

#define DELAY_CONST 200

#define NEO_PIN 4

#define NUMPIXELS 300

Adafruit_NeoPixel pixels = Adafruit_NeoPixel( NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);

uint32_t now;
uint32_t last = 0;

static uint16_t pix_index = 0;

static bool led_on = false;

// Callback when the rotary has moved one or more clicks.
// change this for your application
void cbk_rotarychange( uint16_t pos )
{
  last=now;  // Effective critical section
  pixels.setPixelColor( pix_index, pixels.Color(0, 0, 0));
  pix_index = pos;
  led_on=false;
  last=now-DELAY_CONST;
}

void setup() {

  pixels.begin();
  pixels.show();

  // Start comms
  Serial.begin(9600);

  // ROTARY
  initRGB();
  initRotary( cbk_rotarychange );
}

void loop() {
  static bool button_down = false;
  now = millis();

  if ( now - last > DELAY_CONST )
  {
    last = now;
    if (led_on) {
      pixels.setPixelColor( pix_index, pixels.Color(0, 0, 0));
    } else {
      pixels.setPixelColor( pix_index, pixels.Color(50, 25, 12));
    }
    led_on = !led_on;
    pixels.show();
  }
  if ( digitalRead( PIN_ROTARY_PUSH ) ^ button_down )
  {
    button_down = !button_down;
    if ( button_down ) {
      Serial.println(pix_index);
    }
  }
}

