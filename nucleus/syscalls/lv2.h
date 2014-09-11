/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

enum
{
    LV2_NONE  = 0,

    // Flags
    LV2_AUTH  = (1 << 0),
    LV2_ROOT  = (1 << 1),
    LV2_DBG   = (1 << 2),

    // FW Type
    LV2_CEX   = (1 << 0),
    LV2_DEX   = (1 << 1),
    LV2_DECR  = (1 << 2),
};
