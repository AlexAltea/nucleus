/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_logo.h"
#include "nucleus/ui/ui.h"
#include "nucleus/ui/screens/list.h"
#include "nucleus/ui/transitions.h"
#include "nucleus/ui/length.h"

namespace ui {

ScreenLogo::ScreenLogo(UI* manager) : Screen(manager) {
    auto* logo = new WidgetImage();
    logo->init("..\\resources\\images\\nucleus-logo.png");
    logo->style.top = 25.0_pct;
    logo->style.left = 25.0_pct;
    logo->style.width = 50.0_pct;
    logo->style.sizeMode = PROPORTION_AUTOHEIGHT;

    body.addElement(logo);
}

void ScreenLogo::update() {
    /*if (dtime > 2000.0) {
        logo.style.opacity = 1.0f - transition::easeOut((dtime - 2000.0f) / 1000.0f);
    }
    if (dtime > 3000.0) {
        if (!config.boot.empty()) {
            parent->pushScreen(std::make_unique<ScreenEmulator>(parent));
        } else {
            parent->pushScreen(std::make_unique<ScreenMain>(parent));
        }
        finished = true;
    }*/
}

}  // namespace ui
