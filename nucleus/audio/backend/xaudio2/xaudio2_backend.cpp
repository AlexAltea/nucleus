/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "xaudio2_backend.h"
#include "nucleus/audio/backend/xaudio2/xaudio2.h"
#include "nucleus/logger/logger.h"

namespace audio {
namespace xaudio2 {

bool XAudio2Backend::initialize() {
    HRESULT hr;
    if (!initializeXAudio2()) {
        logger.error(LOG_AUDIO, "XAudio2Backend::initialize: Could not load XAudio2 dynamic library");
        return false;
    }

    hr = _XAudio2Create(&xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        logger.error(LOG_AUDIO, "XAudio2Backend::initialize: XAudio2Create failed (0x%X)", hr);
        return false;
    }

    hr = xaudio2->CreateMasteringVoice(&masteringVoice);
    if (FAILED(hr)) {
        logger.error(LOG_AUDIO, "XAudio2Backend::initialize: IXAudio2::CreateMasteringVoice failed (0x%X)", hr);
        return false;
    }
    return true;
}

void XAudio2Backend::setVolume(float value) {
    if (masteringVoice) {
        masteringVoice->SetVolume(value);
    }
}

}  // namespace xaudio2
}  // namespace audio
