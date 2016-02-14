/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

// Nucleus
#include "nucleus/nucleus.h"
#include "nucleus/core/config.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cstring>

int main(int argc, char **argv) {
    // Load configuration
    nucleusConfigure(argc, argv);

    // Using UI
    if (!config.console) {
        Display* display = XOpenDisplay("");
        int screen = DefaultScreen(display);
        unsigned long background = WhitePixel(display, screen);
        unsigned long foreground = BlackPixel(display, screen);

        XSizeHints hint;
        hint.x = 200;
        hint.y = 200;
        hint.width = 960;
        hint.height = 544;
        hint.flags = PSize;

        Window window = XCreateSimpleWindow(display, DefaultRootWindow(display),
            hint.x, hint.y, hint.width, hint.height, 5, foreground, background);

        const char appName[] = "Nucleus";
        XSetStandardProperties(display, window, appName, appName, None, argv, argc, &hint);

        GC gc = XCreateGC(display, window, 0, 0);
        XSetBackground(display, gc, background);
        XSetForeground(display, gc, foreground);
        XSelectInput(display, window, ButtonPressMask | KeyPressMask | ExposureMask);
        XMapRaised(display, window);
        
        int done = 0;
        while (!done) {
            XEvent evt;
            XNextEvent(display, &evt);
            switch(evt.type) {
            case MappingNotify:
                XRefreshKeyboardMapping(&evt.xmapping);
                break;
            case ButtonPress:
	        nucleusOnMouseClick(evt.xbutton.x, evt.xbutton.y);
                break;
            case KeyPress:
                break;
            }
        }

        XFreeGC(display, gc);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }

    // Using console
    else {
        nucleusInitialize(argc, argv);
    }
    
    return 0;
}
