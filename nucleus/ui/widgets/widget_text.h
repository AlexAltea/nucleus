/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/ui/widget.h"

#include <string>

namespace ui {

class WidgetText : public Widget {
public:
    // Update the texture given a text string
    void update(const std::string& text);

    // Render the image
    virtual void render(gfx::CommandBuffer* cmdBuffer) override;
};

}  // namespace ui
