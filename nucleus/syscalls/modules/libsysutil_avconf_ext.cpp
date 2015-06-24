/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "libsysutil_avconf_ext.h"
#include "nucleus/emulator.h"

s32 cellVideoOutGetGamma()
{
    nucleus.log.warning(LOG_HLE, "libsysutil_avconf_ext: cellVideoOutGetGamma called");
    // TODO: ?
    return CELL_OK;
}
