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
#include <stack>
#include <thread>
#include <tuple>
#include <vector>

namespace ui {

class UI {
    // Graphics
    gfx::CommandQueue* cmdQueue;
    gfx::CommandBuffer* cmdBuffer;
    gfx::Heap* heapResources;

    std::thread thread;
    std::vector<std::unique_ptr<Screen>> screens;
    std::queue<std::unique_ptr<Screen>> newScreens;

public:
    std::shared_ptr<gfx::IBackend> graphics;
    
    // Events
    std::mutex eventMutex;
    std::stack<std::unique_ptr<Event>> events;

    // Message translation database
    Language language;

    // Properties of the global window
    Surface surface;

    // Fonts
    std::unique_ptr<Font> fontRegular;
    std::unique_ptr<Font> fontBold;
    std::unique_ptr<Font> fontLight;
    std::unique_ptr<Font> fontIcon;

    // Constructor
    UI(std::shared_ptr<gfx::IBackend> graphics, Size width, Size height);

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
    std::vector<std::tuple<gfx::Texture*, gfx::Heap*, bool>> textureImages;
    std::vector<std::tuple<gfx::Texture*, gfx::Heap*, bool>> textureText;

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
     * @param[in]  input          WidgetImage vertex data
     * @param[in]  texture        WidgetImage texture
     * @param[in]  isColorTarget  True if the specified texture is a color target
     */
    void pushWidgetImage(const WidgetImageInput& input, gfx::Texture* texture, gfx::Heap* heap, bool isColorTarget);

    /**
     * Pushes a new WidgetText input data to the buffer
     * @param[in]  input    WidgetText vertex data
     * @param[in]  texture  WidgetText texture
     */
    void pushWidgetText(const WidgetTextInput& input, gfx::Texture* texture, gfx::Heap* heap);
};

}  // namespace ui
