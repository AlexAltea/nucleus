/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "emulator.h"
#include <iostream>

// Nucleus global emulator
Emulator nucleus;

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout << "Nucleus v0.0.1: A Playstation 3 emulator and debugger" << std::endl;
        std::cout << "Usage: nucleus [arguments] path/to/executable.ppu.elf" << std::endl;
        std::cout << "Arguments: (none available)" << std::endl;
    }

    else {
        // Configure emulator
        std::string elfPath = argv[argc-1];

        // Start emulator
        if (!nucleus.load(elfPath)) {
            std::cerr << "Could not load the given executable." << std::endl;
            return 1;
        }

        nucleus.run();
    }
    return 0;
}
