/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Backends
#ifdef NUCLEUS_FEATURE_AUDIOBACKEND_COREAUDIO
#include "nucleus/audio/backend/coreaudio/coreaudio_backend.h"
#endif
#ifdef NUCLEUS_FEATURE_AUDIOBACKEND_OPENAL
#include "nucleus/audio/backend/openal/openal_backend.h"
#endif
#ifdef NUCLEUS_FEATURE_AUDIOBACKEND_XAUDIO2
#include "nucleus/audio/backend/xaudio2/xaudio2_backend.h"
#endif

namespace gfx {

// Shorthands
#ifdef NUCLEUS_FEATURE_AUDIOBACKEND_COREAUDIO
using CoreAudioBackend = coreaudio::CoreAudioBackend;
#endif
#ifdef NUCLEUS_FEATURE_AUDIOBACKEND_OPENAL
using OpenALBackend = openal::OpenALBackend;
#endif
#ifdef NUCLEUS_FEATURE_AUDIOBACKEND_XAUDIO2
using XAudio2Backend = xaudio2::XAudio2Backend;
#endif

}  // namespace gfx
