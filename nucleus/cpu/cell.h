/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/cpu.h"

namespace cpu {

// Forward declarations
namespace frontend { namespace ppu { class Module; } }
namespace frontend { namespace spu { class Module; } }

class Cell : public CPU {
public:
    // Executable memory segments
    std::vector<frontend::ppu::Module*> ppu_modules;
    std::vector<frontend::spu::Module*> spu_modules;

    Cell(std::shared_ptr<mem::Memory> memory);
};

}  // namespace cpu
