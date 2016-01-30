/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/ui/event.h"
#include "nucleus/ui/length.h"
#include "nucleus/ui/style.h"

#include <functional>
#include <string>

namespace ui {

// Forward declarations
class UI;

struct WidgetInput {
    struct WidgetVertex {
        F32 position[4];
        F32 color[4];
    } vertex[4];
};

/**
 * Widget
 * ======
 * The Widget base class represents an arbitrary component that is rendered to the user interface.
 * Usually, they are found on Screen objects as part of their Widget tree.
 */
class Widget {
protected:
    // Texture produced after rendering the contents of this widget
    gfx::Texture* texture;

    // Vertices to be rendered
    WidgetInput input;

    float vertWidth;
    float vertHeight;
    float vertTop;
    float vertLeft;

    float getCoord(Length length, float proportion);
    float getCoordX(Length x);
    float getCoordY(Length y);

public:
    // UI Manager
    UI* manager;

    // Parent widget if applicable
    Widget* parent = nullptr;

    std::string id;

    Style style;

    Widget();
    Widget(const std::string& id);

    /**
     * Search children nodes for a particular identifier
     * @param[in]  query  Identifier to search for
     * @return            Pointer to the found widget or nullptr otherwise
     */
    virtual Widget* find(const std::string& query);

    /**
     * Fill information about dimensions in the WidgetInput structure
     */
    virtual void dimensionalize() = 0;

    /**
     * Pass the rendering commands to the UI command buffer
     * @param[in]  cmdBuffer  Command buffer to store the rendering commands in
     */
    virtual void render() = 0;

    // Get dimensions in our rendering coordinate space
    float getOuterWidth();
    float getOuterHeight();

    // Get absolute position in our rendering coordinate space
    float getOffsetX();
    float getOffsetY();

    // Event handling
    void onMouseMove(std::function<void(MouseEvent&)>);
    void onMouseClick(std::function<void(MouseEvent&)>);
    void onMouseWheel(std::function<void(MouseEvent&)>);
    void onKeyDown(std::function<void(KeyEvent&)>);
    void onKeyUp(std::function<void(KeyEvent&)>);
};

}  // namespace ui
