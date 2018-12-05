# Rotary encoder

This project's purpose is to drive the spark fun RGB rotary encoder only.

## Install:

* Name the extracted/cloned folder "rotary" and put it in your `Arduino/libraries` folder.
* Restart Arduino
* `File`->`Examples`->`Rotary` to get started.

## REFERENCE:
  
http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6HSPEC.pdf
http://cdn.sparkfun.com/datasheets/Components/Switches/EC12PLRGBSDVBF-D-25K-24-24C-6108-6H.pdf

## Wiring

The pin names given here are defined in `Pin Definitions` in `rotary.h`
  
### Encoder:

`ROTARY_PIN_A` and `ROTARY_PIN_B` should both be interrupt pins (e.g. 2 and 3 on an Uno).

You don't need pullup resistors for the example because it turns on the internal pullups.

Connect
* Rotary pin C directly to GND
* Rotary pin A to `ROTARY_PIN_A`.
* Rotary pin B to `ROTARY_PIN_B`.

### LEDs:

* Connect R,G,B (pins 1,2,4 of the Rotary package) through 330Ohm current-limiting resistors to `PIN_NOT_RED`, `PIN_NOT_GREEN`, `PIN_NOT_BLUE` on Arduino.
* Connect Rotary package pin 5 (+5v) to +5v. 

### Push button:
Pin 3 of the rotary package, the push button, connects to `PIN_ROTARY_PUSH` with a 10k pulldown resistor

## Apologies

* I haven't read the API guidelines yet. I expect I have committed crimes here.
* The library uses quite a lot of resources (both interrupt pins AND a timer ISR). I welcome contributions that simplify it.
