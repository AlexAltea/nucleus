/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"

#include <mutex>

namespace ui {

enum SurfaceType {
    SURFACE_SMALL,   // Phones
    SURFACE_MEDIUM,  // Tablets
    SURFACE_LARGE,   // Computers
};

class Surface {
    std::mutex mutex;
    bool changed;

    Size nextWidth;
    Size nextHeight;
    SurfaceType nextType;

public:
    // Current properties
    Size width;
    Size height;
    SurfaceType type;
    unsigned int hz;
    unsigned int dpi;
    float proportion;

    // Getters
    Size getWidth() const;
    Size getHeight() const;
    unsigned int getHz() const;
    unsigned int getDpi() const;

    // Setters
    void setWidth(Size w);
    void setHeight(Size h);

    // Constructor
    Surface(Size width, Size height);

    /**
     * Apply surface updates and configure graphics backend accordingly
     * @param[in]  backend  Graphics backend to be configured
     */
    void update(gfx::GraphicsBackend* backend);
};

}  // namespace ui
