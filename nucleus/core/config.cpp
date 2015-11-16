/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "config.h"
#include "nucleus/filesystem/filesystem.h"

#include <cstring>

// Global configuration object
Config config;

Config::Config() {
    // Default settings
    console = false;
    debugger = false;

    language = LANGUAGE_DEFAULT;
    ppuTranslator = CPU_TRANSLATOR_FUNCTION;
    spuTranslator = CPU_TRANSLATOR_INSTRUCTION;
    graphicsBackend = GRAPHICS_BACKEND_OPENGL;
}

void Config::parseArguments(int argc, char** argv) {
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
    if (argc > 1) {
        std::string lastArgument = argv[argc - 1];
        if (fs::HostFileSystem::existsFile(lastArgument)) {
            boot = lastArgument;
        }
    }
}
