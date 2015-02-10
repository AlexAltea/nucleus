/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <string>

// Nucleus Settings
enum ConfigLanguage {
    LANGUAGE_DEFAULT,
    LANGUAGE_DE_DE,    // German (Germany)
    LANGUAGE_EN_US,    // English (United States)
    LANGUAGE_ES_ES,    // Spanish (Spain)
};

// CPU Settings
enum ConfigPpuTranslator {
    PPU_TRANSLATOR_INTERPRETER,
    PPU_TRANSLATOR_RECOMPILER,
};

enum ConfigSpuTranslator {
    SPU_TRANSLATOR_INTERPRETER,
    SPU_TRANSLATOR_RECOMPILER,
};

// GPU Settings
enum ConfigGpuBackend {
    GPU_BACKEND_NULL,
    GPU_BACKEND_SOFTWARE,
    GPU_BACKEND_OPENGL,
    GPU_BACKEND_DIRECT3D,
};

class Config
{
public:
    // Settings
    bool debugger = false;  // Start Nerve debugging server
    bool console = false;   // Run Nucleus in console-only mode, preventing UI or GPU backends from running
    ConfigLanguage language = LANGUAGE_DEFAULT;
    ConfigPpuTranslator ppuTranslator = PPU_TRANSLATOR_INTERPRETER;
    ConfigSpuTranslator spuTranslator = SPU_TRANSLATOR_INTERPRETER;
    ConfigGpuBackend gpuBackend = GPU_BACKEND_OPENGL;

    // Modify settings with arguments or JSON files
    void parseArguments(int argc, char** argv);
    void parseFile(const std::string& path);
};

extern Config config;
