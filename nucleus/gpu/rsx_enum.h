/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

// RSX Class handles
enum {
    RSX_CONTEXT_DMA_MEMORY_FRAME_BUFFER         = 0xFEED0000, // Local memory
    RSX_CONTEXT_DMA_MEMORY_HOST_BUFFER          = 0xFEED0001, // Main memory
    RSX_CONTEXT_SURFACE2D                       = 0x313371C3,
    RSX_CONTEXT_SWIZZLE2D                       = 0x31337A73,
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY0       = 0x66604200, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY1       = 0x66604201, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY2       = 0x66604202, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY3       = 0x66604203, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY4       = 0x66604204, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY5       = 0x66604205, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY6       = 0x66604206, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY7       = 0x66604207, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_TO_MEMORY_GET_NOTIFY8       = 0x66604208, // Target: lpar_reports[0x1000 : 0x????]
    RSX_CONTEXT_DMA_NOTIFY_MAIN_0               = 0x6660420F,
    RSX_CONTEXT_DMA_TO_MEMORY_GET_SEMAPHORE_RW  = 0x66606660, // Target: lpar_reports[0x0000 : 0x1000] (Read/Write)
    RSX_CONTEXT_DMA_TO_MEMORY_GET_SEMAPHORE_R   = 0x66616661, // Target: lpar_reports[0x0000 : 0x1000] (Read)
    RSX_CONTEXT_DMA_TO_MEMORY_GET_REPORT        = 0x66626660, // Target: lpar_reports[0x1400 : 0x9400] (Read/Write)
    RSX_CONTEXT_DMA_REPORT_LOCATION_MAIN        = 0xBAD68000,
    RSX_OBJECT_56616660_RW                      = 0x56616660,
    RSX_OBJECT_56616660_R                       = 0x56616661,
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
};
