/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget.h"
#include "nucleus/assert.h"
#include "nucleus/ui/ui.h"

namespace ui {

Widget::Widget() : style({}) {
}

Widget::Widget(const std::string& id) : id(id), style({}) {
}

Widget* Widget::find(const std::string& query) {
    if (id == query) {
        return this;
    }
    return nullptr;
}

float Widget::getCoord(const Length& length, float pixels) {
    constexpr auto meterToInch = 39.3701;
    const Surface& surface = manager->surface;

    switch (length.type) {
    case Length::TYPE_UNDEFINED:
        return 0;
    case Length::TYPE_PCT:
        return (length.value) / 100;
    case Length::TYPE_PX:
        return (length.value) / pixels;
    case Length::TYPE_IN:
        return (length.value * surface.dpi) / pixels;
    case Length::TYPE_CM:
        return (length.value * surface.dpi * meterToInch * 0.01) / pixels;
    case Length::TYPE_MM:
        return (length.value * surface.dpi * meterToInch * 0.001) / pixels;
    default:
        assert_always("Unimplemented");
        return 0.0;
    }
}

float Widget::getCoordX(const Length& length) {
    const Surface& surface = manager->surface;
    return getCoord(length, surface.width);
}

float Widget::getCoordY(const Length& length) {
    const Surface& surface = manager->surface;
    return getCoord(length, surface.height);
}

// Dimensions in rendering coordinate space
float Widget::getContentWidth() {
    return vertWidth;
}
float Widget::getContentHeight() {
    return vertHeight;
}
float Widget::getPaddingWidth() {
    auto paddingLeft = getCoordX(style.padding.left);
    auto paddingRight = getCoordX(style.padding.right);
    return getContentWidth() + paddingLeft + paddingRight;
}
float Widget::getPaddingHeight() {
    auto paddingTop = getCoordY(style.padding.top);
    auto paddingBottom = getCoordY(style.padding.bottom);
    return getContentHeight() + paddingTop + paddingBottom;
}
float Widget::getBorderWidth() {
    auto borderLeft = getCoordX(style.border.left);
    auto borderRight = getCoordX(style.border.right);
    return vertWidth + borderLeft + borderRight;
}
float Widget::getBorderHeight() {
    auto borderTop = getCoordY(style.border.top);
    auto borderBottom = getCoordY(style.border.bottom);
    return vertHeight + borderTop + borderBottom;
}
float Widget::getMarginWidth() {
    auto marginLeft = getCoordX(style.margin.left);
    auto marginRight = getCoordX(style.margin.right);
    return vertWidth + marginLeft + marginRight;
}
float Widget::getMarginHeight() {
    auto marginTop = getCoordY(style.margin.top);
    auto marginBottom = getCoordY(style.margin.bottom);
    return vertHeight + marginTop + marginBottom;
}

float Widget::getOffsetTop() {
    return vertTop;
}

float Widget::getOffsetLeft() {
    return vertLeft;
}

void Widget::setOffsetTop(float value) {
    vertTop = value;
}

void Widget::setOffsetLeft(float value) {
    vertLeft = value;
}

}  // namespace ui
