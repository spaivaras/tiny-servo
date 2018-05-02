# TinyServo
This is the most simple solution for controlling ESC and other devices working with PPM (pwm in public). With no overcomplicated and inflated arduino code. The chip is controlled with 2 buttons, 1 to increase the pulse width (throttle __+__) another is to decrease it (throttle __-__). Settings are stored into EEPROM to reach the same pulse width on each boot (with slow rising start). To save EEPROM as much as possible, writes are delayed and done only on value change.

### Usage
* While holding __INC__ button on boot this will rise pulse to max (2ms) this is useful for calibrating ESC's and what not
* While holding __DEC__ button on boot this will ignore EEPROM setting and start at lowest setting (1ms)
* While running __INC__ increases the pulse with set step
* While running __DEC__ decreases the pulse with set step
* EEPROM setting is saved only if stored and set values differ and ~3 seconds passed without no changes to value
* On boot after ~10 sec the pulse width will increase slowly by steps to EEPROM stored value

### Hardware target
Code is designed around attiny13 with single timer and no external components. CPU declarations, clock frequency and fused bits are stored in makefile.

### Make targets
* __all__: build objects
* __upload__: create hex and write to attiny using avrdude
* __fuse__: Write fuse-bits using avrdude (Refer to Makefile for actual fuse-bits)
* __clean__: Remove intermediate files (object, maps, hex, etc.)

### Pinout
* __PB0__:  INC button (internal pullup, active __low__)
* __PB4__: DEC button (internal pullup, active __low__)
* __PB3__: PPM output

### Liability
This project was built for my own needs. I won't be held liable for any physical or mental damage, of any kind, using this work. There is no warranty that this project will not blow your house up and leave your children orphans. Use on your own risk. Use common sense: always disconnect any objects that can do any harm while testing this code.

### Copyleft
GNU AGPL v3  - Aivaras Spaicys (See LICENSE for full license text)

