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

struct Color {
    float r;
    float g;
    float b;
    float a;
};

/**
 * Style
 * =====
 * ## Box Model
 * There are four nested boxes: Margin > Border > Padding > Content.
 *  - Content: The actual contents of the object,
 *  - Padding: Clears a transparent area around the content.
 *  - Border:  Border that goes around the padding.
 *  - Margin:  Clears a transparent area around the border.
 *
 * ## Z-Index
 * Depth index to render an object with. Greater values imply nearer objects and
 * lower values imply farther objects. This is used to compute object occlusion.
 *  - Valid values are 1 to 255.
 *  - Specify 0 to automatically assign Z-Index based on the object render tree position.
 */
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
    struct Border : Box {
        Color color;
    } border;

    Color color;
    Color background;
    Size zindex = 0;

    float opacity = 1.0;
};

}  // namespace ui
