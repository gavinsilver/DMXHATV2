/*
 * DMX Interface Code - Runs on the DMX Nano CPU U5 on board
 * 
 * Now on github.com/gavinsilver/DMXHATV2
 * 
 * V1.0 - 
 */


#include <Conceptinetics.h>

//
// The slave device will use a block of 10 channels counting from
// its start address.
//
// If the start address is for example 56, then the channels kept
// by the dmx_slave object is channel 56-66
//
#define DMX_SLAVE_CHANNELS  512 

// Configure a DMX slave controller
DMX_Slave dmx_slave ( DMX_SLAVE_CHANNELS );

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
int jp3 = 0;      // JP3 0=Open 1=Closed
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
  pinMode(A7, INPUT);

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
  delay(20);
  
}
