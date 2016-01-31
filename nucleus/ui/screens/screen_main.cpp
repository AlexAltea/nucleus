/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_main.h"

namespace ui {

ScreenMain::ScreenMain(UI* manager) : Screen(manager) {
    body.layout = WidgetContainer::LAYOUT_VERTICAL;
    
    auto* header = new WidgetContainer("header");
    header->style.width = 100_pct;
    header->style.padding = 100_px;
    header->style.background = Color{0.7f, 0.7f, 0.2f, 1.0f};

    auto* content = new WidgetContainer("content");
    content->style.padding = 100_px;

    body.addElement(header);
    body.addElement(content);
}

void ScreenMain::update() {
}

}  // namespace ui
