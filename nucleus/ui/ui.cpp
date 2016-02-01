/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ui.h"
#include "nucleus/core/config.h"
#include "nucleus/core/resource.h"
#include "nucleus/graphics/frontend/shader_parser.h"
#include "nucleus/ui/screens/list.h"

namespace ui {

UI::UI(std::shared_ptr<gfx::IBackend> graphics, gfx::CommandQueue* queue, Size width, Size height) :
    graphics(std::move(graphics)), cmdQueue(queue), language(), surface(width, height) {
}

bool UI::initialize() {
    language.open(config.language);

    fontRegular = std::make_unique<Font>(core::RES_FONT_SOURCESANSPRO_REGULAR);
    fontBold = std::make_unique<Font>(core::RES_FONT_SOURCESANSPRO_BOLD);
    fontLight = std::make_unique<Font>(core::RES_FONT_SOURCESANSPRO_LIGHT);

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
    core::Resource resVS(core::RES_SHADER_UI_WIDGET_VS);
    core::Resource resPS(core::RES_SHADER_UI_WIDGET_PS);
    vertDesc.type = gfx::SHADER_TYPE_VERTEX;
    vertDesc.module = gfx::frontend::ShaderParser::parse(reinterpret_cast<const char*>(resVS.data), resVS.size);
    fragDesc.type = gfx::SHADER_TYPE_PIXEL;
    fragDesc.module = gfx::frontend::ShaderParser::parse(reinterpret_cast<const char*>(resPS.data), resPS.size);
    auto* vertShader = graphics->createShader(vertDesc);
    auto* fragShader = graphics->createShader(fragDesc);

    gfx::PipelineDesc pipelineDesc = {};
    pipelineDesc.vs = vertShader;
    pipelineDesc.ps = fragShader;
    pipelineDesc.iaState.topology = gfx::TOPOLOGY_TRIANGLE_STRIP;
    pipelineDesc.iaState.inputLayout = {
        { 0, gfx::FORMAT_R32G32B32A32, 0, 0, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 },
        { 1, gfx::FORMAT_R32G32B32A32, 0, 16, 0, 0, gfx::INPUT_CLASSIFICATION_PER_VERTEX, 0 },
    };
    pipelineDesc.cbState.colorTarget[0] = {
        true, false,
        gfx::BLEND_SRC_ALPHA, gfx::BLEND_INV_SRC_ALPHA, gfx::BLEND_OP_ADD,
        gfx::BLEND_SRC_ALPHA, gfx::BLEND_INV_SRC_ALPHA, gfx::BLEND_OP_ADD,
        gfx::LOGIC_OP_NOOP,
        gfx::COLOR_WRITE_ENABLE_ALL
    };
    gfx::Pipeline* pipeline = graphics->createPipeline(pipelineDesc);

    // Initial screen
#if defined(NUCLEUS_PLATFORM_UWP)
    screens.push_back(std::make_unique<ScreenMain>(this));
#else
    screens.push_back(std::make_unique<ScreenLogo>(this));
#endif

    while (true) {
        surface.update(graphics.get());
        const gfx::Viewport viewport = { 0, 0, surface.getWidth(), surface.getHeight() };
        const gfx::Rectangle scissor = { 0, 0, surface.getWidth(), surface.getHeight() };

        cmdBuffer->reset();
        cmdBuffer->cmdBindPipeline(pipeline);

        gfx::ResourceBarrier barrierBegin;
        barrierBegin.transition.resource = graphics->screenBackBuffer;
        barrierBegin.transition.before = gfx::RESOURCE_STATE_PRESENT;
        barrierBegin.transition.after = gfx::RESOURCE_STATE_COLOR_TARGET;
        cmdBuffer->cmdResourceBarrier(1, &barrierBegin);

        cmdBuffer->cmdSetTargets(1, &graphics->screenBackTarget, nullptr);
        cmdBuffer->cmdSetViewports(1, &viewport);
        cmdBuffer->cmdSetScissors(1, &scissor);
        cmdBuffer->cmdClearColor(graphics->screenBackTarget, clearColor);

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
        if (!widgetVtxBuffer.empty()) {
            gfx::VertexBufferDesc vtxBufferDesc = {};
            vtxBufferDesc.size = widgetVtxBuffer.size() * sizeof(WidgetInput);
            gfx::VertexBuffer* vtxBuffer = graphics->createVertexBuffer(vtxBufferDesc);

            void* bufferAddr = vtxBuffer->map();
            memcpy(bufferAddr, widgetVtxBuffer.data(), vtxBufferDesc.size);
            vtxBuffer->unmap();

            U32 offsets[] = {0};
            U32 strides[] = { sizeof(WidgetInput) / 4 };
            cmdBuffer->cmdSetPrimitiveTopology(gfx::TOPOLOGY_TRIANGLE_STRIP);
            cmdBuffer->cmdSetVertexBuffers(0, 1, &vtxBuffer, offsets, strides);
            for (size_t i = 0; i < widgetVtxBuffer.size(); i++) {
                cmdBuffer->cmdDraw(4 * i, 4, 0, 1);
            }
        }

        // Add new screens
        while (!newScreens.empty()) {
            auto& screen = newScreens.front();
            screens.push_back(std::move(screen));
            newScreens.pop();
        }

        gfx::ResourceBarrier barrierEnd;
        barrierEnd.transition.resource = graphics->screenBackBuffer;
        barrierEnd.transition.before = gfx::RESOURCE_STATE_COLOR_TARGET;
        barrierEnd.transition.after = gfx::RESOURCE_STATE_PRESENT;
        cmdBuffer->cmdResourceBarrier(1, &barrierEnd);
        cmdBuffer->finalize();

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
    widgetVtxBuffer.push_back(input);
}

}  // namespace ui
