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

class Widget {
public:
    struct Style {
        // Position of the image in the [+0.0, +1.0] system
        Length top;
        Length left;

        // Dimensions of the image in the [+0.0, +1.0] system
        ProportionMode sizeMode = PROPORTION_AUTO;
        Length width;
        Length height;

        float opacity = 1.0;
    } style;

public:
    static Length correctHeight(Length height);

    static float getCoordinateX(Length x);
    static float getCoordinateY(Length y);

    // Render the screen components
    virtual void render() = 0;
};

}  // namespace ui
