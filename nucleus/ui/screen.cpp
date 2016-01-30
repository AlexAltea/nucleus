/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen.h"

namespace ui {

Screen::Screen(UI* ui) : manager(ui) {
    time_creation = Clock::now();

    // Body container style
    body.manager = ui;
    body.style.height = 100.0_pct;
    body.style.width = 100.0_pct;
}

void Screen::prologue() {
    time_current = Clock::now();
    dtime = time_current - time_creation;
}

void Screen::render() {
    body.dimensionalize();
    body.render();
}

void Screen::epilogue() {
    frame += 1;
}

}  // namespace ui
