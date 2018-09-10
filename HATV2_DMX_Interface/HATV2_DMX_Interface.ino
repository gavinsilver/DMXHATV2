/*
 * DMX Interface Code - Runs on the DMX Nano CPU U5 on board
 * 
 * Now on github.com/gavinsilver/DMXHATV2
 * 
 * V1.0 - First Version
 * V1.1 - Added Debug and fake DMX for testing
 */

#include <Conceptinetics.h>

//
// The slave device will use a block of 10 channels counting from
// its start address.
//
// If the start address is for example 56, then the channels kept
// by the dmx_slave object is channel 56-66
//
#define DMX_SLAVE_CHANNELS  5 

// Configure a DMX slave controller
DMX_Slave dmx_slave ( DMX_SLAVE_CHANNELS );

#define DEBUG_PIN A7    // Set debug Pin when held low we go into debug mode
#define FAKE_DMX A6     // Set Pin to use to swith on fake DMX
boolean debugon=false;  // debug Mode
boolean fakedmx=false;  // create fake DMX data for testing 
int fakedmxi=0;         // counter used to cycle through colours

const int ledPinRed = 3;
const int ledPinGreen = 5;
const int ledPinBlue = 6;
volatile byte redVal = 0;
volatile byte greenVal = 0;
volatile byte blueVal = 0;
volatile byte dimVal = 0;
volatile byte strobeVal = 0;

int dmx_add = 0;  // 0 - 511
int dmx_mode = 0; // 0-7
int val = 0;      // Temp variable used to read input

volatile byte DMXval = 0;

// the setup routine runs once when you press reset:
void setup() {    

  // SPI Setup have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);
   
  // Setup pins to read DMX address and DMX_Mode.  Becuse of the internal pullup resistor 
  // when the swith id off the input will be HIGH and when the switch is on the input will
  // be LOW
  // read in DMX address (9bits)
  
  pinMode(2, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT); // Note A6 & A7 Analog only and need external pullup resistor
  pinMode(A7, INPUT); // Also can only be read with AnalogRead!

  // read in DMX address (9bits)
  dmx_add = 0;
  val = digitalRead(2); 
  if (val==LOW) {
      dmx_add |= 1;
    };
  val = digitalRead(4); 
  if (val==LOW) {
      dmx_add |= 1<<1;
    };
  val = digitalRead(7); 
  if (val==LOW) {
      dmx_add |= 1<<2;
    };
  val = digitalRead(8); 
  if (val==LOW) {
      dmx_add |= 1<<3;
    };
  val = digitalRead(9); 
  if (val==LOW) {
      dmx_add |= 1<<4;
    };
  val = digitalRead(A0); 
  if (val==LOW) {
      dmx_add |= 1<<5;
    };
  val = digitalRead(A1); 
  if (val==LOW) {
      dmx_add |= 1<<6;
    };
  val = digitalRead(A2); 
  if (val==LOW) {
      dmx_add |= 1<<7;
    };
  val = digitalRead(A3); 
  if (val==LOW) {
      dmx_add |= 1<<8;
    };

    // Read debug Mode
  val = analogRead(A7); 
  if (val<500) {
      debugon = true;
    }
    else {
      debugon = false;
    }
  
  // Read fake DMX Mode
  val = analogRead(A6); 
  if (val<=500) {
      fakedmx = true;
    }
    else {
      fakedmx = false;
    }
    
  // Enable DMX slave interface and start recording
  // DMX data
  dmx_slave.enable ();  
  
  // Set start address to 1, this is also the default setting
  // You can change this address at any time during the program
  dmx_slave.setStartAddress (dmx_add);
  
  // Set led pin as output pin
  pinMode ( ledPinRed, OUTPUT );
  pinMode ( ledPinGreen, OUTPUT );
  pinMode ( ledPinBlue, OUTPUT );

  // light up LED's if we are in Debug Mode
  if (debugon == true) {
      // turn on each RGB led on one at a time to test if we are in debug mode
      analogWrite(ledPinRed,255);
      delay(500);
      analogWrite(ledPinGreen,255);
      delay(500);
      analogWrite(ledPinBlue,255);
      delay(500);
      // now turn off all the leds,
      analogWrite(ledPinRed,0);
      analogWrite(ledPinGreen,0);
      analogWrite(ledPinBlue,0);
  }
}

// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;
  if (c == 1) {
    SPDR = redVal;
  }
  if (c == 2) {
    SPDR = greenVal;
  }
  if (c == 3) {
    SPDR = blueVal;
  }
  if (c == 4) {
    SPDR = dimVal;
  }
  if (c == 5) {
    SPDR = strobeVal;
  }

}  // end of interrupt service routine (ISR) SPI_STC_vect

// the loop routine runs over and over again forever:
void loop() 
{
  //
  // EXAMPLE DESCRIPTION
  //

  if (fakedmx == true){
    // Fake the DMX output to Flashing Light Pink
    fakedmxi++;
    if (fakedmxi >= 101) fakedmxi=0; // reset back to start of sequence
    if (fakedmxi <= 25) {
      // RED
      redVal = 255;
      greenVal = 0;
      blueVal = 0;
      dimVal = 255;
      strobeVal = 0;
    }
    if ((fakedmxi > 25) and (fakedmxi <= 50 )) {
      // GREEN
      redVal = 0;
      greenVal = 255;
      blueVal = 0;
      dimVal = 255;
      strobeVal = 0;
    }
    if ((fakedmxi > 50) and (fakedmxi <= 75 )) {
      // BLUE
      redVal = 0;
      greenVal = 0;
      blueVal = 255;
      dimVal = 255;
      strobeVal = 0;
    }
    if ((fakedmxi > 75) and (fakedmxi <= 100 )) {
      // WHITE
      redVal = 255;
      greenVal = 255;
      blueVal = 255;
      dimVal = 255;
      strobeVal = 0;
    }
  }
  else {
    // Read the data from the DMX Interface
    // Uses PWM on 3 pins to do RGB in a very crude way.
    // NOTE:
    // getChannelValue is relative to the configured startaddress
    redVal = dmx_slave.getChannelValue(1);
    analogWrite(ledPinRed,redVal);
    greenVal = dmx_slave.getChannelValue(2);
    analogWrite(ledPinGreen,greenVal);
    blueVal = dmx_slave.getChannelValue(3);
    analogWrite(ledPinBlue,blueVal);
    dimVal = dmx_slave.getChannelValue(4);
    strobeVal = dmx_slave.getChannelValue(5); 
  }

  delay(20);
  
}
