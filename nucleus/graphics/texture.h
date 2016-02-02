/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/format.h"

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
    FILTER_MIN_MAG_MIP_POINT,
};

enum TextureAddress {
    TEXTURE_ADDRESS_WRAP = 1,
    TEXTURE_ADDRESS_MIRROR = 2,
    TEXTURE_ADDRESS_CLAMP = 3,
    TEXTURE_ADDRESS_BORDER = 4,
    TEXTURE_ADDRESS_MIRROR_ONCE = 5,
};

enum TextureFlags {
    TEXTURE_FLAGS_NONE = 0,
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
    U64 alignment;       // Alignment of the texture in bytes
    U32 width;           // Width of the texture
    U32 height;          // Width of the texture
    U16 mipmapLevels;    // Maximum number of mipmap levels in the texture
    Format format;       // Format of the texture
    TextureFlags flags;  // Flags
};

class Texture {
public:
};

}  // namespace gfx
