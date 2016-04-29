/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "widget_button.h"
#include "nucleus/assert.h"
#include "nucleus/ui/widgets/list.h"
#include "nucleus/ui/ui.h"

namespace ui {

void WidgetButton::setText(const std::string& label) {
    const auto* lightFont = manager->fontLight.get();

    wLabel = std::make_unique<WidgetText>(this);
    wLabel->update(lightFont, 30_px, label);
    wLabel->style.color = Color{1,1,1,1};
}

void WidgetButton::setIcon(Icon icon) {
    const auto* iconFont = manager->fontLight.get(); // TODO

    wIcon = std::make_unique<WidgetText>(this);
    wIcon->update(iconFont, 30_px, "\xFF"); // TODO
    wIcon->style.color = Color{1,1,1,1};
}

void WidgetButton::setDesign(Design design) {
    switch (design) {
    case DESIGN_HEADER:
        layout = LAYOUT_HORIZONTAL;
        style.margin.left = 10_px;
        style.margin.right = 10_px;
        style.padding.left = 5_px;
        style.padding.right = 5_px;
        style.background = Color{1.0f, 1.0f, 1.0f, 0.5f};
        break;

    default:
        assert_always("Unimplemented design");
    }
}

}  // namespace ui
