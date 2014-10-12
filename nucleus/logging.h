/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <cstdio>
#include <iostream>
#include <mutex>

#if defined(NUCLEUS_WIN)
#define snprintf _snprintf_s
#endif

enum LoggingType
{
    LOG_COMMON = 0,
    LOG_CPU,
    LOG_FS,
    LOG_LOADER,
    LOG_MEMORY,
    LOG_GPU,
    LOG_HLE,
};

// Prints emulator events (notices, warnings and errors) using std::out.
// This is thread-safe in C++11, so there is no need to provide manual synchronization.
class Logger
{
public:
    template<typename... Args>
    void notice(LoggingType type, const char* format, Args... args)
    {
        char log[4096];
        char message[4096];
        snprintf(message, sizeof(message), format, std::forward<Args>(args)...);
        snprintf(log, sizeof(log), "N: %s", message);
        std::cout << log << std::endl;
    }

    template<typename... Args>
    void warning(LoggingType type, const char* format, Args... args)
    {
        char log[4096];
        char message[4096];
        snprintf(message, sizeof(message), format, std::forward<Args>(args)...);
        snprintf(log, sizeof(log), "W: %s", message);
        std::cout << log << std::endl;
    }

    template<typename... Args>
    void error(LoggingType type, const char* format, Args... args)
    {
        char log[4096];
        char message[4096];
        snprintf(message, sizeof(message), format, std::forward<Args>(args)...);
        snprintf(log, sizeof(log), "W: %s", message);
        std::cout << log << std::endl;
    }
};
