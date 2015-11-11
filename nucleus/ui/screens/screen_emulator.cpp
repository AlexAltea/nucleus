/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_emulator.h"
#include "nucleus/emulator.h"

namespace ui {

ScreenEmulator::ScreenEmulator(UI* parent) : Screen(parent) {
    rsxDisplay.style.top = 0.0_pct;
    rsxDisplay.style.left = 0.0_pct;
    rsxDisplay.style.width = 100.0_pct;
    rsxDisplay.style.height = 100.0_pct;
}

void ScreenEmulator::render() {
    //rsxDisplay.textureId = nucleus.rsx.get_display();
    rsxDisplay.render();
}

void ScreenEmulator::update() {
}

}  // namespace ui
