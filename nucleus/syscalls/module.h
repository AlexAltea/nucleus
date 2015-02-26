/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/ppu/ppu_thread.h"
#include "nucleus/syscalls/syscall.h"

#include <string>
#include <unordered_map>
#include <vector>

struct Module {
    std::string name;
    std::unordered_map<u32, Syscall*> functions;

    Module(const std::string& name, std::unordered_map<u32, Syscall*> functions) : name(name), functions(functions) {};
};

class ModuleManager {
    std::vector<Module> m_modules;

public:
    ModuleManager();

    // Check if a certain library function is available for HLE
    bool find(const std::string& libraryName, u32 functionId);

    // Get function ID from the current thread and call it
    void call(cpu::ppu::State& state);
};
