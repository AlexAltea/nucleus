/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/widget.h"

namespace ui {

class WidgetTexture : public Widget {
public:
    // Initialize the texture
    void init();

    // Render the image
    virtual void render(gfx::CommandBuffer* cmdBuffer) override;
};

}  // namespace ui
