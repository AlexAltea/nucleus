/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_logo.h"
#include "nucleus/ui/ui.h"
#include "nucleus/ui/transitions.h"
#include "nucleus/ui/screens/screen_emulator.h"
#include "nucleus/ui/screens/screen_main.h"

ScreenLogo::ScreenLogo()
{
    logo.init("..\\resources\\images\\nucleus-logo.png");
    logo.style.top = Length{0.25, Length::Unit::PCT};
    logo.style.left = Length{0.25, Length::Unit::PCT};
    logo.style.width = Length{0.50, Length::Unit::PCT};
    logo.style.sizeMode = PROPORTION_AUTOHEIGHT;
}

void ScreenLogo::render()
{
    logo.render();
}

void ScreenLogo::update()
{
    if (dtime > 2000.0) {
        logo.style.opacity = 1.0f - transition::easeOut((dtime - 2000.0f) / 1000.0f);
    }
    if (dtime > 3000.0) {
        if (!config.boot.empty()) {
            ui.push_screen(new ScreenEmulator());
        } else {
            ui.push_screen(new ScreenMain());
        }
        finished = true;
    }
}
