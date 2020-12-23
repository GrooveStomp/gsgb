# gsgb

## Motivation
A GameBoy emulator!

The initial motivation for this project was a step-up from my CHIP-8 emulator.
Subsequently I saw it as an opportunity to refresh my C++ skills.

## Caveats
Only tested in Linux.
Currently a work in progress. See `grep -r TODO *`.

Prior to 2020-12-23 the license for this project was CC-BY 4.0. If you were making use of this project by that license then you can continue to do so.
However, if you've stumbled upon this project on 2020-12-23 or later then you must abide by the new license.

## Dependencies

### Executable
- SDL2
- C++ Compiler (only tested with GNU g++)

### Documentation
- doxygen

## Build

    # Build debug executable
    $ make debug

    # Build release executable
    $ make release

    # Build html documentation
    $ make docs
