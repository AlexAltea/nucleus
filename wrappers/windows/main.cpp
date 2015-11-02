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
        Window window("Nucleus", 960, 544);

        auto thread = std::thread([&]{
            nucleusPrepare(window.hdc);
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
