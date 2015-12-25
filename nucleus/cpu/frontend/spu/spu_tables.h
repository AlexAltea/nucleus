/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/spu/spu_instruction.h"
#include "nucleus/cpu/frontend/spu/recompiler/spu_recompiler.h"

#include <string>

namespace cpu {
namespace spu {

enum EntryType {
    ENTRY_INVALID = 0,  // Entry does not match any PPU valid instruction or tables
    ENTRY_INSTRUCTION,  // Entry is an instruction
    ENTRY_TABLE,        // Entry is a table of entries
};

/**
 * PPU table entry
 */
struct Entry
{
    EntryType type;

    // Table data
    const Entry& (*caller)(Instruction);

    // Instruction data
    const char* name;
    void (Recompiler::*recompile)(Instruction);
};

// Instruction callers
const Entry& get_entry(Instruction code);

}  // namespace spu
}  // namespace cpu
