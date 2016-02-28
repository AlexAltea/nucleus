/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "xaudio2.h"
#include "nucleus/logger/logger.h"

// Load functions
#define LOAD_FUNCTION(function) \
    _##function = reinterpret_cast<decltype(&function)>(GetProcAddress(hmodule, #function)); \
    if (!_##function) { \
        logger.error(LOG_AUDIO, "Could not load XAudio2 function '%s'", #function); \
        return false; \
    }

namespace audio {
namespace xaudio2 {

// Declare functions
#define DECLARE_FUNCTION(function) decltype(&function) _##function;
#define FUNCTION DECLARE_FUNCTION
#include "xaudio2.inl"
#undef FUNCTION
#undef DECLARE_FUNCTION

bool initializeXAudio2() {
    HMODULE hmodule = LoadLibrary(XAUDIO2_DLL);

#define FUNCTION LOAD_FUNCTION
#include "xaudio2.inl"
#undef FUNCTION
    return true;
}

}  // namespace xaudio2
}  // namespace audio
