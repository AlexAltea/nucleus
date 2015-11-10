/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

namespace gfx {

struct Viewport {
    float originX;
    float originY;
    float width;
    float height;
    float minDepth;
    float maxDepth;
};

}  // namespace gfx
