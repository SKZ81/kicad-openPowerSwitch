# Protoyping openPowerSwitch

Please check : https://skz81.wordpress.com/ for more infos.

## Compilation

Two makefiles are provided. Makefile is a link to Makefile.standalone, which should match most installation.

For Arduino.mk users (I am !) the Makefile.arduino_mk file is provided (just edit this file and replace the symlink).

## ENV VARs that make stuff: 

You can define in make command environment the following variables :
F_CPU : AVR frequency
BAUD : UART baud rate

With Makefile.standalone, another variable can be defined :   
MCU : AVR chip name as passed to GCC's -mmcu= option

## Typical command line

~~~
make clean && F_CPU=16000000 BAUD=115200 MCU=atmega328p make
~~~

### Note :
 arduino.mk is intended to be used with (more or less) Arduino prototyping boards, then it has to be defined directly into the BOARD variable of this Makefile.
Anyway, I bet it is easy to add the parameters for a custom board into the local board.txt ( from `arduino-avr-core` package at least on Arch Linux distro)

### Note : 
Standalone makefile does not provide anyway to flash the .hex
I assume if you prefer using standalone Makefiles ; it means you're "grown up", AVR's Dude, so just use avrdude, as usual ^^


