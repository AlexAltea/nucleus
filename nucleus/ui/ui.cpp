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
    graphics(std::move(graphics)), cmdQueue(queue), language() {
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
    cmdBuffer = graphics->createCommandBuffer();
    gfx::ColorTarget* colorTarget = graphics->screenBackBuffer;

    // Prepare state
    /* TODO
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

    gfx::ShaderDesc vertDesc = {};
    gfx::ShaderDesc fragDesc = {};
    vertDesc.type = SHADER_TYPE_VERTEX;
    fragDesc.type = SHADER_TYPE_PIXEL;
    auto* vertShader = graphics->createShader(vertDesc);
    auto* fragShader = graphics->createShader(fragDesc);

    gfx::PipelineDesc pipelineDesc = {};
    pipelineDesc.vs = vertShader;
    pipelineDesc.ps = fragShader;
    pipelineDesc.iaState.inputLayout = {
        { 1, gfx::DATA_FORMAT_R32G32B32, 0, 0, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 },
    };
    gfx::Pipeline* pipeline = graphics->createPipeline(pipelineDesc);

    // Initial screen
    screens.push_back(std::make_unique<ScreenLogo>(this));

    while (true) {
        const gfx::Viewport viewport = { 0, 0, surfaceWidth, surfaceHeight };
        
        cmdBuffer->reset();
        cmdBuffer->cmdBindPipeline(pipeline);
        cmdBuffer->cmdSetViewports(1, &viewport);
        cmdBuffer->cmdClearColor(colorTarget, clearColor);

        // Vertex buffer
        widgetVtxBuffer.clear();

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

        // Render widgets
        gfx::VertexBufferDesc vtxBufferDesc = {};
        vtxBufferDesc.size = widgetVtxBuffer.size() * sizeof(WidgetInput);
        gfx::VertexBuffer* vtxBuffer = graphics->createVertexBuffer(vtxBufferDesc);

        void* bufferAddr = vtxBuffer->map();
        memcpy(bufferAddr, widgetVtxBuffer.data(), vtxBufferDesc.size);
        vtxBuffer->unmap();

        U32 offsets[] = {0};
        U32 strides[] = {16};
        cmdBuffer->cmdSetPrimitiveTopology(gfx::TOPOLOGY_TRIANGLE_LIST);
        cmdBuffer->cmdSetVertexBuffers(0, 1, &vtxBuffer, offsets, strides);
        cmdBuffer->cmdDraw(0, 3 * widgetVtxBuffer.size(), 0, 1);

        // Add new screens
        while (!newScreens.empty()) {
            auto& screen = newScreens.front();
            screens.push_back(std::move(screen));
            newScreens.pop();
        }

        cmdQueue->submit(cmdBuffer, fence);
        fence->wait();

        graphics->doSwapBuffers();
    }
}

void UI::pushScreen(std::unique_ptr<Screen>&& screen) {
    newScreens.push(std::move(screen));
}

// Rendering
void UI::renderWidget(const WidgetInput& input) {
    const U32 offset = sizeof(WidgetInput) * widgetVtxBuffer.size();
    widgetVtxBuffer.push_back(input);
}

}  // namespace ui
