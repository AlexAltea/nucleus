/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/gpu/rsx_vp.h"

class PGRAPH {
public:
    // Vertex Program
    bool vp_dirty;         // Needs to be recompiled
    u32 vp_data[512 * 4];  // 512 VPE instructions
    u32 vp_load;           // Set through NV4097_SET_TRANSFORM_PROGRAM_LOAD
    u32 vp_start;          // Set through NV4097_SET_TRANSFORM_PROGRAM_START
};
