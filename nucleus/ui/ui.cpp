/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ui.h"
#include "nucleus/core/config.h"
#include "nucleus/core/resource.h"
#include "nucleus/ui/screens/list.h"
#include "nucleus/ui/widgets/list.h"

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

    pipelineContainers = WidgetContainer::createPipeline(*graphics);
    pipelineImages = WidgetImage::createPipeline(*graphics);
    pipelineText = WidgetText::createPipeline(*graphics);

    // Initial screen
#if defined(NUCLEUS_PLATFORM_UWP)
    screens.push_back(std::make_unique<ScreenLogo>(this));
#else
    screens.push_back(std::make_unique<ScreenLogo>(this));
#endif
    while (true) {
        surface.update(graphics.get());
        const gfx::Viewport viewport = { 0, 0, surface.getWidth(), surface.getHeight() };
        const gfx::Rectangle scissor = { 0, 0, surface.getWidth(), surface.getHeight() };

        gfx::ResourceBarrier barrierBegin;
        barrierBegin.transition.resource = graphics->screenBackBuffer;
        barrierBegin.transition.before = gfx::RESOURCE_STATE_PRESENT;
        barrierBegin.transition.after = gfx::RESOURCE_STATE_COLOR_TARGET;
        cmdBuffer->cmdResourceBarrier(1, &barrierBegin);

        cmdBuffer->cmdSetTargets(1, &graphics->screenBackTarget, nullptr);
        cmdBuffer->cmdSetViewports(1, &viewport);
        cmdBuffer->cmdSetScissors(1, &scissor);
        cmdBuffer->cmdClearColor(graphics->screenBackTarget, clearColor);

        // Re-fill vertex buffers
        clearVtxBuffers();
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
        renderContainers();
        renderImages();
        renderText();

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
        cmdBuffer->reset();
    }
}

void UI::pushScreen(std::unique_ptr<Screen>&& screen) {
    newScreens.push(std::move(screen));
}

// Rendering
void UI::clearVtxBuffers() {
    dataContainers.clear();
    dataImages.clear();
    dataText.clear();
    textureImages.clear();
    textureText.clear();
}

void UI::renderContainers() {
    if (dataContainers.empty()) {
        return;
    }
    cmdBuffer->cmdBindPipeline(pipelineContainers);

    gfx::VertexBufferDesc vtxBufferDesc = {};
    vtxBufferDesc.size = dataContainers.size() * sizeof(WidgetContainerInput);
    auto* vtxBuffer = graphics->createVertexBuffer(vtxBufferDesc);
    vtxBufferContainers.reset(vtxBuffer);

    void* bufferAddr = vtxBuffer->map();
    memcpy(bufferAddr, dataContainers.data(), vtxBufferDesc.size);
    vtxBuffer->unmap();

    U32 offsets[] = { 0 };
    U32 strides[] = { sizeof(WidgetContainerInput) / 4 };
    cmdBuffer->cmdSetPrimitiveTopology(gfx::TOPOLOGY_TRIANGLE_STRIP);
    cmdBuffer->cmdSetVertexBuffers(0, 1, &vtxBuffer, offsets, strides);
    for (size_t i = 0; i < dataContainers.size(); i++) {
        cmdBuffer->cmdDraw(4 * i, 4, 0, 1);
    }
}

void UI::renderImages() {
    if (dataImages.empty()) {
        return;
    }
    cmdBuffer->cmdBindPipeline(pipelineImages);

    gfx::VertexBufferDesc vtxBufferDesc = {};
    vtxBufferDesc.size = dataImages.size() * sizeof(WidgetImageInput);
    auto* vtxBuffer = graphics->createVertexBuffer(vtxBufferDesc);
    vtxBufferImages.reset(vtxBuffer);

    void* bufferAddr = vtxBuffer->map();
    memcpy(bufferAddr, dataImages.data(), vtxBufferDesc.size);
    vtxBuffer->unmap();

    U32 offsets[] = { 0 };
    U32 strides[] = { sizeof(WidgetImageInput) / 4 };
    cmdBuffer->cmdSetPrimitiveTopology(gfx::TOPOLOGY_TRIANGLE_STRIP);
    cmdBuffer->cmdSetVertexBuffers(0, 1, &vtxBuffer, offsets, strides);
    gfx::Texture* texture = nullptr;
    for (size_t i = 0; i < dataImages.size(); i++) {
        if (texture != textureImages[i]) {
            texture = textureImages[i];
            cmdBuffer->cmdSetTexture(0, texture);
        }
        cmdBuffer->cmdDraw(4 * i, 4, 0, 1);
    }
}

void UI::renderText() {
    if (dataText.empty()) {
        return;
    }
    cmdBuffer->cmdBindPipeline(pipelineText);

    gfx::VertexBufferDesc vtxBufferDesc = {};
    vtxBufferDesc.size = dataText.size() * sizeof(WidgetTextInput);
    auto* vtxBuffer = graphics->createVertexBuffer(vtxBufferDesc);
    vtxBufferText.reset(vtxBuffer);

    void* bufferAddr = vtxBuffer->map();
    memcpy(bufferAddr, dataText.data(), vtxBufferDesc.size);
    vtxBuffer->unmap();

    U32 offsets[] = { 0 };
    U32 strides[] = { sizeof(WidgetTextInput) / 4 };
    cmdBuffer->cmdSetPrimitiveTopology(gfx::TOPOLOGY_TRIANGLE_STRIP);
    cmdBuffer->cmdSetVertexBuffers(0, 1, &vtxBuffer, offsets, strides);
    gfx::Texture* texture = nullptr;
    for (size_t i = 0; i < dataText.size(); i++) {
        if (texture != textureText[i]) {
            texture = textureText[i];
            cmdBuffer->cmdSetTexture(0, texture);
        }
        cmdBuffer->cmdDraw(4 * i, 4, 0, 1);
    }
}

void UI::pushWidgetContainer(const WidgetContainerInput& input) {
    dataContainers.push_back(input);
}
void UI::pushWidgetImage(const WidgetImageInput& input, gfx::Texture* texture) {
    dataImages.push_back(input);
    textureImages.push_back(texture);
}
void UI::pushWidgetText(const WidgetTextInput& input, gfx::Texture* texture) {
    dataText.push_back(input);
    textureText.push_back(texture);
}

}  // namespace ui
