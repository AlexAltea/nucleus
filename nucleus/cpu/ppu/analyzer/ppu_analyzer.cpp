/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"

namespace cpu {
namespace ppu {

void Analyzer::setFlag(AnalyzerEvent& reg, AnalyzerEvent evt)
{
    // If the register was already written, the original register cannot be read
    if (reg != REG_WRITE) {
        reg = evt;
    }
}

}  // namespace ppu
}  // namespace cpu
