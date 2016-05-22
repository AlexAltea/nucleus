/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/ppu/ppu_instruction.h"
#include "nucleus/cpu/frontend/ppu/analyzer/ppu_analyzer.h"
#include "nucleus/cpu/frontend/ppu/translator/ppu_translator.h"

#include <string>

namespace cpu {
namespace frontend {
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
    void (Analyzer::*analyze)(Instruction);
    void (Translator::*recompile)(Instruction);
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
}  // namespace frontend
}  // namespace cpu
