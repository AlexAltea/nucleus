/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "config.h"

#include <cstring>

// Global configuration object
Config config;

void Config::parseArguments(int argc, char** argv)
{
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "--debugger")) {
            debugger = true;
        }
    }
}
