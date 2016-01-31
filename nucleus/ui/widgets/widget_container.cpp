/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_container.h"
#include "nucleus/assert.h"
#include "nucleus/ui/ui.h"

#include <algorithm>

namespace ui {

bool WidgetContainer::addElement(Widget* widget) {
    assert_true(widget->parent == nullptr, "Widget already has a parent");
    widget->parent = this;
    widget->manager = manager;
    children.push_back(widget);
    return true;
}

bool WidgetContainer::removeElement(const std::string& target) {
    bool found = false;
    auto it = children.begin();
    while (it != children.end()) {
        Widget* child = *it;
        if (child->id == target) {
            it = children.erase(it);
            found = true;
        } else {
            it++;
        }
    }
    return found;
}

Widget* WidgetContainer::find(const std::string& query) {
    for (auto& child : children) {
        Widget* result = child->find(query);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

void WidgetContainer::dimensionalize() {
    vertWidth = 0.0;
    vertWidth += getCoordX(style.padding.left);
    vertWidth += getCoordX(style.padding.right);
    vertWidth += getCoordX(style.border.left);
    vertWidth += getCoordX(style.border.right);
    vertHeight = 0.0;
    vertHeight += getCoordY(style.padding.top);
    vertHeight += getCoordY(style.padding.bottom);
    vertHeight += getCoordY(style.border.top);
    vertHeight += getCoordY(style.border.bottom);

    if (layout == LAYOUT_VERTICAL) {
        for (auto& child : children) {
            child->dimensionalize();
            auto ow = child->getOuterWidth();
            auto oh = child->getOuterHeight();
            vertWidth = std::max(ow, vertWidth);
            vertHeight += oh;
        }
    } else {
        for (auto& child : children) {
            child->dimensionalize();
            auto ow = child->getOuterWidth();
            auto oh = child->getOuterHeight();
            vertHeight = std::max(oh, vertHeight);
            vertWidth += ow;
        }
    }

    // Override dimensions
    if (style.width.type != Length::TYPE_UNDEFINED) {
        vertWidth = getCoordX(style.width);
    }
    if (style.height.type != Length::TYPE_UNDEFINED) {
        vertHeight = getCoordY(style.height);
    }
}

void WidgetContainer::render() {
    auto offsetTop = getOffsetTop() + getCoordY(style.margin.top);
    auto offsetLeft = getOffsetLeft() + getCoordX(style.margin.top);

    // Preparing vertex input
    auto x1 = -1.0 + 2 * (offsetLeft);
    auto x2 = -1.0 + 2 * (offsetLeft + getOuterWidth());
    auto y1 = +1.0 - 2 * (offsetTop + getOuterHeight());
    auto y2 = +1.0 - 2 * (offsetTop);

    /**
     * Screen
     * ======
     *  (0,1)                              (1,1)
     *  +--------------------------------------+
     *  |   V1 = (x1,y2)       V3 = (x2,y2)    |
     *  |     + - - - - - - - +                |
     *  |     : \             :                |
     *  |     :    \          :                |
     *  |     :       \       : h              |
     *  |     :          \    :                |
     *  |     :       w     \ :                |
     *  |     + - - - - - - - +                |
     *  |   V0 = (x1,y1)       V2 = (x2,y1)    |
     *  +--------------------------------------+
     *  (0,0)                              (1,0)
     */
    auto& V0 = input.vertex[0];
    auto& V1 = input.vertex[1];
    auto& V2 = input.vertex[2];
    auto& V3 = input.vertex[3];

    V0.position[0] = V1.position[0] = x1;
    V2.position[0] = V3.position[0] = x2;
    V0.position[1] = V2.position[1] = y1;
    V1.position[1] = V3.position[1] = y2;
    V0.position[2] = V1.position[2] = V2.position[2] = V3.position[2] = 0.0;
    V0.position[3] = V1.position[3] = V2.position[3] = V3.position[3] = 1.0;
    V0.background = style.background;
    V1.background = style.background;
    V2.background = style.background;
    V3.background = style.background;

    manager->renderWidget(input);

    auto childOffsetTop = offsetTop + getCoordY(style.padding.top);
    auto childOffsetLeft = offsetLeft + getCoordX(style.padding.left);
    if (layout == LAYOUT_VERTICAL) {
        for (auto& child : children) {
            child->setOffsetTop(childOffsetTop);
            child->setOffsetLeft(childOffsetLeft);
            child->render();
            childOffsetTop += child->getOuterHeight();
        }
    } else {
        for (auto& child : children) {
            child->setOffsetTop(childOffsetTop);
            child->setOffsetLeft(childOffsetLeft);
            child->render();
            childOffsetLeft += child->getOuterWidth();
        }
    }
}

}  // namespace ui
