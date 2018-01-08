# SacroSeqMk1
Software for a DIY midi sequencer loosely inspired by the Intellijel Metropolis.

## Current status : On hold - very much unfinished
I ran into a hardware problem and I don't currently have access to the tools to fix it, and I am not terribly motivated to work on software for a project for which the hardware is broken.

## Hardware
This software is written for a piece of [teensy3.1](https://www.pjrc.com/teensy/teensy31.html)-based custom hardware, design files or schematics for which may or may not appear on here at some point (it's not that I don't wish to share them, it's that they don't exist).

### Quick-and-dirty BOM
* 1x [teensy3.1](https://www.pjrc.com/teensy/teensy31.html) (ARM Cortex-M4 based microcontroller board)
* 12x momentary push switches
* 8x rotary encoders with integrated momentary push switch (30 detent, I think?)
* 8x LEDs (with current-limiting resistors of some fitting value)
* 1x 4-digit 7-segment display.
* 1x MIDI-out (combination of DIN-connector, opto-coupler and a couple resistors)
* 1x USB-UART mostly for debugging purposes

## Tools
* [platformio](http://www.platformio.org) (core) for building and programming of the hardware. This may change in the future.
* [mbed](http://os.mbed.org) OS/HAL (whatever you wish to call it) to save me the effort of fiddling with bits and bytes.
* [clang extra tools](http://clang.llvm.org/extra/) for various extra developer tools (e.g. formatting, autocomplete, static analysis)
* [spacemacs](http://www.spacemacs.org) for writing code (though, no spacemacs-specific files will be added to the project at any point)
* [git](http://www.git-scm.com) hosted on [github](http://www.github.com)

## License
Software and any other design files on this repository are licensed under GPLv3.

## Who built this diabolical contraption?
Me. [@apvanzanten](http://www.github.com/apvanzanten).
