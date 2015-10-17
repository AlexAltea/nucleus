/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "nucleus.h"
#include "nucleus/core/config.h"
#include "nucleus/debugger/debugger.h"
#include "nucleus/emulator.h"
#include "nucleus/ui/ui.h"

#include <iostream>

void nucleusConfigure(int argc, char **argv)
{
    // Configure emulator
    config.parseArguments(argc, argv);
}

int nucleusInitialize(int argc, char **argv)
{
    if (argc <= 1) {
        std::cout
            << "Nucleus v0.0.4: A PlayStation 3 emulator.\n"
            << "Usage: nucleus [arguments] path/to/executable.ppu.self\n"
            << "Arguments:\n"
            << "  --console      Avoids the Nucleus UI window, disabling GPU backends.\n"
            << "  --debugger     Create a Nerve backend debugging server.\n"
            << "                 More information at: http://alexaltea.github.io/nerve/ \n"
            << std::endl;
    }

    // Start debugger
    if (config.debugger) {
        debugger.init();
        std::cerr << "Debugger listening on 127.0.0.1:8000" << std::endl;
    }

    // Start UI if console-only mode is disabled
    if (!config.console) {
        ui.init();
    }

    // Start emulator
    if (!config.boot.empty()) {
        nucleus.load(config.boot);
        nucleus.run();
        nucleus.idle();
    }

    return 0;
}

/**
 * Events
 */
void nucleusOnResize(unsigned int w, unsigned int h, unsigned int dpi, unsigned int hz)
{
    ui.surfaceWidth = w;
    ui.surfaceHeight = h;
    ui.surfaceDpi = dpi;
    ui.surfaceHz = hz;
    ui.surfaceProportion = float(w) / float(h);
    ui.surfaceChanged = true;
}
