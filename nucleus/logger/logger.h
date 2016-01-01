/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/format.h"

#include <iostream>

enum LogType {
    LOG_COMMON = 0,
    LOG_CPU,
    LOG_FS,
    LOG_GPU,
    LOG_GRAPHICS,
    LOG_LOADER,
    LOG_MEMORY,
    LOG_HLE,
    LOG_UI,
};

// Prints emulator events (notices, warnings and errors) using std::cerr.
// This is thread-safe in C++11, so there is no need to provide manual synchronization.
class Logger {
public:
    template <typename... Args>
    void notice(LogType type, const char* pattern, Args... args) {
        const std::string message = "N: " + format(pattern, std::forward<Args>(args)...);
        std::cerr << message.c_str() << std::endl;
    }

    template <typename... Args>
    void warning(LogType type, const char* pattern, Args... args) {
        const std::string message = "W: " + format(pattern, std::forward<Args>(args)...);
        std::cerr << message.c_str() << std::endl;
    }

    template <typename... Args>
    void error(LogType type, const char* pattern, Args... args) {
        const std::string message = "E: " + format(pattern, std::forward<Args>(args)...);
        std::cerr << message.c_str() << std::endl;
    }
};

extern Logger logger;
