/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/length.h"

namespace ui {

enum AlignHorizontal {
    ALIGN_HORIZONTAL_LEFT = 0,
    ALIGN_HORIZONTAL_CENTER,
    ALIGN_HORIZONTAL_RIGHT,
};

enum AlignVertical {
    ALIGN_VERTICAL_TOP = 0,
    ALIGN_VERTICAL_CENTER,
    ALIGN_VERTICAL_BOTTOM,
};

enum ProportionMode {
    PROPORTION_FIXED,      // Use the provided width/height dimensions
    PROPORTION_AUTOWIDTH,  // Calculate width based on the given height
    PROPORTION_AUTOHEIGHT, // Calculate height based on the given width
    PROPORTION_AUTO,       // Calculate width and height
};

struct Color {
    float r;
    float g;
    float b;
    float a;
};

class Style {
private:
    struct Box {
        Length top;
        Length right;
        Length bottom;
        Length left;

        Box& operator= (const Length& length) {
            top = length;
            right = length;
            bottom = length;
            left = length;
            return *this;
        }
    };

public:
    Length top;
    Length left;
    Length width;
    Length height;

    AlignHorizontal alignH;
    AlignVertical alignV;

    Box padding;
    Box margin;
    struct Border {
        Length top;
        Length right;
        Length bottom;
        Length left;
        Color color;
    } border;

    Color color;
    Color background;

    ProportionMode sizeMode = PROPORTION_AUTO;

    float opacity = 1.0;
};

}  // namespace ui
