/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "window.h"
#include "window_opengl.h"

#include "nucleus/config.h"
#include "nucleus/nucleus.h"

#include <thread>

// Global Nucleus window
Window* window = nullptr;

int main(int argc, char **argv)
{
    // Load configuration
    nucleusConfigure(argc, argv);

    // Using UI
    if (!config.console) {
        // Initialize UI
        switch (config.gpuBackend) {
        case GPU_BACKEND_OPENGL:
            window = new WindowOpenGL("Nucleus", 960, 544);
            break;
        default:
            break;
        }

        new std::thread([&]{ nucleusInitialize(argc, argv); });
        window->loop();

        // Finalize UI
        delete window;
    }

    // Using console
    else {
        // Run Nucleus
        int result = nucleusInitialize(argc, argv);
    }

    return 0;
}
