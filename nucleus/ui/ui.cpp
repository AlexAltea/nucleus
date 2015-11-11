/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ui.h"
#include "nucleus/core/config.h"
#include "nucleus/ui/screens/screen_logo.h"

// Platform specific drawing contexts
#ifdef NUCLEUS_PLATFORM_WINDOWS
#include "wrappers/windows/window.h"
extern Window* window;
#endif

namespace ui {

UI::UI(std::shared_ptr<gfx::IBackend> graphics, gfx::ICommandQueue* queue) :
    graphics(std::move(graphics)), queue(queue), language() {
}

bool UI::initialize() {
    language.open(config.language);

    thread = std::thread([this]{
        task();
    });

    return true;
}

void UI::task() {
    // Prepare output buffers
    gfx::HeapDesc desc = {};
    desc.type = gfx::HEAP_TYPE_CT;
    desc.size = 2;

    graphics->createHeap(desc);

    const float clearColor[] = {0.5f, 0.5f, 0.0f, 1.0f};
    gfx::ICommandBuffer* cmdBuffer = graphics->createCommandBuffer();
    gfx::IColorTarget* colorTarget = nullptr; // TODO
    // Prepare state
    /* TODO
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

    // Initial screen
    screens.push_back(std::make_unique<ScreenLogo>(this));

    while (true) {
        const gfx::Viewport viewport = { 0, 0, surfaceWidth, surfaceHeight };
        
        cmdBuffer->cmdSetViewports(1, &viewport);
        cmdBuffer->cmdClearColor(colorTarget, clearColor);

        // Display screens
        for (auto i = 0ULL; i < screens.size(); i++) {
            auto& screen = screens[i];
            screen->prologue();
            screen->render();
            screen->update();
            screen->epilogue();

            if (screen->finished) {
                screens.erase(screens.begin() + i--);
            }
        }

        queue->submit(cmdBuffer);
    }
}

void UI::pushScreen(std::unique_ptr<Screen>&& screen) {
    screens.push_back(std::move(screen));
}

}  // namespace ui
