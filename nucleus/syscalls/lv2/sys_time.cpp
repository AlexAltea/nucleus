/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_time.h"
#include "nucleus/syscalls/lv2.h"

s32 sys_time_get_timezone(be_t<u32>* timezone, be_t<u32>* summertime)
{
    *timezone = 1;
    *summertime = 1;
    return CELL_OK;
}

s32 sys_time_get_current_time(be_t<u64>* sec, be_t<u64>* nsec)
{
    *sec = 1;
    *nsec = 1;
    return CELL_OK;
}

u64 sys_time_get_timebase_frequency()
{
    return 79800000;
}
