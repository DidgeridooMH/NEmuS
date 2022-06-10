# NEmuS
NEmuS is an NES emulator written in C++ using the Qt libraries. Cross compatibilty is a main focus during the development and no platform specific code should be pushed without OS guards and sufficient workarounds for other platforms. Cycle specific timings isn't as much of a priority as getting more game support for the project. Any who wish to use, copy, or modify any of this project can do so freely (check LICENSE for more details).

<p align="center">
 <img src="https://github.com/didgeridoomh/nemus/raw/master/resources/nes_mario.gif" alt="mario" width="350px">
</p>

## Building

### Setting Up Libraries

- Add the directories "include" and "lib" to the src/ directory. 
- Then, put the corresponding Qt .lib files into a folder labeled x64. (The destination should look like src/lib/x64/{Library Files})
- Inside the include folder add header files for Qt.

### Meson building

TODO

### Contribution
A contribution guide will be coming soon...

## Housekeeping

- [ ] Replace all include guards with `#pragma once`.
- [ ] Restyle all code files.
- [ ] Replace all headers with `.hpp`.
- [ ] Make more use of smart pointers.
- [ ] Better separate code and data.
- [ ] Redo README
- [ ] Constify member functions that don't alter components.
- [ ] Simplify namespaces in `.cpp` files.
- [ ] Replace vague types with explicits `uint32_t, uint8_t`
