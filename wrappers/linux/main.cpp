/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "window-wayland.h"
#include "window-xcb.h"
#include "window-xlib.h"

// Nucleus
#include "nucleus/nucleus.h"
#include "nucleus/core/config.h"

#include <cstring>
#include <thread>

enum WindowBackend {
    WINDOW_BACKEND_WAYLAND,
    WINDOW_BACKEND_XCB,
    WINDOW_BACKEND_XLIB,
};

int main(int argc, char **argv) {
    // Load configuration
    nucleusConfigure(argc, argv);

    // Using UI
    if (!config.console) {
        // Choose Window manager
        WindowBackend backend = WINDOW_BACKEND_XCB;
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "--wayland") == 0) {
                backend = WINDOW_BACKEND_WAYLAND;
                continue;
            }
            if (strcmp(argv[i], "--xcb") == 0) {
                backend = WINDOW_BACKEND_XCB;
                continue;
            }
            if (strcmp(argv[i], "--xlib") == 0) {
                backend = WINDOW_BACKEND_XLIB;
                continue;
            }
        }

        // Create window and initialize Nucleus
        switch (backend) {
        case WINDOW_BACKEND_WAYLAND:
            createWindowWayland(argc, argv);
            break;
        case WINDOW_BACKEND_XCB:
            createWindowXcb(argc, argv);
            break;
        case WINDOW_BACKEND_XLIB:
            createWindowXlib(argc, argv);
            break;
        }
    }

    // Using console
    else {
        nucleusInitialize(argc, argv);
    }
    
    return 0;
}
