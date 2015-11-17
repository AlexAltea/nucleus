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

UI::UI(std::shared_ptr<gfx::IBackend> graphics, gfx::CommandQueue* queue) :
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
    gfx::HeapDesc heapDesc = {};
    heapDesc.type = gfx::HEAP_TYPE_CT;
    heapDesc.size = 2;
    graphics->createHeap(heapDesc);

    gfx::FenceDesc fenceDesc = {};
    gfx::Fence* fence = graphics->createFence(fenceDesc);

    const float clearColor[] = {0.5f, 0.5f, 0.0f, 1.0f};
    gfx::CommandBuffer* cmdBuffer = graphics->createCommandBuffer();
    gfx::ColorTarget* colorTarget = graphics->screenBackBuffer;

    // Prepare state
    /* TODO
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

    // Initial screen
    screens.push_back(std::make_unique<ScreenLogo>(this));

    while (true) {
        const gfx::Viewport viewport = { 0, 0, surfaceWidth, surfaceHeight };
        
        cmdBuffer->reset();
        cmdBuffer->cmdSetViewports(1, &viewport);
        cmdBuffer->cmdClearColor(colorTarget, clearColor);

        // Display screens
        for (auto i = 0ULL; i < screens.size(); i++) {
            auto& screen = screens[i];
            screen->prologue();
            screen->render(cmdBuffer);
            screen->update();
            screen->epilogue();

            if (screen->finished) {
                screens.erase(screens.begin() + i--);
            }
        }

        // Add new screens
        while (!newScreens.empty()) {
            auto& screen = newScreens.front();
            screens.push_back(std::move(screen));
            newScreens.pop();
        }

        queue->submit(cmdBuffer, fence);
        fence->wait();

        graphics->doSwapBuffers();
    }
}

void UI::pushScreen(std::unique_ptr<Screen>&& screen) {
    newScreens.push(std::move(screen));
}

}  // namespace ui
