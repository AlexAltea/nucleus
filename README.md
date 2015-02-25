Nucleus
=======
[![Build Status](https://travis-ci.org/AlexAltea/nucleus.svg)](https://travis-ci.org/AlexAltea/nucleus)
[![Last Release](https://img.shields.io/badge/version-0.0.4-brightgreen.svg?style=flat)](https://github.com/AlexAltea/nucleus/releases)

Created by Alexandro Sánchez Bach.

A PlayStation 3 emulator for Windows / Linux / Mac OS released under GPLv2 license.

### Dependencies
You need to provide the PlayStation 3 firmware keys by placing a file named `keys.xml` next to the Nucleus executable. This file has to met the requirements stated at [docs/keys.md](https://github.com/AlexAltea/nucleus/blob/master/docs/keys.md). Instructions about how to extract the keys and generate the file will be provided as well. Extra steps:
* __Windows__: Download the *glew32.dll* library from the [GLEW](http://glew.sourceforge.net/index.html) website and place it next to the Nucleus executable. Future packages provided at the Nucleus website will include all necessary dependencies.

### Building
* __Windows__: Install [GLEW](http://glew.sourceforge.net/install.html), [CMake](http://www.cmake.org/download/), [Python](https://www.python.org/downloads/) and [Visual Studio 2013](http://www.visualstudio.com/). Run `tools/nucleus-init.py` and build LLVM manually with the generated solution file. Then open `nucleus.sln` and press *Build* > *Rebuild solution*.
* __Linux__: Follow the `.travis.yml` file.

### Limitations
* __Memory__: Nucleus emulates the PS3 user-mode environment, which uses 32-bit addresses. This reflects on the CPU / Memory related code, and could cause issues if you use it for other platforms, or for designing a low-level PS3 emulator.
* __Portability__: Nucleus assumes that the host system runs on a low endian CPU, with 64-bit addresses.

### Disclaimer
The goal of this project is to experiment, research, and educate on the topic of emulation of modern devices and operating systems. **It is not for enabling illegal activity**. All information is obtained via reverse engineering of legally purchased devices and games and information made public on the Internet.
