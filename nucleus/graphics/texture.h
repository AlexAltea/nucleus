/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/format.h"
#include "nucleus/graphics/resource.h"

namespace gfx {

enum ComparisonFunc {
    COMPARISON_FUNC_NEVER = 1,
    COMPARISON_FUNC_LESS = 2,
    COMPARISON_FUNC_EQUAL = 3,
    COMPARISON_FUNC_LESS_EQUAL = 4,
    COMPARISON_FUNC_GREATER = 5,
    COMPARISON_FUNC_NOT_EQUAL = 6,
    COMPARISON_FUNC_GREATER_EQUAL = 7,
    COMPARISON_FUNC_ALWAYS = 8,
};

enum Filter {
    FILTER_MIN_MAG_MIP_POINT                 = 0,
    FILTER_MIN_MAG_POINT_MIP_LINEAR          = 0x1,
    FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT    = 0x4,
    FILTER_MIN_POINT_MAG_MIP_LINEAR          = 0x5,
    FILTER_MIN_LINEAR_MAG_MIP_POINT          = 0x10,
    FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR   = 0x11,
    FILTER_MIN_MAG_LINEAR_MIP_POINT          = 0x14,
    FILTER_MIN_MAG_MIP_LINEAR                = 0x15,
};

enum TextureAddress {
    TEXTURE_ADDRESS_WRAP = 1,
    TEXTURE_ADDRESS_MIRROR = 2,
    TEXTURE_ADDRESS_CLAMP = 3,
    TEXTURE_ADDRESS_BORDER = 4,
    TEXTURE_ADDRESS_MIRROR_ONCE = 5,
};

enum TextureSwizzle {
    TEXTURE_SWIZZLE_COMPONENT_0	= 0,
    TEXTURE_SWIZZLE_COMPONENT_1	= 1,
    TEXTURE_SWIZZLE_COMPONENT_2	= 2,
    TEXTURE_SWIZZLE_COMPONENT_3	= 3,
    TEXTURE_SWIZZLE_VALUE_0	= 4,
    TEXTURE_SWIZZLE_VALUE_1	= 5
};

#define TEXTURE_SWIZZLE_MASK   0x7 
#define TEXTURE_SWIZZLE_SHIFT  3 
#define TEXTURE_SWIZZLE_ENCODE(S0, S1, S2, S3) ( \
    (((S0) & TEXTURE_SWIZZLE_MASK) << (TEXTURE_SWIZZLE_SHIFT * 0)) | \
    (((S1) & TEXTURE_SWIZZLE_MASK) << (TEXTURE_SWIZZLE_SHIFT * 1)) | \
    (((S2) & TEXTURE_SWIZZLE_MASK) << (TEXTURE_SWIZZLE_SHIFT * 2)) | \
    (((S3) & TEXTURE_SWIZZLE_MASK) << (TEXTURE_SWIZZLE_SHIFT * 3)) | \
                                 1 << (TEXTURE_SWIZZLE_SHIFT * 4))
#define TEXTURE_SWIZZLE_DEFAULT TEXTURE_SWIZZLE_ENCODE(0,1,2,3)

enum TextureFlags {
    TEXTURE_FLAG_NONE = 0,
    TEXTURE_FLAG_COLOR_TARGET         = (1 << 0),
    TEXTURE_FLAG_DEPTHSTENCIL_TARGET  = (1 << 1),
};

struct Sampler {
    Filter filter;
    TextureAddress addressU;
    TextureAddress addressV;
    TextureAddress addressW;
};

struct TextureDesc {
    const Byte* data;    // Pointer to the texture data (optional)
    Size size;           // Size of the texture data (optional)
    U32 width;           // Width of the texture
    U32 height;          // Width of the texture
    U16 mipmapLevels;    // Maximum number of mipmap levels in the texture
    Format format;       // Format of the texture
    TextureFlags flags;  // Flags
    int swizzle = TEXTURE_SWIZZLE_DEFAULT;
};

class Texture : public virtual Resource {
public:
};

}  // namespace gfx
