/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "nucleus.h"
#include "nucleus/core/config.h"
#include "nucleus/debugger/debugger.h"
#include "nucleus/emulator.h"
#include "nucleus/ui/ui.h"

#include "nucleus/audio/backend/list.h"
#include "nucleus/graphics/backend/list.h"

#include <iostream>

#if !defined(NUCLEUS_BUILD_TEST)

using namespace audio;
using namespace gfx;
using namespace ui;

struct Nucleus {
    std::unique_ptr<AudioBackend> audio;
    std::unique_ptr<GraphicsBackend> graphics;
    std::unique_ptr<UI> ui;
};

// Global Nucleus object
Nucleus nucleus;

#if defined(NUCLEUS_TARGET_LINUX)
void nucleusPrepare(Display* display, Window window, int width, int height) {
    gfx::BackendParameters params = {};
    params.display = display;
    params.window = window;
    params.width = width;
    params.height = height;
    nucleus.initialize(params);
}
#elif defined(NUCLEUS_TARGET_UWP)
void nucleusPrepare(IUnknown* window, int width, int height) {
    gfx::BackendParameters params = {};
    params.window = window;
    params.width = width;
    params.height = height;
    nucleus.initialize(params);
}
#elif defined(NUCLEUS_TARGET_WINDOWS)
void nucleusPrepare(HWND hwnd, HDC hdc, int width, int height) {
    gfx::BackendParameters params = {};
    params.hdc = hdc;
    params.hwnd = hwnd;
    params.width = width;
    params.height = height;
    nucleus.initialize(params);
}
#endif

void nucleusConfigure(int argc, char **argv) {
    if (argc <= 1) {
        std::cout
            << "Nucleus " NUCLEUS_VERSION ": Multi-platform emulator.\n"
            << "Usage: nucleus [arguments] path/to/app\n"
            << "Arguments:\n"
            << "  --console    Avoids the Nucleus UI window, disabling GPU backends.\n"
            << "  --debugger   Create a Nerve backend debugging server.\n"
            << "               More information at: http://alexaltea.github.io/nerve/ \n"
            << std::endl;
    }
    config.parseArguments(argc, argv);
}

void nucleusInitialize() {
    // Select graphics backend
    auto& graphics = nucleus.graphics;
    switch (config.graphicsBackend) {
#if defined(NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D11)
    case GRAPHICS_BACKEND_DIRECT3D11:
        graphics = std::make_unique<gfx::Direct3D11Backend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D12)
    case GRAPHICS_BACKEND_DIRECT3D12:
        graphics = std::make_unique<gfx::Direct3D12Backend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_GFXBACKEND_OPENGL)
    case GRAPHICS_BACKEND_OPENGL:
        graphics = std::make_unique<gfx::OpenGLBackend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_GFXBACKEND_VULKAN)
    case GRAPHICS_BACKEND_VULKAN:
        graphics = std::make_unique<gfx::VulkanBackend>();
        break;
#endif
    default:
        logger.warning(LOG_COMMON, "Unsupported graphics backend");
        return false;
    }

    // Select audio backend
    auto& audio = nucleus.audio;
    switch (config.audioBackend) {
#if defined(AUDIO_FEATURE_AUDIOBACKEND_COREAUDIO)
    case AUDIO_BACKEND_COREAUDIO:
        audio = std::make_unique<audio::CoreAudioBackend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_AUDIOBACKEND_OPENAL)
    case AUDIO_BACKEND_OPENAL:
        audio = std::make_unique<audio::OpenALBackend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_AUDIOBACKEND_XAUDIO2)
    case AUDIO_BACKEND_XAUDIO2:
        audio = std::make_unique<audio::XAudio2Backend>();
        break;
#endif
     default:
        logger.warning(LOG_COMMON, "Unsupported audio backend");
        return false;
    }

    // Initialize backends
    if (!graphics->initialize(params)) {
        logger.warning(LOG_COMMON, "Could not initialize graphics backend");
        return false;
    }
    if (!audio->initialize()) {
        logger.warning(LOG_COMMON, "Could not initialize audio backend");
        return false;
    }

    if (!config.console) {
        ui = std::make_unique<ui::UI>(graphics, params.width, params.height);
        if (!ui->initialize()) {
            logger.warning(LOG_COMMON, "Could not initialize user interface");
            return false;
        }
    }

    return true;
}
    // Start debugger
    if (config.debugger) {
        debugger.init();
        std::cerr << "Debugger listening on 127.0.0.1:8000" << std::endl;
    }

    // Start emulator
    if (!config.boot.empty()) {
        nucleus.load(config.boot);
        nucleus.run();
        nucleus.idle();
    }
    return 0;
}

void nucleusFinalize() {
    //nucleus.task(NUCLEUS_EVENT_CLOSE);
}

// Events
void nucleusOnWindowSizeChanged(unsigned int width, unsigned int height) {
    auto* ui = nucleus.ui.get();
    if (!ui) {
        return;
    }
    ui::Surface& surface = ui->surface;
    surface.setWidth(width);
    surface.setHeight(height);
}

void nucleusOnMouseClick(int x, int y) {
    auto* ui = nucleus.ui.get();
    if (!ui) {
        return;
    }
    std::lock_guard<std::mutex> lock(ui->eventMutex);
    auto evt = std::make_unique<ui::MouseEvent>(ui::Event::TYPE_MOUSE_CLICK);
    evt->button = ui::MouseEvent::BUTTON_LEFT;
    evt->x = x;
    evt->y = y;
    ui->events.emplace(std::move(evt));
}

void nucleusOnMouseDoubleClick(int x, int y) {
    auto* ui = nucleus.ui.get();
    if (!ui) {
        return;
    }
    std::lock_guard<std::mutex> lock(ui->eventMutex);
    auto evt = std::make_unique<ui::MouseEvent>(ui::Event::TYPE_MOUSE_DBLCLICK);
    evt->button = ui::MouseEvent::BUTTON_LEFT;
    evt->x = x;
    evt->y = y;
    ui->events.emplace(std::move(evt));
}

void nucleusOnMouseMove(int x, int y) {
    auto* ui = nucleus.ui.get();
    if (!ui) {
        return;
    }
    std::lock_guard<std::mutex> lock(ui->eventMutex);
    auto evt = std::make_unique<ui::MouseEvent>(ui::Event::TYPE_MOUSE_MOVE);
    evt->button = ui::MouseEvent::BUTTON_LEFT;
    evt->x = x;
    evt->y = y;
    ui->events.emplace(std::move(evt));
}

void nucleusOnMouseWheel(int x, int y, int delta) {
    auto* ui = nucleus.ui.get();
    if (!ui) {
        return;
    }
    std::lock_guard<std::mutex> lock(ui->eventMutex);
    auto evt = std::make_unique<ui::MouseWheelEvent>();
    evt->x = x;
    evt->y = y;
    evt->delta = delta;
    ui->events.emplace(std::move(evt));
}

void nucleusOnKeyDown(int keycode) {
    auto* ui = nucleus.ui.get();
    if (!ui) {
        return;
    }
    std::lock_guard<std::mutex> lock(ui->eventMutex);
    auto evt = std::make_unique<ui::KeyEvent>(ui::Event::TYPE_KEY_DOWN);
    evt->keycode = keycode;
    ui->events.emplace(std::move(evt));
}

void nucleusOnKeyUp(int keycode) {
    auto* ui = nucleus.ui.get();
    if (!ui) {
        return;
    }
    std::lock_guard<std::mutex> lock(ui->eventMutex);
    auto evt = std::make_unique<ui::KeyEvent>(ui::Event::TYPE_KEY_UP);
    evt->keycode = keycode;
    ui->events.emplace(std::move(evt));
}

// Drag-and-Drop events
void nucleusOnDragEnter() {
}

void nucleusOnDragOver() {
}

void nucleusOnDragLeave() {
}

void nucleusOnDrop() {
}

#endif  // !defined(NUCLEUS_BUILD_TEST)
