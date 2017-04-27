////////////////////////////////////////////////////////////////////////////
//
// A simple CW Blinker (aka LED Beacon)
// by SV1DJG (c.2016)
//
// This is a ultra simple blinker in CW for the 
// digispark mini board (attiny85).
// change the led port if using a different
// board (e.g. Arduino)
//
// Just set your message into the msg array and you 
// are ready to go.Make sure to use ONLY the set
// of the supported characters.
// the supported set of characters is capital 
// letters,numbers and a few symbols.
//
// You can also modify the transmission speed by 
// changing the speedWPM variable.
//
// HAVE FUN!!
// 
// This code is provided as-is and may contain bugs!
//
// (codesize:1132 bytes + message length)
//
// Copyright 2016, Nick Tsakonas,SV1DJG
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////

#include <avr/pgmspace.h>



// this is the port where the LED is connected (on digispark this is pin 1)
#define ledPort 1

// this defines the keying level and provides a quick way to change the keying method. 
// when we use a LED we need HIGH to turn the LED on and LOW to turn the LED off
// but if we select to connect an external switching hardware , this may need to be reversed
#define MORSE_KEY_DOWN  HIGH
#define MORSE_KEY_UP    LOW

// delay before repeating the message (in Ms)
#define repeatDelayMs 3000

// transmission speed (in words per minute)
#define speedWPM 12



// element duration is according to Farnsworth Technique
// see : http://www.arrl.org/files/file/Technology/x9004008.pdf
#define dotDuration  1200 / speedWPM          // element duration in milliseconds
#define dashDuration        dotDuration * 3

#define interSymbolDuration dotDuration
#define interCharDuration   dotDuration * 3
#define interWordDuration   dotDuration * 7

//////////////////////////////////////////////////////////////////////////////////////////////
//
// this is the beacon message (only capital letters, numbers and a few symbols - see below)
//
//////////////////////////////////////////////////////////////////////////////////////////////
char const msg[] = "TEST DE SV1DJG";

// Morse code table
// ----------------
// Morse code elements are variable length and transmitted Msb first.
// we need to know the length of each element so we save
// it along with the element as the first part of the pair in the table.
//
// Each element is made up from 1's and 0's where:
// 1 means DOT
// 0 mean DASH
// for example, A is DOT-DASH which is translated to 10.
// B is DASH-DOT-DOT-DOT which is translated to 0111 etc.
// these are encoded as binary values and paired with their length
// for example 
//    A -> 2,B10 
//    B -> 4,B0111
// etc
//
// table of elements for Morse cose numbers do not follow this as all elements are 5 digits long
// (so we save space in the symbol table)
//
byte const letters[] PROGMEM =
{ 
  2,B10,   // A
  4,B0111, // B
  4,B0101, // C
  3,B011,  // D
  1,B1,    // E
  4,B1101, // F
  3,B001,  // G
  4,B1111, // H
  2,B11,   // I
  4,B1000, // J
  3,B010,  // K
  4,B1011, // L
  2,B00,   // M
  2,B01,   // N
  3,B000,  // O
  4,B1001, // P
  4,B0010, // Q
  3,B101,  // R
  3,B111,  // S
  1,B0,    // T
  3,B110,  // U
  4,B1110, // V
  3,B100,  // W
  4,B0110, // X
  4,B0100, // Y
  4,B0011, // Z
};

byte const numbers[] PROGMEM =
{ 
  B00000, // 0
  B10000, // 1
  B11000, // 2
  B11100, // 3
  B11110, // 4
  B11111, // 5
  B01111, // 6
  B00111, // 7
  B00011, // 8
  B00001, // 9
};


byte const symbols[] PROGMEM =
{ 
  6,B101010,  // Full-stop (period)
  6,B001100,  // Comma
  6,B110011,  // Question mark (query)
  5,B01101 ,  // Slash
  5,B01110 ,  // Equals sign
};

// transmits a morse element (either a dot or a dash)
void sendSymbol(boolean sendDot)
{
  digitalWrite(ledPort, MORSE_KEY_DOWN);
  delay(sendDot ? dotDuration : dashDuration); 
  digitalWrite(ledPort, MORSE_KEY_UP);
}

// transmits an ASCII character in Morse Code
void sendCharacter(char c)
{
  byte numberOfBits = 0;
  byte elementCode  = 0;
  
  if (c >= 'A' && c <='Z')
  {
    int elementIndex = (int)(c-'A');
    numberOfBits = pgm_read_byte(&letters[elementIndex * 2]);
    elementCode  = pgm_read_byte(&letters[elementIndex * 2 + 1]);
  }
  else if (c >= '0' && c <='9')
  {
    int elementIndex = (int)(c-'0');
    numberOfBits = 5;
    elementCode  = pgm_read_byte(&numbers[elementIndex]);
  }
  else if (c == ' ')
  {
    // adjust delay because we have already "waited" for a character separation duration
    // NOTE:this delay will not be 100% accurate if the first character in the message is SPACE
    delay(interWordDuration-interCharDuration); 
  }
  else
  {
    int elementIndex = -1;

    if (c == '.')      elementIndex = 0;
    else if (c == ',') elementIndex = 1;
    else if (c == '?') elementIndex = 2;
    else if (c == '/') elementIndex = 3;
    else if (c == '=') elementIndex = 4;
   
    if (elementIndex != -1)
    {
      numberOfBits = pgm_read_byte(&symbols[elementIndex * 2]);
      elementCode  = pgm_read_byte(&symbols[elementIndex * 2 + 1]);
    }
  }
  
  
  if (numberOfBits > 0)
  {
    byte mask = 0x01 << (numberOfBits - 1);
    
    while (numberOfBits > 0 )
    {
      
      sendSymbol((elementCode & mask) == mask);
      delay(interSymbolDuration);
      mask = mask >> 1;
      --numberOfBits;
    }
     // adjust delay because we have already "waited" for a dot duration
     delay(interCharDuration - interSymbolDuration);
   }  
   
}

// transmits an ASCII message in Morse Code
void sendMessage(const char* msg)
{
  while (*msg)
    sendCharacter(*msg++);
 
}


void setup() 
{                
  // initialize the digital pin as an output.
  pinMode(ledPort, OUTPUT); 
}

// transmits a message over and over again forever:
void loop() 
{
  // transmit message... 
  sendMessage(msg);
  // wait and repeat
  delay(repeatDelayMs);               
}

