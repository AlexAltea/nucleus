/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_timer.h"
#include "nucleus/syscalls/lv2.h"

s32 sys_timer_sleep(u32 sleep_time)
{
    // TODO: Use a condition variable to kill the thread while it sleeps
    std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
    return CELL_OK;
}

s32 sys_timer_usleep(u64 sleep_time)
{
    // Maximum value is: 2^48-1
    if (sleep_time > 0xFFFFFFFFFFFFULL) {
        sleep_time = 0xFFFFFFFFFFFFULL;
    }
    // TODO: Use a condition variable to kill the thread while it sleeps
    std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
    return CELL_OK;
}
