/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/translator.h"

#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>

enum CellThreadType {
    CELL_THREAD_PPU,
    CELL_THREAD_SPU,
    CELL_THREAD_RAWSPU,
};

class CellThread
{
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::string m_name;
    std::thread* m_thread;

    EmulatorEvent m_event;
    EmulatorStatus m_status;

protected:
    // Code translation mechanism used by the thread
    CellTranslator* m_translator;

public:
    s32 prio;  // Thread priority
    u64 id;    // Thread ID (relevant for LV2)

    // Program Counter and RTOC
    u32 pc;
    u32 rtoc;

    // Open a new thread that will enter the code emulation loop
    void start();

    // Enter emulation loop
    void task();

    // These functions control the thread once it is started
    void run();
    void pause();
    void finishCallback();
    void stop();

    // Block caller thread until this thread finishes
    void join();
};
