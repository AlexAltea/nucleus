/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "config.h"
#include "nucleus/loader/loader.h"

#include <cstring>

// Global configuration object
Config config;

void Config::parseArguments(int argc, char** argv)
{
    // Parse arguments
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "--console")) {
            console = true;
        }
        if (!strcmp(argv[i], "--debugger")) {
            debugger = true;
        }
    }

    // Check if booting an executable was requested
    std::string lastArgument = argv[argc - 1];
    switch (detectFiletype(lastArgument)) {
    case FILETYPE_ELF:
    case FILETYPE_SELF:
        boot = lastArgument;
    }
}
