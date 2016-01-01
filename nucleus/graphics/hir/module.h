/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/graphics/hir/type.h"

#include <string>
#include <vector>

namespace gfx {
namespace hir {

// Class declarations
class Function;
class Value;

class Module {
public:
    std::vector<Function*> functions;
    std::vector<Value*> inputs;
    std::vector<Value*> outputs;

    // Generate IDs for child blocks and values
    S32 functionIdCounter = 0;
    
    /**
     * Set pipeline inputs for this module
     * @param[in]  inputTypes  Pipeline input types
     * @return                 True on success
     */
    bool setInputs(std::vector<Type> inputTypes);

    /**
     * Set pipeline outputs for this module
     * @param[in]  outputTypes  Pipeline output types
     * @return                  True on success
     */
    bool setOutputs(std::vector<Type> outputTypes);

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
