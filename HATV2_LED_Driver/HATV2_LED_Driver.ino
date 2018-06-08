/*
 * HAT V2 Code to run on U5 CPU
 * 
 * Now on github.com/gavinsilver/DMXHATV2
 * 
 */

// SPI Library
#include <SPI.h>

// Neopixel Library
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 9

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

/// setup global variables for main code segment.

const int debugPin = 2; // Set debug Pin when held low we go into debug mode
const int ledPin = 7;   // debug  LED pin
boolean debugon=false;  // debug Mode

//long unsigned int time_debug = 2; // ms delay between strobes to turn on LED

long unsigned int millis_now =0;              // hold current millis value for this loop
long unsigned int DMX_read_interval = 3;     // interval in ms between reading DMX values 
long unsigned int DMX_last_check_millis = 0;  // value of millis last time we checked the DMX values 

     

byte redVal = 0;        // red value 0-255 (Channel 1)
byte dim_red = 0;       // after dimer value is applied
byte red_last =0;       // holds the laste value when checking the DMX to see if it has changed

byte greenVal = 0;      // green value 0-255 (Channel 2)
byte dim_green = 0;
byte green_last = 0;

byte blueVal = 0;       // blue value 0-255 (Channel 3)
byte dim_blue = 0;
byte blue_last = 0;

byte dim_val = 0;       // dimmer value 0 (off) to 255 (full) (Channel 4)
byte dim_val_last = 0;

byte strobe_val = 0;    // 000 - Solid On
                        // 001 - 50ms Strobe
                        // 002 - 254 - Linear between 50ms and 500ms
                        // 255 - 500ms Strobe
int strobe_val_last = 0;
long unsigned int strobe_ms = 30;                // Strobe transition time in Ms
const int strobe_min = 30;       // Minimum Strobe Transition in Ms
const int strobe_max = 500;     // Maximun Strobe Transition in Ms
boolean strobe_on = false;      // True if Strobe should be on
boolean strobe_change = false;  // True if Strobe state has changed
long unsigned int strobe_last_transition_millis; // what the millis function returned last time we changed strobe state

void setup() {

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // start serial port for debug
  // if debug enabled
  Serial.begin(115200);
  Serial.println("Starting ....");
    
  // Setup Debug Pin (D2) if D2 is held low then debug mode is enabled
  pinMode (ledPin, OUTPUT);
  pinMode(debugPin, INPUT_PULLUP);
  if (digitalRead(debugPin) == LOW) // see if we are in debug mode
    {
    debugon = true; // we are in debug Mode
    Serial.println("Debug is ON");
    Serial.println("LED test");
    // Startup Colour Test (in Debug Mode)
    colorWipe(strip.Color(255, 0, 0), 100); // Red
    colorWipe(strip.Color(0, 255, 0), 100); // Green
    colorWipe(strip.Color(0, 0, 255), 100); // Blue
    colorWipe(strip.Color(255, 255, 255), 100); // White
    colorWipe(strip.Color(0, 0, 0), 100);   // Off
    
    }
  else
    {
      Serial.println("Debug is OFF");
    }

  // Setup the SPI Interface
  digitalWrite(SS, HIGH);  // ensure SS stays high for now
  // Put SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.begin ();
  // Slow down the master a bit
  SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void loop() {
  
  if (digitalRead(debugPin) == LOW) // see if we are in debug mode
    {
      debugon = true; // we are in debug Mode
      Serial.println("Debug is ON");
    }
  else
    {
      debugon = false; // we are in debug Mode
      //Serial.println("Debug is OFF");
    }
  millis_now = millis();              // read current millis for this loop
  if (debugon) {
    Serial.print("Loop Milis_now = ");
    Serial.println(millis_now);
  }

  // see if we need to change strobe state
  // to do - add handelingin for millis wrap (after 9 hrs)
  if (strobe_val !=0) {
      if ((millis_now - strobe_last_transition_millis) > strobe_ms) {
        // code to change strobe state
        
        /* DEBUG
        if ((millis_now - strobe_last_transition_millis - strobe_ms) > time_debug){ // debug code to work out delay
          //digitalWrite(ledPin, HIGH);  // debug
        }
        else {
          //digitalWrite(ledPin, LOW);  // debug
        }*/
                
        if (strobe_on) {
          strobe_on = false;
          colorWipe(strip.Color(0, 0, 0), 0); // turn everything off
          strobe_last_transition_millis = millis_now;
        }
        else {
          strobe_on = true;
          colorWipe(strip.Color(dim_red, dim_green, dim_blue), 0); // turn everything back as it was
          strobe_last_transition_millis = millis_now;
        }
      }
  }
  
  else {
    // Code to force to turn off strobe.  This is to handle the case where the strobe goes from on to off.
    if (strobe_on) {
      strobe_on = false;      // Turn Strobe off
      strobe_change = false;  // we dont need to change
      // write data to LED strip
      colorWipe(strip.Color(dim_red, dim_green, dim_blue), 0); // make sure pixels are set to original values, no wipe.
    }  
  }
 
  if ((millis_now - DMX_last_check_millis) > DMX_read_interval) { //section reading DMX Values
        
    // Read the current values from the DMX CPU using SPI Interface
    if (debugon) {
      Serial.println("Read DMX over SPI");
    }
    // enable Slave Select
    digitalWrite(SS, LOW);
    
    transferAndWait(1);
    redVal = transferAndWait(2);
    greenVal = transferAndWait(3);
    blueVal = transferAndWait(4);
    dim_val = transferAndWait(5);
    strobe_val = transferAndWait(0);
    DMX_last_check_millis = millis_now; // record the last time we read the DMX interface
    digitalWrite(SS, HIGH);
    if (debugon) {
      Serial.print("Red Value =");
      Serial.println(redVal);
      Serial.print("Green Value =");
      Serial.println(greenVal);
      Serial.print("Blue Value =");
      Serial.println(blueVal);
      Serial.print("Dim Value =");
      Serial.println(dim_val);
      Serial.print("Strobe Value =");
      Serial.println(strobe_val);
      Serial.println("");
      delay(100);
    }
      
    // see if any DMX channel values have changed?
    if ((redVal != red_last) || (greenVal != green_last) || (blueVal != blue_last)|| (dim_val != dim_val_last) || (strobe_val != strobe_val_last)){
      // now calculate the dimmed value of RGB
      digitalWrite(ledPin, HIGH);  // debug
      dim_red = newmap(redVal, 0, 255, 0, dim_val); 
      dim_green = newmap(greenVal, 0, 255, 0, dim_val);
      dim_blue = newmap(blueVal, 0, 255, 0, dim_val);
  
      // now look at the Strobe value to see if it has changed
      if (strobe_val != strobe_val_last){
        // Strobe Value has changed
        if ((strobe_val == 0) ){  // Always On, i.e. no strobe efect
          // Settings for solid on;
          strobe_ms = strobe_max;
          strobe_on = false;
        }
        else {  // We are strobing
          // Calculating strobe timing
          strobe_ms = newmap(strobe_val, 1, 255, strobe_max, strobe_min); // Calculate strobe value in Ms where 
          // 0=off 1 = 500ms up to 255 = 50ms
        }
      }
      // write data to LED strip when a DMX valuse has changed (should we ignore changes to strobe?)
      colorWipe(strip.Color((int)dim_red, dim_green, dim_blue), 0); // something has changed so change pixels, no wipe.
  
      // Record previous DMX RGB, Dim & Strobe values
      red_last = redVal;
      green_last = greenVal;
      blue_last = blueVal;
      dim_val_last = dim_val;
      strobe_val_last = strobe_val;
    }

  }  // end of section reading DMX Values

   
} // end of void loop()

// FUNCTIONS

// Neopixel Strip, fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// SPI Transfer & Wait return the Byte
byte transferAndWait (const byte what)
{
  byte a = SPI.transfer (what);
  delayMicroseconds (20);
  return a;
} // end of transferAndWait

// New Map command supposed to give a smoother range (from http://forum.arduino.cc/index.php?topic=417690.30 )
long newmap(long x, long in_min, long in_max, long out_min, long out_max)
{
  if( x == in_max)
    return out_max;
  else if(out_min < out_max)
    return (x - in_min) * (out_max - out_min+1) / (in_max - in_min) + out_min;
  else
    return (x - in_min) * (out_max - out_min-1) / (in_max - in_min) + out_min;
}
