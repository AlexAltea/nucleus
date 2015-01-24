/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_analyzer.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_instruction.h"

#include <map>

namespace cpu {
namespace ppu {

void analyze_segment(u32 address, u32 size)
{
    Segment segment;
    segment.address = address;
    segment.size = size;

    std::map<u32, Block> blocks;

    // Current basic block being processed
    Block current;

    // Step 1: Basic Block Slicing / Control Flow Graph
    for (u32 i = address; i < (address + size); i += 4) {
        const Instruction code = { nucleus.memory.read32(i) };

        // Block is corrupt
        if (!code.is_valid() && current.address != 0) {
            current.address = 0;
        }

        // New block appeared
        if (code.is_valid() && current.address == 0) {
            if (blocks.find(i) != blocks.end()) {
                current = blocks[i];
            } else {
                current.address = i;
                current.size = 0;
            }
        }

        // Block continues
        if (code.is_valid() && current.address != 0) {
            current.size += 4;
        }

        // Block finished
        if (code.is_branch() && !code.is_call() && current.address != 0) {
            if (code.is_branch_conditional()) {
                const u32 target_a = code.get_target(i);
                const u32 target_b = i + 4;
                blocks.emplace(target_a, Block{});
                blocks.emplace(target_b, Block{});
                current.branch_a = &blocks[target_a];
                current.branch_b = &blocks[target_b];
                current.branch_a->initial = false;
                current.branch_b->initial = false;
            }
            if (code.is_branch_unconditional()) {
                const u32 target = code.get_target(i);
                blocks.emplace(target, Block{});
                current.branch_a = &blocks[target];
                current.branch_a->initial = false;
            }
            blocks[current.address] = current;
            current = Block{};
        }
    }

    // Step 2: Generate the function list
    for (auto& item : blocks) {
        const Block& block = item.second;
        if (block.initial) {
            Function function;
            function.block = block;
            segment.functions.push_back(function);
        }
    }
}

}  // namespace ppu
}  // namespace cpu
