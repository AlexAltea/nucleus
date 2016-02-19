/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
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
enum ConfigCpuTranslator {
    // Modes
    CPU_TRANSLATOR_INSTRUCTION  = (1 << 0),
    CPU_TRANSLATOR_BLOCK        = (1 << 1),
    CPU_TRANSLATOR_FUNCTION     = (1 << 2),
    CPU_TRANSLATOR_MODULE       = (1 << 3),

    // Properties
    CPU_TRANSLATOR_IS_RWX       = CPU_TRANSLATOR_BLOCK | CPU_TRANSLATOR_FUNCTION,
    CPU_TRANSLATOR_IS_CACHED    = CPU_TRANSLATOR_FUNCTION | CPU_TRANSLATOR_MODULE,
    CPU_TRANSLATOR_IS_INT       = CPU_TRANSLATOR_INSTRUCTION,
    CPU_TRANSLATOR_IS_JIT       = CPU_TRANSLATOR_BLOCK | CPU_TRANSLATOR_FUNCTION,
    CPU_TRANSLATOR_IS_AOT       = CPU_TRANSLATOR_MODULE,
};

// Graphics Settings
enum ConfigGraphicsBackend {
    GRAPHICS_BACKEND_NULL,
    GRAPHICS_BACKEND_SOFTWARE,
    GRAPHICS_BACKEND_OPENGL,
    GRAPHICS_BACKEND_DIRECT3D11,
    GRAPHICS_BACKEND_DIRECT3D12,
    GRAPHICS_BACKEND_VULKAN,
};

class Config {
public:
    // Immediate settings
    std::string boot;       // Boot the specified file automatically
    bool console;           // Run Nucleus in console-only mode, preventing UI or GPU backends from running
    bool debugger;          // Start Nerve debugging server

    // Saved settings
    ConfigLanguage language;
    ConfigCpuTranslator ppuTranslator;
    ConfigCpuTranslator spuTranslator;
    ConfigGraphicsBackend graphicsBackend;

    // Constructor
    Config();

    // Modify settings with arguments or JSON files
    void parseArguments(int argc, char** argv);
    void parseFile(const std::string& path);
};

extern Config config;
