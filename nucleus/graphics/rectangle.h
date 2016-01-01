/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

namespace gfx {

struct Rectangle {
    int left;    // Coordinate X of the upper-left corner
    int top;     // Coordinate Y of the upper-left corner
    int right;   // Coordinate X of the lower-right corner
    int bottom;  // Coordinate Y of the lower-right corner
};

}  // namespace gfx
