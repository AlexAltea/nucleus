/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_logo.h"
#include "nucleus/ui/ui.h"
#include "nucleus/ui/screens/screen_emulator.h"

// OpenGL dependencies
#include "nucleus/opengl.h"

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
    if (frame > 30) {
        ui.push_screen(new ScreenEmulator());
        finished = true;
    }
}
