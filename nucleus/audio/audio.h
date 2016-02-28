/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

namespace audio {

class Backend {
public:
    virtual bool initialize() = 0;

    /**
     * Adjust global volume of the emitted audio. Remarks:
     *  - Volume levels are amplitude multipliers between -2^24 and +2^24.
     *  - Volume level of 1.0f means no attenuation or gain.
     *  - Volume level of 0.0f means silence.
     * @param[in]  value  Volume level
     */
    virtual void setVolume(float value) = 0;
};

}  // namespace audio
