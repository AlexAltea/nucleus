/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/cpu.h"
#include "nucleus/cpu/hir/module.h"

#include <map>

namespace cpu {
namespace frontend {

// Forward declarations
class Function;

class Module {
public:
    CPU* parent;

    // HIR Module
    hir::Module* hirModule;

    // Starting address of this module
    U64 address = 0;

    // Number of bytes covered by this module
    U64 size = 0;

    // Name of this module
    std::string name;

    // List of functions
    std::map<U64, Function*> functions;

    // Constructor
    Module(CPU* parent) : parent(parent) {
        hirModule = new hir::Module();
    }

    // Destructor
    ~Module() {
        delete hirModule;
    }

    // Check whether an address is inside this module
    bool contains(U64 addr) const {
        const U64 from = address;
        const U64 to = address + size;
        return from <= addr && addr < to;
    }
};

}  // namespace frontend
}  // namespace cpu
