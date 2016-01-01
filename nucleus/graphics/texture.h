/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/format.h"

namespace gfx {

enum TextureFlags {
    TEXTURE_FLAGS_NONE = 0,
};

struct TextureDesc {
    U64 alignment;          // Alignment of the texture in bytes
    U32 width;              // Width of the texture
    U32 height;             // Width of the texture
    U16 mipmapLevels;       // Maximum number of mipmap levels in the texture
    Format format;   // Format of the texture
    TextureFlags flags;     // Flags
};

class Texture {
public:
};

}  // namespace gfx
