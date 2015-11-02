/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <string>
#include <vector>

namespace gfx {
namespace hir {

// Class declarations
class Function;

class Module {
public:
    std::vector<Function*> functions;

    // Generate IDs for child blocks and values
    S32 functionIdCounter = 0;

    /**
     * Add an existing function to the module
     * @param[in]  function  Function to be added to the module
     * @return               True on success
     */
    bool addFunction(Function* function);

    /**
     * Save a human-readable version of this HIR module
     * @return               String containing the readable version of this HIR module
     */
    std::string dump() const;
};

}  // namespace hir
}  // namespace gfx
