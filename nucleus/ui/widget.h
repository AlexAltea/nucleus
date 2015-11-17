/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/ui/length.h"
#include "nucleus/ui/style.h"

#include <string>

namespace ui {

class Widget {
protected:
    // Parent widget if applicable
    Widget* parent;

    // Texture produced after rendering the contents of this widget
    gfx::Texture* texture;

public:
    std::string id;

    Style style;

    /**
     * Search children nodes for a particular identifier
     * @param[in]  query  Identifier to search for
     * @return            Pointer to the found widget or nullptr otherwise
     */
    virtual Widget* find(const std::string& query);

    /**
     * Pass the rendering commands to the UI command buffer
     * @param[in]  cmdBuffer  Command buffer to store the rendering commands in
     */
    virtual void render(gfx::CommandBuffer* cmdBuffer) = 0;

    static Length correctHeight(Length height);

    static float getCoordinateX(Length x);
    static float getCoordinateY(Length y);
};

}  // namespace ui
