/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <string>

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
    bool debugger = false;
    bool ui = false;
    ConfigGpuBackend gpuBackend = GPU_BACKEND_OPENGL;

    // Modify settings with arguments or JSON files
    void parseArguments(int argc, char** argv);
    void parseFile(const std::string& path);
};

extern Config config;
