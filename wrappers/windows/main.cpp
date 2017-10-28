/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "window.h"

// Nucleus
#include "nucleus/nucleus.h"
#include "nucleus/core/config.h"

#include <thread>

int main(int argc, char **argv) {
    // Load configuration
    nucleusConfigure(argc, argv);

    // Using UI
    if (!config.console) {
        unsigned int width = 960;
        unsigned int height = 544;
        Window window("Nucleus", width, height);

        nucleusInitialize(window.hwnd, window.hdc, width, height);
        auto thread = std::thread([]{
            nucleusInitialize();
        });

        window.loop();
    }

    // Using console
    else {
        nucleusInitialize();
    }

    return 0;
}
