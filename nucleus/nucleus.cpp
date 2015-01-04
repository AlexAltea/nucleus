/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "config.h"
#include "debugger.h"
#include "emulator.h"
#include "ui/ui.h"

#include <iostream>

int main(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout
            << "Nucleus v0.0.2: A PlayStation 3 emulator.\n"
            << "Usage: nucleus [arguments] path/to/executable.ppu.self\n"
            << "Arguments:\n"
            << "  --console     Avoids the Nucleus UI window, disabling GPU backends.\n"
            << "  --debugger    Create a Nerve backend debugging server.\n"
            << "                More information at: http://alexaltea.github.io/nerve/ \n"
            << std::endl;
    }

    else {
        // Configure emulator
        config.parseArguments(argc, argv);
        std::string elfPath = argv[argc-1];

        // Start UI if console-only mode is disabled
        if (!config.console) {
            ui.init();
        }

        // Start emulator
        if (!nucleus.load(elfPath)) {
            std::cerr << "Could not load the given executable." << std::endl;
            return 1;
        }

        // Start debugger
        if (config.debugger) {
            debugger.init();
            std::cerr << "Debugger listening on 127.0.0.1:8000" << std::endl;
        }

        nucleus.run();
        nucleus.idle();
    }

    return 0;
}
