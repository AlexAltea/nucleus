/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/cpu.h"
#include "nucleus/cpu/frontend/ppu/ppu_decoder.h"

namespace cpu {

class Cell : public CPU {
public:
    // Executable memory segments
    std::vector<frontend::ppu::Module*> ppu_modules;

    Cell(std::shared_ptr<mem::Memory> memory);
};

}  // namespace cpu
