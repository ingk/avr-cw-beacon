# avr-cw-beacon
A very simple CW (Morse code) beacon implemented on an AVR microcontroller (attiny, atmega)

This is a very simple toy-project made as an alternative to the LED blinker (aka tha "hello world" of Arduino). 
The project was initially made for the Digispark Tiny board [Link](http://digistump.com/products/1) which runs on the attiny85 mcu, but will happily run on an normal Arduino board if you just change the LED port.

At the moment the only thing it does is to turn on/off the on-board LED but by changing the port you can use it to switch on/off a CW transmitter (extra circuit may be required)

Just set your message into the msg array and you are ready to go. eg.
````
char const msg[] = "TEST DE SV1DJG";
````

NOTE: Make sure to use ONLY the set of the supported characters.

|TYPE | RANGE|
--------|-------
CAPITALS| A .. Z
NUMBERS | 0 .. 9
SYMBOLS | (.) PERIOD (,) COMMA (?) QUESTION MARK (/) SLASH (=) EQUALS

You can also modify the transmission speed by changing the speedWPM variable.

This code is provided as-is and may contain bugs!

(codesize:1132 bytes + message length)
