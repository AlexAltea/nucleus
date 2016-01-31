/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_logo.h"
#include "nucleus/ui/ui.h"
#include "nucleus/ui/screens/list.h"
#include "nucleus/ui/transitions.h"
#include "nucleus/ui/length.h"

namespace ui {

ScreenLogo::ScreenLogo(UI* manager) : Screen(manager) {
    auto* logo = new WidgetImage("logo");
    logo->init(core::RES_IMAGE_NUCLEUS_LOGO);
    logo->style.top = 25_pct;
    logo->style.left = 25_pct;
    logo->style.width = 50_pct;

    body.addElement(logo);
}

void ScreenLogo::update() {
    if (dtime > 2000ms) {
        //logo->style.opacity = 1.0f - transition::easeOut((dtime - 2000ms) / 1000ms);
    }
    if (dtime > 3000ms) {
        if (!config.boot.empty()) {
            manager->pushScreen(std::make_unique<ScreenEmulator>(manager));
        } else {
            manager->pushScreen(std::make_unique<ScreenMain>(manager));
        }
        finished = true;
    }
}

}  // namespace ui
