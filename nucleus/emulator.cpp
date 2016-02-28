/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "emulator.h"
#include "nucleus/core/config.h"
#include "nucleus/audio/backend/list.h"
#include "nucleus/graphics/backend/list.h"
#include "nucleus/filesystem/filesystem_host.h"
#include "nucleus/filesystem/filesystem_virtual.h"
#include "nucleus/ui/ui.h"
#include "nucleus/cpu/cell.h"
#include "nucleus/gpu/list.h"
#include "nucleus/filesystem/utils.h"
#include "nucleus/loader/self.h"
#include "nucleus/logger/logger.h"
#include "nucleus/memory/memory.h"
#include "nucleus/system/lv2.h"

// Global emulator object
Emulator nucleus;

bool Emulator::initialize(const gfx::BackendParameters& params) {
    // Select graphics backend
    switch (config.graphicsBackend) {
#if defined(NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D11)
    case GRAPHICS_BACKEND_DIRECT3D11:
        graphics = std::make_shared<gfx::Direct3D11Backend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_GFXBACKEND_DIRECT3D12)
    case GRAPHICS_BACKEND_DIRECT3D12:
        graphics = std::make_shared<gfx::Direct3D12Backend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_GFXBACKEND_OPENGL)
    case GRAPHICS_BACKEND_OPENGL:
        graphics = std::make_shared<gfx::OpenGLBackend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_GFXBACKEND_VULKAN)
    case GRAPHICS_BACKEND_VULKAN:
        graphics = std::make_shared<gfx::VulkanBackend>();
        break;
#endif
    default:
        logger.warning(LOG_COMMON, "Unsupported graphics backend");
        return false;
    }

    // Select audio backend
    switch (config.audioBackend) {
#if defined(AUDIO_FEATURE_AUDIOBACKEND_COREAUDIO)
    case AUDIO_BACKEND_COREAUDIO:
        audio = std::make_shared<audio::CoreAudioBackend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_AUDIOBACKEND_OPENAL)
    case AUDIO_BACKEND_OPENAL:
        audio = std::make_shared<audio::OpenALBackend>();
        break;
#endif
#if defined(NUCLEUS_FEATURE_AUDIOBACKEND_XAUDIO2)
    case AUDIO_BACKEND_XAUDIO2:
        audio = std::make_shared<audio::XAudio2Backend>();
        break;
#endif
     default:
        logger.warning(LOG_COMMON, "Unsupported graphics backend");
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
        ui = std::make_shared<ui::UI>(graphics, params.width, params.height);
        if (!ui->initialize()) {
            logger.warning(LOG_COMMON, "Could not initialize user interface");
            return false;
        }
    }

    return true;
}

bool Emulator::load(const std::string& filepath) {
    // Initialize hardware
    memory = std::make_shared<mem::Memory>();
    cpu = std::make_shared<cpu::Cell>(memory);
    gpu = std::make_shared<gpu::RSX>(memory, graphics);
    sys = std::make_shared<sys::LV2>(memory, sys::LV2_DEX);

    // Initialize application filesystem devices
    const fs::Path& processPath = fs::getProcessPath(filepath);
    sys->vfs.registerDevice(new fs::HostPathDevice("/app_home/", processPath));

    // Load ELF/SELF file
    SELFLoader self;
    auto file = fs::HostFileSystem::openFile(filepath, fs::Read);
    if (!self.open(file.get())) {
        logger.error(LOG_COMMON, "Invalid file given.");
        return false;
    }

    self.load_elf(static_cast<sys::LV2*>(sys.get())->proc);
    if (self.getMachine() != EM_PPC64) {
        logger.error(LOG_COMMON, "Only PPC64 executables are allowed");
        return false;
    }

    auto entry = self.getEntry();
    static_cast<sys::LV2*>(sys.get())->init(entry);
    return true;
}

void Emulator::run() {
    cpu->run();
}

void Emulator::pause() {
    cpu->pause();
}

void Emulator::stop() {
    cpu->stop();
}

void Emulator::idle() {
    while (true) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&]{ return m_event; });

        // Process event
        switch (m_event) {
        case NUCLEUS_EVENT_RUN:
            cpu->run();
            break;
        case NUCLEUS_EVENT_PAUSE:
            cpu->pause();
            break;
        case NUCLEUS_EVENT_STOP:
            cpu->stop();
            return;
        case NUCLEUS_EVENT_CLOSE:
            return;
        default:
            logger.warning(LOG_COMMON, "Unknown event");
            break;
        }

        m_event = NUCLEUS_EVENT_NONE;
    }
}

void Emulator::task(EmulatorEvent evt) {
    m_event = evt;
    m_cv.notify_one();
}
