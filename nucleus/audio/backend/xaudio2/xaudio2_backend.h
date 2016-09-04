/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/audio/audio.h"

// Forward declarations
struct IXAudio2;
struct IXAudio2MasteringVoice;

namespace audio {
namespace xaudio2 {

class XAudio2Backend : public AudioBackend {
    IXAudio2* xaudio2;
    IXAudio2MasteringVoice* masteringVoice;

public:
    virtual bool initialize() override;

    virtual void setVolume(float value) override;
};

}  // namespace xaudio2
}  // namespace audio
