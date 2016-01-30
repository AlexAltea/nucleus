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
}

void WidgetContainer::render() {
    manager->renderWidget(input);
    for (auto& child : children) {
        child->render();
    }
}

}  // namespace ui
