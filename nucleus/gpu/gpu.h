/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Forward declarations
class Emulator;
namespace gfx { class Texture; }
namespace gfx { class GraphicsBackend; }

namespace gpu {

class GPU {
    Emulator* m_emulator;

public:
    GPU(Emulator* emulator);

    /**
     * Get the emulator object to which this this CPU is belongs.
     * @return  Emulator object
     */
    Emulator* getEmulator() const;

    /**
     * Get the framebuffer that the device wants to output to the screen
     * @return  Texture of the framebuffer to be displayed
     */
    virtual gfx::Texture* getFrontBuffer() = 0;
};

}  // namespace gpu
