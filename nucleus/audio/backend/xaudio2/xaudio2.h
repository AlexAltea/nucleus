/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <xaudio2.h>
#include <Windows.h>

namespace audio {
namespace xaudio2 {

// Declare functions
#define DECLARE_FUNCTION(function) extern decltype(&function) _##function;
#define FUNCTION DECLARE_FUNCTION
#include "xaudio2.inl"
#undef FUNCTION
#undef DECLARE_FUNCTION

bool initializeXAudio2();

}  // namespace xaudio2
}  // namespace audio
