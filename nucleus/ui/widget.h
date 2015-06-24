/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

enum ProportionMode {
    PROPORTION_FIXED,      // Use the provided width/height dimensions
    PROPORTION_AUTOWIDTH,  // Calculate width based on the given height
    PROPORTION_AUTOHEIGHT, // Calculate height based on the given width
    PROPORTION_AUTO,       // Calculate width and height
};

struct Length {
    float value;
    enum Unit {
        PX,  // Pixel
        CM,  // Centimeter
        PCT, // Percentage
    } unit;

    // Scalar operations
    Length operator+ (float rhs) { return Length{value + rhs, unit}; }
    Length operator- (float rhs) { return Length{value - rhs, unit}; }
    Length operator* (float rhs) { return Length{value * rhs, unit}; }
    Length operator/ (float rhs) { return Length{value / rhs, unit}; }

    // Length operations // TODO: Check unit used
    Length operator+ (const Length& rhs) { return Length{value + rhs.value, unit}; }
    Length operator- (const Length& rhs) { return Length{value - rhs.value, unit}; }
    Length operator* (const Length& rhs) { return Length{value * rhs.value, unit}; }
    Length operator/ (const Length& rhs) { return Length{value / rhs.value, unit}; }
};

class Widget
{
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
    virtual void render()=0;
};
