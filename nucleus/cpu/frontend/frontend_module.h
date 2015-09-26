/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/frontend_function.h"

#include <map>

namespace cpu {
namespace frontend {

// Class declarations
template <typename TAddr>
class Function;

template <typename TAddr>
class Module {
public:
    // HIR Module
    hir::Module* hirModule;

    // Starting address of this module
    TAddr address = 0;

    // Number of bytes covered by this module
    TAddr size = 0;

    // Name of this module
    std::string name;

    // List of functions
    std::map<TAddr, Function<TAddr>*> functions;

    // Constructor
    Module() {
        hirModule = new hir::Module();
    }

    // Destructor
    ~Module() {
        delete hirModule;
    }

    // Check whether an address is inside this module
    bool contains(TAddr addr) const {
        const TAddr from = address;
        const TAddr to = address + size;
        return from <= addr && addr < to;
    }
};

}  // namespace frontend
}  // namespace cpu
