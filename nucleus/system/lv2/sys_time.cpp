/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "sys_time.h"
#include "nucleus/system/lv2.h"

namespace sys {

S32 sys_time_get_timezone(BE<U32>* timezone, BE<U32>* summertime)
{
    *timezone = 1;
    *summertime = 1;
    return CELL_OK;
}

S32 sys_time_get_current_time(BE<U64>* sec, BE<U64>* nsec)
{
    *sec = 1;
    *nsec = 1;
    return CELL_OK;
}

U64 sys_time_get_timebase_frequency()
{
    return 79800000;
}

}  // namespace sys
