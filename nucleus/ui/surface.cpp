/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "surface.h"

namespace ui {

// Getters
Size Surface::getWidth() const {
    return width;
}
Size Surface::getHeight() const {
    return height;
}

// Setters
void Surface::setWidth(Size w) {
    std::lock_guard<std::mutex> lock(mutex);
    nextWidth = w;
    changed = true;
}
void Surface::setHeight(Size h) {
    std::lock_guard<std::mutex> lock(mutex);
    nextHeight = h;
    changed = true;
}

Surface::Surface(Size width, Size height) :
    width(width), height(height), hz(60), dpi(100), changed(false) {
    proportion = float(width) / float(height);
}

void Surface::update(gfx::IBackend* backend) {
    if (!changed) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);
    if (nextWidth != width || nextHeight != height) {
        width = nextWidth;
        height = nextHeight;
        proportion = float(width) / float(height);
        backend->doResizeBuffers(width, height);
    }

    changed = false;
}

}  // namespace ui
