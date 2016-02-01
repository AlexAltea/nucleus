/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/ui/font.h"
#include "nucleus/ui/widget.h"

#include <string>
#include <vector>

namespace ui {

class WidgetText : public Widget {
    // Text
    std::vector<Byte> txBuffer;
    Size txWidth = 0;
    Size txHeight = 0;

public:
    WidgetText() {}
    WidgetText(const std::string& id) : Widget(id) {}

    /**
     * Update the Widget texture given a text string to render and a font family and size to use
     * @param[in]  fontFamily  Font family to render the text with
     * @param[in]  fontSize    Line height to render the text with
     * @param[in]  text        Text to rasterize
     */
    void update(const Font* fontFamily, const Length& fontSize, const std::string& text);

    virtual void dimensionalize() override;
    virtual void render() override;
};

}  // namespace ui
