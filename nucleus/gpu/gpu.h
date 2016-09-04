/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Forward declarations
namespace gfx { class Texture; }
namespace gfx { class GraphicsBackend; }

namespace gpu {

class GPU {
public:
    /**
     * Get the framebuffer that the device wants to output to the screen
     * @return  Texture of the framebuffer to be displayed
     */
    virtual gfx::Texture* getFrontBuffer() = 0;
};

}  // namespace gpu
