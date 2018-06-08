# DMXHATV2

##DMX Interface to NeoPixel Strip

DMX to Neopixel board. Voltage Isolated DMX interface with separate Arduino Nano CPU’s for DMX and Neopixel driver

If you have tried driving NeoPixel strips from an Arduino and then tried to mix this with DMX then you 
may have hit the problem that the NeoPixel strip requires interrupts to be turned off when it is updated
and when this happens the DMX interface stops receiving data.  Even with small strips you may need to 
wait 50ms before the DMX data recovers.

This project is an attempt to get around this by have two Arduino Nano boards one dedicated to DMX
and the other to controlling the LED strip.  These subsystems communicate over an SPI interface.  The
DMX interface is also fully isolated which most of the cheaper DMX interfaces are not.

The PCB has been produced using KiCAD
The Arduino Code is written in the Arduino IDE

Gvain Silver - 8th June 2018
