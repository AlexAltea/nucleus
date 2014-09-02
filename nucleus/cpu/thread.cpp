/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "thread.h"
#include "nucleus/common.h"
#include <iostream>//
void CellThread::start()
{
    if (m_thread) {
        stop();
    }

    m_thread = new std::thread([&](){
        m_event = NUCLEUS_EVENT_PAUSE;
        task();
    });
}

void CellThread::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_RUN;
    m_cv.notify_one();
}

void CellThread::pause()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_PAUSE;
}

void CellThread::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event = NUCLEUS_EVENT_STOP;
}

void CellThread::task()
{
    while (true) {
        // Handle events
        if (m_event) {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_event == NUCLEUS_EVENT_PAUSE) {
                m_status = NUCLEUS_STATUS_PAUSED;
                m_cv.wait(lock, [&]{ return m_event == NUCLEUS_EVENT_RUN; });
                m_status = NUCLEUS_STATUS_RUNNING;
            }
            if (m_event == NUCLEUS_EVENT_STOP) {
                break;
            }
            m_event = NUCLEUS_EVENT_NONE;
        }
        m_translator->step();
    }
}