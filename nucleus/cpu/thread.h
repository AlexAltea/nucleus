/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/translator.h"

#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>

class CellThread
{
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::string m_name;
    std::thread* m_thread;

    EmulatorEvent m_event;
    EmulatorStatus m_status;

    // Enter loop of code emulation
    void task();

protected:
    // Code translation mechanism used by the thread
    CellTranslator* m_translator;

public:
    // Program Counter
    u32 pc;

    // Open a new thread that will enter the code emulation loop
    void start();

    // These functions control the thread once it is started
    void run();
    void pause();
    void stop();
};
