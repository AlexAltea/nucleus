/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/format.h"

#include <iostream>
#include <mutex>

enum LoggingType
{
    LOG_COMMON = 0,
    LOG_CPU,
    LOG_FS,
    LOG_LOADER,
    LOG_MEMORY,
    LOG_GPU,
    LOG_HLE,
    LOG_UI,
};

// Prints emulator events (notices, warnings and errors) using std::cerr.
// This is thread-safe in C++11, so there is no need to provide manual synchronization.
class Logger
{
public:
    template <typename... Args>
    void notice(LoggingType type, const char* pattern, Args... args)
    {
        const std::string log = "N: " + format(pattern, std::forward<Args>(args)...) + "\n";
        std::cerr << log.c_str();
    }

    template <typename... Args>
    void warning(LoggingType type, const char* pattern, Args... args)
    {
        const std::string log = "W: " + format(pattern, std::forward<Args>(args)...) + "\n";
        std::cerr << log.c_str();
    }

    template <typename... Args>
    void error(LoggingType type, const char* pattern, Args... args)
    {
        const std::string log = "E: " + format(pattern, std::forward<Args>(args)...) + "\n";
        std::cerr << log.c_str();
    }
};
