/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/ppu/ppu_thread.h"
#include "nucleus/system/scei/cellos/syscall.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace sys {

// Forward declarations
class LV2;

struct Module {
    std::string name;
    std::unordered_map<U32, Syscall*> functions;

    Module(const std::string& name, std::unordered_map<U32, Syscall*> functions) : name(name), functions(functions) {};
};

class ModuleManager {
    LV2* parent;

    std::vector<Module> modules;

public:
    ModuleManager(LV2* parent);

    // Check if a certain library function is available for HLE
    bool find(const std::string& libraryName, U32 functionId);

    // Get function ID from the current thread and call it
    void call(cpu::frontend::ppu::PPUState& state);
    void call(cpu::frontend::ppu::PPUState& state, U32 fnid);
};

}  // namespace sys
