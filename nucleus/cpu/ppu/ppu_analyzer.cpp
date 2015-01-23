/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_instruction.h"

#include <vector>

namespace cpu {
namespace ppu {

void analyze_segment(u32 address, u32 size)
{
    std::vector<Block> blocks;

    // Current basic block being processed
    Block current{};

    // Step 1: Basic block slicing
    for (u32 i = address; i < (address + size); i += 4 ) {
        const Instruction code = { nucleus.memory.read32(i) };

        // Block is corrupt
        if (!code.is_valid() && current.address != 0) {
            current.address = 0;
        }

        // New block appeared
        if (code.is_valid() && current.address == 0) {
            current.address = i;
        }

        // Block continues
        if (code.is_valid() && current.address != 0) {
            current.size += 4;
        }

        // Block finished
        if (code.is_branch() && !code.is_call() && current.address != 0) {
            blocks.push_back(current);
            current.address = 0;
        }
    }
}

}  // namespace ppu
}  // namespace cpu
