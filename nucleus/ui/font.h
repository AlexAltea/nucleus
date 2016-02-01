/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/core/resource.h"

#include "externals/stb/stb_truetype.h"

namespace ui {

class Font {
    // Holds the font buffer
    core::Resource resource;

public:
    // Holds the font information used by the STB TrueType library
    stbtt_fontinfo fontinfo;

    Font(core::ResourceName resName);
    ~Font();
};

}  // namespace ui
