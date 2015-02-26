/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

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
protected:
    // Thread status and management
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::string m_name;
    std::thread* m_thread = nullptr;

    EmulatorEvent m_event = NUCLEUS_EVENT_NONE;
    EmulatorStatus m_status = NUCLEUS_STATUS_UNKNOWN;

public:
    s32 prio;  // Thread priority
    u64 id;    // Thread ID (relevant for LV2)

    // Open a new thread that will enter the code emulation loop
    virtual void start()=0;

    // Enter emulation loop
    virtual void task()=0;

    // Control the thread once it is started
    virtual void run()=0;
    virtual void pause()=0;
    virtual void stop()=0;

    // Block caller thread until this thread finishes
    void join();
};
