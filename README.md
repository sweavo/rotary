# Rotary encoder

This project drives the spark fun RGB rotary encoder with pushbutton and RGB LED.

It's currently tested on Arduino Uno and ESP32


## Install:

* Name the extracted/cloned folder "rotary" and put it in your `Arduino/libraries` folder.
* Restart Arduino
* `File`->`Examples`->`Rotary` to get started.

## REFERENCE:
  
http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6HSPEC.pdf
http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6H.pdf

## Wiring

The pin names given in the library and this documentation are defined in `Pin Definitions` in `rotary.h`

 ![](/rotary%20basic_bb.svg)
  
### Encoder:

`ROTARY_PIN_A` and `ROTARY_PIN_B` should both be interrupt pins (e.g. 2 and 3 on an Uno) and must be adjacent (e.g. use 12 and 13 on the ESP32).

You don't need pullup resistors for the example because it turns on the internal pullups.

Connect
* Rotary pin C directly to GND
* Rotary pin A to `ROTARY_PIN_A`.
* Rotary pin B to `ROTARY_PIN_B`.

### LEDs:

* Connect R,G,B (pins 1,2,4 of the Rotary package) through 330Ohm current-limiting resistors to `PIN_NOT_RED`, `PIN_NOT_GREEN`, `PIN_NOT_BLUE` on Arduino.
* Connect Rotary package pin 5 (+5v) to +5v. 

The LEDs have not been tested on ESP32 but should be target-independent.

### Push button:
Pin 3 of the rotary package, the push button, connects to `PIN_ROTARY_PUSH` with a 10k pulldown resistor

## Apologies

* I haven't read the API guidelines yet. I expect I have committed crimes here.
* The library uses quite a lot of resources two interrupt pins AND a timer ISR). But it's light on compute cycles at runtime.

