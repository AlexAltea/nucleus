/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen.h"

namespace ui {

Screen::Screen(UI* parent) : parent(parent) {
    time_creation = Clock::now();
}

void Screen::prologue() {
    time_current = Clock::now();
    dtime = time_current - time_creation;
}

void Screen::render(gfx::CommandBuffer* cmdBuffer) {
    body.render(cmdBuffer);
}

void Screen::epilogue() {
    frame += 1;
}

}  // namespace ui
