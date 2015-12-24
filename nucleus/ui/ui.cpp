/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ui.h"
#include "nucleus/core/config.h"
#include "nucleus/ui/screens/screen_logo.h"


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
    gfx::FenceDesc fenceDesc = {};
    gfx::Fence* fence = graphics->createFence(fenceDesc);

    const float clearColor[] = {0.157f, 0.157f, 0.360f, 1.0f};
    cmdBuffer = graphics->createCommandBuffer();

    gfx::ShaderDesc vertDesc = {};
    gfx::ShaderDesc fragDesc = {};
    vertDesc.type = SHADER_TYPE_VERTEX;
    fragDesc.type = SHADER_TYPE_PIXEL;
    auto* vertShader = graphics->createShader(vertDesc);
    auto* fragShader = graphics->createShader(fragDesc);

    gfx::PipelineDesc pipelineDesc = {};
    pipelineDesc.vs = vertShader;
    pipelineDesc.ps = fragShader;
    pipelineDesc.iaState.topology = gfx::TOPOLOGY_TRIANGLE_LIST;
    pipelineDesc.iaState.inputLayout = {
        { 0, gfx::FORMAT_R32G32B32A32, 0, 0, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 },
    };
    pipelineDesc.cbState.colorTarget[0] = { true, false,
        gfx::BLEND_SRC_ALPHA, gfx::BLEND_INV_SRC_ALPHA, gfx::BLEND_OP_ADD,
        gfx::BLEND_SRC_ALPHA, gfx::BLEND_INV_SRC_ALPHA, gfx::BLEND_OP_ADD,
        gfx::LOGIC_OP_NOOP,
        gfx::COLOR_WRITE_ENABLE_ALL
    };
    gfx::Pipeline* pipeline = graphics->createPipeline(pipelineDesc);

    // Initial screen
    screens.push_back(std::make_unique<ScreenLogo>(this));

    while (true) {
        const gfx::Viewport viewport = { 0, 0, surfaceWidth, surfaceHeight };
        
        cmdBuffer->reset();
        cmdBuffer->cmdBindPipeline(pipeline);
        cmdBuffer->cmdSetTargets(1, &graphics->screenBackBuffer, nullptr);
        cmdBuffer->cmdSetViewports(1, &viewport);
        cmdBuffer->cmdClearColor(graphics->screenBackBuffer, clearColor);

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
        cmdBuffer->finalize();

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
