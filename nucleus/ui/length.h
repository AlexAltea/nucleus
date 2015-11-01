/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

namespace ui {

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

}  // namespace ui
