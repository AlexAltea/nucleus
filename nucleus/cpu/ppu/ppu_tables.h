/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/cpu/ppu/ppu_instruction.h"
#include "nucleus/cpu/ppu/ppu_thread.h"
#include "nucleus/cpu/ppu/analyzer/ppu_analyzer.h"

#include <string>

namespace cpu {
namespace ppu {

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
    void (*interpreter)(Instruction, PPUThread&);
    void (Analyzer::*analyzer)(Instruction);
};

// Instruction callers
const Entry& get_entry(Instruction code);
const Entry& get_table4  (Instruction code);
const Entry& get_table4_ (Instruction code);
const Entry& get_table19 (Instruction code);
const Entry& get_table30 (Instruction code);
const Entry& get_table31 (Instruction code);
const Entry& get_table58 (Instruction code);
const Entry& get_table59 (Instruction code);
const Entry& get_table62 (Instruction code);
const Entry& get_table63 (Instruction code);
const Entry& get_table63_(Instruction code);

}  // namespace ppu
}  // namespace cpu
