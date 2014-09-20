/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/cpu/ppu/ppu_fields.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

#include <string>

struct PPUInstruction
{
    char* name;
    void (*interpreter)(PPUFields, PPUThread&);
    //void (*recompiler)(PPUFields, PPUThread&);
    std::string (*disassembler)(PPUFields);
};

extern PPUInstruction s_tablePrimary[0x40];
extern PPUInstruction s_table4[0x40];
extern PPUInstruction s_table4_[0x800];
extern PPUInstruction s_table19[0x400];
extern PPUInstruction s_table30[0x8];
extern PPUInstruction s_table31[0x400];
extern PPUInstruction s_table58[0x4];
extern PPUInstruction s_table59[0x20];
extern PPUInstruction s_table62[0x4];
extern PPUInstruction s_table63[0x20];
extern PPUInstruction s_table63_[0x400];

void initTables();
