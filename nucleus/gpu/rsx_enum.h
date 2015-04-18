/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gpu {

// Properties
enum {
    RSX_MAX_TEXTURES = 16,
};

// RSX Class handles
enum {
    RSX_CONTEXT_DMA_MEMORY_FRAME_BUFFER         = 0xFEED0000, // Local memory
    RSX_CONTEXT_DMA_MEMORY_HOST_BUFFER          = 0xFEED0001, // Main memory
    RSX_CONTEXT_SURFACE2D                       = 0x313371C3,
    RSX_CONTEXT_SWIZZLE2D                       = 0x31337A73,
    RSX_OBJECT_CAFEBABE                         = 0xCAFEBABE,
};

// RSX Constants
enum {
    // Primitives
    RSX_PRIMITIVE_POINTS          = 1,
    RSX_PRIMITIVE_LINES           = 2,
    RSX_PRIMITIVE_LINE_LOOP       = 3,
    RSX_PRIMITIVE_LINE_STRIP      = 4,
    RSX_PRIMITIVE_TRIANGLES       = 5,
    RSX_PRIMITIVE_TRIANGLE_STRIP  = 6,
    RSX_PRIMITIVE_TRIANGLE_FAN    = 7,
    RSX_PRIMITIVE_QUADS           = 8,
    RSX_PRIMITIVE_QUAD_STRIP      = 9,
    RSX_PRIMITIVE_POLYGON         = 10,

    // Vertex data type
    RSX_VERTEX_S1     = 1,
    RSX_VERTEX_F      = 2,
    RSX_VERTEX_SF     = 3,
    RSX_VERTEX_UB     = 4,
    RSX_VERTEX_S32K   = 5,
    RSX_VERTEX_CMP    = 6,
    RSX_VERTEX_UB256  = 7,

    // Data location
    RSX_LOCATION_LOCAL  = 0,
    RSX_LOCATION_MAIN   = 1,

    // Reports
    RSX_REPORT_ZPASS_PIXEL_CNT  = 1,
    RSX_REPORT_ZCULL_STATS      = 2,
    RSX_REPORT_ZCULL_STATS1     = 3,
    RSX_REPORT_ZCULL_STATS2     = 4,
    RSX_REPORT_ZCULL_STATS3     = 5,

    // Surfaces
    RSX_SURFACE_TARGET_NONE     = 0,
    RSX_SURFACE_TARGET_0        = 1,
    RSX_SURFACE_TARGET_1        = 2,
    RSX_SURFACE_TARGET_MRT1     = 0x10 | 0x3,
    RSX_SURFACE_TARGET_MRT2     = 0x10 | 0x7,
    RSX_SURFACE_TARGET_MRT3     = 0x10 | 0xF,
};

}  // namespace gpu
