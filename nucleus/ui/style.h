/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/length.h"

namespace ui {

enum ProportionMode {
    PROPORTION_FIXED,      // Use the provided width/height dimensions
    PROPORTION_AUTOWIDTH,  // Calculate width based on the given height
    PROPORTION_AUTOHEIGHT, // Calculate height based on the given width
    PROPORTION_AUTO,       // Calculate width and height
};

struct Style {
    Length top;
    Length left;

    ProportionMode sizeMode = PROPORTION_AUTO;
    Length width;
    Length height;

    float opacity = 1.0;
};

}  // namespace ui
