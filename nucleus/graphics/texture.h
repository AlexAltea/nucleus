/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gfx {

enum TextureFormat {
    TEXTURE_FORMAT_R8G8B8A8,
};

enum TextureFlags {
    TEXTURE_FLAGS_NONE = 0,
};

struct TextureDesc {
    U64 alignment;          // Alignment of the texture in bytes
    U32 width;              // Width of the texture
    U32 height;             // Width of the texture
    U16 mipmapLevels;       // Maximum number of mipmap levels in the texture
    TextureFormat format;   // Format of the texture
    TextureFlags flags;     // Flags
};

class ITexture {
public:
};

}  // namespace gfx
