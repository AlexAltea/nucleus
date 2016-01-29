/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_main.h"

namespace ui {

ScreenMain::ScreenMain(UI* manager) : Screen(manager) {
    auto* header = new WidgetContainer();
    header->style.top = 25.0_pct;
    header->style.left = 25.0_pct;
    header->style.width = 100.0_pct;
    header->style.sizeMode = PROPORTION_AUTOHEIGHT;

    body.addElement(header);
}

void ScreenMain::update() {
}

}  // namespace ui
