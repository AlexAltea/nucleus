/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_emulator.h"
#include "nucleus/emulator.h"
#include "nucleus/gpu/gpu.h"

namespace ui {

ScreenEmulator::ScreenEmulator(UI* manager) : Screen(manager) {
    body.style.alignH = ALIGN_HORIZONTAL_CENTER;
    body.style.alignV = ALIGN_VERTICAL_CENTER;
    body.style.background = Color{0,0,0,1};

    app = new WidgetImage("app");
    app->isColorTarget = true;
    app->manager = manager;
    app->style.width = 100_pct;
    app->style.height = 100_pct;
    body.addElement(app);
}

void ScreenEmulator::update() {
    if (nucleus.gpu.get()) {
        auto* appTexture = nucleus.gpu->getFrontBuffer();
        app->update(appTexture);
    }
}

}  // namespace ui
