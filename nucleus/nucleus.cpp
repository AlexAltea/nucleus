/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "config.h"
#include "emulator.h"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout
            << "Nucleus v0.0.1: A PlayStation 3 emulator.\n"
            << "Usage: nucleus [arguments] path/to/executable.ppu.self\n"
            << "Arguments:\n"
            << "  --debugger    Create a Nerve backend debugging server\n"
            << "                More information at: http://alexaltea.github.io/nerve/ \n"
            << std::endl;
    }

    else {
        // Configure emulator
        config.parseArguments(argc, argv);
        std::string elfPath = argv[argc-1];

        // Start emulator
        if (!nucleus.load(elfPath)) {
            std::cerr << "Could not load the given executable." << std::endl;
            return 1;
        }

        // Start debugger
        if (config.debugger) {
            std::cout << "Debugger listening on 127.0.0.1:8000" << std::endl;
        }

        nucleus.run();
        nucleus.idle();
    }

    return 0;
}
