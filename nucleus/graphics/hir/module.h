/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <vector>
#include <unordered_map>

namespace gfx {
namespace hir {

// Class declarations
class Function;
class Instruction;

class Module {
public:
    std::vector<Function*> functions;

    // Header
    std::vector<Instruction*> header;

    std::vector<Instruction*> hCapabilities;      // 1. All OpCapability
    std::vector<Instruction*> hExtensions;        // 2. All OpExtension
    std::vector<Instruction*> hImports;           // 3. All OpExtInstImport (at least one required)
    std::vector<Instruction*> hMemoryModel;       // 4. Single OpMemoryModel (exactly one required, member type is vector for consistency)
    std::vector<Instruction*> hEntryPoints;       // 5. All OpEntryPoint
    std::vector<Instruction*> hExecutions;        // 6. All OpExecutionMode
    std::vector<Instruction*> hDebugs;            // 7. All debug instructions: {OpString, OpSource*, Op*Name}
    std::vector<Instruction*> hAnnotation;        // 8. All annonation instructions: {Op*Decorate}
    std::vector<Instruction*> hConstsTypesGlobs;  // 9. All constant, type, global, declarations.

    // Mapping each result ID to the instruction with that result ID
    std::vector<Instruction*> idInstructions;

    Module();

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
