/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "window.h"

#include "nucleus/core/config.h"
#include "nucleus/nucleus.h"

#include <thread>

int main(int argc, char **argv)
{
    // Load configuration
    nucleusConfigure(argc, argv);

    // Using UI
    if (!config.console) {
        unsigned int width = 960;
        unsigned int height = 544;
        Window window("Nucleus", width, height);

        nucleusPrepare(window.hwnd, window.hdc, width, height);
        auto thread = std::thread([&]{
            nucleusInitialize(argc, argv);
        });

        window.loop();
    }

    // Using console
    else {
        nucleusInitialize(argc, argv);
    }

    return 0;
}
