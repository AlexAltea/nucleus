/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/cpu/ppu/ppu_instruction.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

#include <string>

namespace cpu {
namespace ppu {

enum EntryType {
    ENTRY_INVALID,      // Entry does not match any PPU valid instruction or tables
    ENTRY_INSTRUCTION,  // Entry is an instruction
    ENTRY_TABLE,        // Entry is a table of entries
};

/**
 * PPU table entry
 */
struct Entry
{
    EntryType type;
    const char* name;
    void (*interpreter)(Instruction, PPUThread&);
    const Entry& (*caller)(Instruction);
};

extern Entry s_tablePrimary[0x40];
extern Entry s_table4[0x40];
extern Entry s_table4_[0x800];
extern Entry s_table19[0x400];
extern Entry s_table30[0x8];
extern Entry s_table31[0x400];
extern Entry s_table58[0x4];
extern Entry s_table59[0x20];
extern Entry s_table62[0x4];
extern Entry s_table63[0x20];
extern Entry s_table63_[0x400];

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

void initTables();

}  // namespace ppu
}  // namespace cpu
