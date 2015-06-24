/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_emulator.h"
#include "nucleus/emulator.h"

// OpenGL dependencies
#include "nucleus/opengl.h"

ScreenEmulator::ScreenEmulator()
{
    rsxDisplay.style.top = Length{0.00, Length::Unit::PCT};
    rsxDisplay.style.left = Length{0.00, Length::Unit::PCT};
    rsxDisplay.style.width = Length{1.00, Length::Unit::PCT};
    rsxDisplay.style.height = Length{1.00, Length::Unit::PCT};
}

void ScreenEmulator::render()
{
    rsxDisplay.textureId = nucleus.rsx.get_display();
    rsxDisplay.render();
    glFinish();
}

void ScreenEmulator::update()
{
}
