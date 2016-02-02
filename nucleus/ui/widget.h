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
        float position[2];
        Color background;
    } vertex[4];
};

/**
 * Widget
 * ======
 * The Widget base class represents an arbitrary component that is rendered to the user interface.
 * Usually, they are found on Screen objects as part of their Widget tree.
 *
 * ## Style
 * Each Widget has four nested boxes: Margin > Border > Padding > Content.
 *  - Content: The actual content of the Widget, such as text, images or other Widgets.
 *  - Padding: Clears a transparent area around the content.
 *  - Border:  Border that goes around the padding.
 *  - Margin:  Clears a transparent area around the border.
 *
 * Based on this, there are four different ways per axis to measure a Widget length.
 *
 *  | Dimensions | Width              | Height              |
 *  +------------+--------------------+---------------------+
 *  | Content    | getContentWidth()  | getContentHeight()  |
 *  | Padding    | getPaddingWidth()  | getPaddingHeight()  |
 *  | Border     | getBorderWidth()   | getBorderHeight()   |
 *  | Margin     | getMarginWidth()   | getMarginHeight()   |
 */
class Widget {
protected:
    // Texture produced after rendering the contents of this widget
    gfx::Texture* texture;

    // Vertices to be rendered
    WidgetInput input;

    // Final dimension and size
    float vertWidth;
    float vertHeight;
    float vertTop;
    float vertLeft;

    // Computed dimension and size
    float compWidth;
    float compHeight;

    float getCoord(const Length& length, float pixels);
    float getCoordX(const Length& length);
    float getCoordY(const Length& length);

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

    // Dimensions in rendering coordinate space
    float getContentWidth();
    float getContentHeight();
    float getPaddingWidth();
    float getPaddingHeight();
    float getBorderWidth();
    float getBorderHeight();
    float getMarginWidth();
    float getMarginHeight();

    // Absolute position in our rendering coordinate space, including margins
    float getOffsetTop();
    float getOffsetLeft();
    void setOffsetTop(float value);
    void setOffsetLeft(float value);

    // Event handling
    void onMouseMove(std::function<void(MouseEvent&)>);
    void onMouseClick(std::function<void(MouseEvent&)>);
    void onMouseWheel(std::function<void(MouseEvent&)>);
    void onKeyDown(std::function<void(KeyEvent&)>);
    void onKeyUp(std::function<void(KeyEvent&)>);
};

}  // namespace ui
