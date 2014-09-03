Nucleus
=======
[![Build Status](https://travis-ci.org/AlexAltea/nucleus.svg)](https://travis-ci.org/AlexAltea/nucleus)

This is just a private project about PlayStation 3 emulation. Some parts are based in RPCS3 code, where I have been working on since 2013, while others have been written from scratch to match my own idea of a *well designed emulator*.

Please, understand that I'm not interested on providing support, releases, or attention to any comments, issues or pull requests. This strict policy may change in the future though.

### Limitations
* __Memory__: Nucleus emulates the PS3 user-mode environment, which uses 32-bit addresses. This reflects on the CPU / Memory related code, and could cause issues if you use it for other platforms, or for designing a low-level PS3 emulator.
* __Portability__: Nucleus assumes that the host system runs on a low endian CPU, with 64-bit addresses.
