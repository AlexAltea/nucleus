/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/graphics.h"
#include "nucleus/ui/font.h"
#include "nucleus/ui/language.h"
#include "nucleus/ui/screen.h"
#include "nucleus/ui/surface.h"

#include <queue>
#include <thread>
#include <vector>

namespace ui {

class UI {
    // Graphics
    gfx::CommandQueue* cmdQueue;
    gfx::CommandBuffer* cmdBuffer;

    std::thread thread;
    std::vector<std::unique_ptr<Screen>> screens;
    std::queue<std::unique_ptr<Screen>> newScreens;

public:
    std::shared_ptr<gfx::IBackend> graphics;

    // Message translation database
    Language language;

    // Properties of the global window
    Surface surface;

    // Fonts
    std::unique_ptr<Font> fontRegular;
    std::unique_ptr<Font> fontBold;
    std::unique_ptr<Font> fontLight;

    // Constructor
    UI(std::shared_ptr<gfx::IBackend> graphics, gfx::CommandQueue* queue, Size width, Size height);

    // RSX connection
    bool rsxChanged = false;

    bool initialize();

    void task();

    /**
     * Pushes a new screen to the back of the screen array
     * @param[in]  screen  Screen to be passed
     */
    void pushScreen(std::unique_ptr<Screen>&& screen);

public:
    // Pipelines
    gfx::Pipeline* pipelineContainers;
    gfx::Pipeline* pipelineImages;
    gfx::Pipeline* pipelineText;

    // Inputs
    std::vector<WidgetContainerInput> dataContainers;
    std::vector<WidgetImageInput> dataImages;
    std::vector<WidgetTextInput> dataText;
    std::vector<gfx::Texture*> textureImages;
    std::vector<gfx::Texture*> textureText;

    // Vertex buffers
    std::unique_ptr<gfx::VertexBuffer> vtxBufferContainers;
    std::unique_ptr<gfx::VertexBuffer> vtxBufferImages;
    std::unique_ptr<gfx::VertexBuffer> vtxBufferText;

    // Clears all UI-related vertex and texture buffers
    void clearVtxBuffers();

    // Rendering methods
    void renderContainers();
    void renderImages();
    void renderText();

    /**
     * Pushes a new WidgetContainer input data to the buffer
     * @param[in]  input  WidgetContainer vertex data
     */
    void pushWidgetContainer(const WidgetContainerInput& input);

    /**
     * Pushes a new WidgetImage input data to the buffer
     * @param[in]  input    WidgetImage vertex data
     * @param[in]  texture  WidgetImage texture
     */
    void pushWidgetImage(const WidgetImageInput& input, gfx::Texture* texture);

    /**
     * Pushes a new WidgetText input data to the buffer
     * @param[in]  input    WidgetText vertex data
     * @param[in]  texture  WidgetText texture
     */
    void pushWidgetText(const WidgetTextInput& input, gfx::Texture* texture);
};

}  // namespace ui
