/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"

namespace cpu {
namespace ppu {

void Analyzer::setFlag(AnalyzerEvent& reg, AnalyzerEvent evt)
{
    auto& regValue = (u8&)reg;
    if (regValue == REG_NONE && evt == REG_READ) {
        regValue |= REG_READ_ORIG;
    }
    regValue |= evt;
}

}  // namespace ppu
}  // namespace cpu
