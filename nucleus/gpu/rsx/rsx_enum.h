/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gpu {
namespace rsx {

// Properties
enum {
    RSX_MAX_TEXTURES = 16,
    RSX_MAX_VERTEX_INPUTS = 16,
};

// RSX Class handles
enum {
    RSX_CONTEXT_DMA_MEMORY_FRAME_BUFFER         = 0xFEED0000, // Local memory
    RSX_CONTEXT_DMA_MEMORY_HOST_BUFFER          = 0xFEED0001, // Main memory
    RSX_CONTEXT_SURFACE2D                       = 0x313371C3,
    RSX_CONTEXT_SWIZZLE2D                       = 0x31337A73,
    RSX_OBJECT_CAFEBABE                         = 0xCAFEBABE,
};

enum Blend : U16 {
    RSX_BLEND_ZERO                      = 0,
    RSX_BLEND_ONE                       = 1,
    RSX_BLEND_SRC_COLOR                 = 0x0300,
    RSX_BLEND_ONE_MINUS_SRC_COLOR       = 0x0301,
    RSX_BLEND_SRC_ALPHA                 = 0x0302,
    RSX_BLEND_ONE_MINUS_SRC_ALPHA       = 0x0303,
    RSX_BLEND_DST_ALPHA                 = 0x0304,
    RSX_BLEND_ONE_MINUS_DST_ALPHA       = 0x0305,
    RSX_BLEND_DST_COLOR                 = 0x0306,
    RSX_BLEND_ONE_MINUS_DST_COLOR       = 0x0307,
    RSX_BLEND_SRC_ALPHA_SATURATE        = 0x0308,
    RSX_BLEND_CONSTANT_COLOR            = 0x8001,
    RSX_BLEND_ONE_MINUS_CONSTANT_COLOR  = 0x8002,
    RSX_BLEND_CONSTANT_ALPHA            = 0x8003,
    RSX_BLEND_ONE_MINUS_CONSTANT_ALPHA  = 0x8004,
};

enum BlendEquation : U16 {
    RSX_BLEND_EQUATION_FUNC_ADD                      = 0x8006,
    RSX_BLEND_EQUATION_MIN                           = 0x8007,
    RSX_BLEND_EQUATION_MAX                           = 0x8008,
    RSX_BLEND_EQUATION_FUNC_SUBTRACT                 = 0x800A,
    RSX_BLEND_EQUATION_FUNC_REVERSE_SUBTRACT         = 0x800B,
    RSX_BLEND_EQUATION_FUNC_REVERSE_SUBTRACT_SIGNED  = 0xF005,
    RSX_BLEND_EQUATION_FUNC_ADD_SIGNED               = 0xF006,
    RSX_BLEND_EQUATION_FUNC_REVERSE_ADD_SIGNED       = 0xF007,
};

enum CullMode : U16 {
    RSX_CULL_MODE_FRONT = 0x0404,
    RSX_CULL_MODE_BACK  = 0x0405,
};

enum ColorMask : U32 {
    RSX_COLOR_MASK_R  = (1 << (0 * 8)),
    RSX_COLOR_MASK_G  = (1 << (1 * 8)),
    RSX_COLOR_MASK_B  = (1 << (2 * 8)),
    RSX_COLOR_MASK_A  = (1 << (3 * 8)),
};

enum LogicOp : U32 {
    RSX_LOGIC_OP_CLEAR          = 0x1500,
    RSX_LOGIC_OP_AND            = 0x1501,
    RSX_LOGIC_OP_AND_REVERSE    = 0x1502,
    RSX_LOGIC_OP_COPY           = 0x1503,
    RSX_LOGIC_OP_AND_INVERTED   = 0x1504,
    RSX_LOGIC_OP_NOOP           = 0x1505,
    RSX_LOGIC_OP_XOR            = 0x1506,
    RSX_LOGIC_OP_OR             = 0x1507,
    RSX_LOGIC_OP_NOR            = 0x1508,
    RSX_LOGIC_OP_EQUIV          = 0x1509,
    RSX_LOGIC_OP_INVERT         = 0x150A,
    RSX_LOGIC_OP_OR_REVERSE     = 0x150B,
    RSX_LOGIC_OP_COPY_INVERTED  = 0x150C,
    RSX_LOGIC_OP_OR_INVERTED    = 0x150D,
    RSX_LOGIC_OP_NAND           = 0x150E,
    RSX_LOGIC_OP_SET            = 0x150F,
};

enum Primitive {
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
};

enum StencilOp {
    RSX_STENCIL_OP_KEEP       = 0x1E00,
    RSX_STENCIL_OP_REPLACE    = 0x1E01,
    RSX_STENCIL_OP_INCR       = 0x1E02,
    RSX_STENCIL_OP_DECR       = 0x1E03,
    RSX_STENCIL_OP_INCR_WRAP  = 0x8507,
    RSX_STENCIL_OP_DECR_WRAP  = 0x8508,
};

enum VertexType : U08 {
    RSX_VERTEX_S1     = 1,
    RSX_VERTEX_F      = 2,
    RSX_VERTEX_SF     = 3,
    RSX_VERTEX_UB     = 4,
    RSX_VERTEX_S32K   = 5,
    RSX_VERTEX_CMP    = 6,
    RSX_VERTEX_UB256  = 7,
};

// RSX Constants
enum {
    RSX_CLEAR_BIT_DEPTH   = 0x01,
    RSX_CLEAR_BIT_STENCIL = 0x02,
    RSX_CLEAR_BIT_COLOR   = 0xF0,

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

}  // namespace rsx
}  // namespace gpu
