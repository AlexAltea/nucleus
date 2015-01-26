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

void Function::analyze()
{
}

void Segment::analyze()
{
    // List of labels, stored as blocks.
    std::map<u32, Block> blocks;

    // Current basic block being processed
    Block current;

    // Basic Block Slicing: Classify them as starting or non-starting blocks
    for (u32 i = address; i < (address + size); i += 4) {
        const Instruction code = { nucleus.memory.read32(i) };

        // New block appeared
        if (code.is_valid() && current.address == 0) {
            if (blocks.find(i) != blocks.end()) {
                current = blocks[i];
            } else {
                current.address = i;
            }
        }

        // Block is corrupt
        if (current.address != 0 && !code.is_valid()) {
            current.address = 0;
        }

        // Block finished
        if (current.address != 0 && code.is_branch() && !code.is_call()) {
            if (code.is_branch_conditional()) {
                const u32 target_a = code.get_target(i);
                const u32 target_b = i + 4;
                blocks.emplace(target_a, Block{});
                blocks.emplace(target_b, Block{});
                blocks[target_a].address = target_a;
                blocks[target_b].address = target_b;
                blocks[target_a].initial = false;
                blocks[target_b].initial = false;
            }
            if (code.is_branch_unconditional()) {
                const u32 target = code.get_target(i);
                blocks.emplace(target, Block{});
                blocks[target].address = target;
                blocks[target].initial = false;
            }
            blocks[current.address] = current;
            current = Block{};
        }
    }

    // List the functions and analyze them
    for (const auto& item : blocks) {
        const Block& block = item.second;
        if (block.initial) {
            Function function(block.address);
            function.analyze();
            functions.push_back(function);
        }
    }
}

}  // namespace ppu
}  // namespace cpu
