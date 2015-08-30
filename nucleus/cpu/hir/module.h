/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <string>
#include <vector>

namespace cpu {
namespace hir {

class Function;

class Module {
public:
    std::vector<Function*> functions;


    // Frontend methods

    /**
     * Add an existing function to the module
     * @param[in]  function  Function to be added to the module
     * @return               True on success
     */
    bool addFunction(Function* function);

    /**
     * Add an existing variable to the module
     * @param[in]  variable  Variable to be added to the module
     * @return               True on success
     */
    bool addVariable();


    // Backend methods

    /**
     * Compile all defined functions in the module
     * @return           True on success
     */
    bool compile();


    // Load-Saving methods

    /**
     * Save a readable version of the IR
     * @param[im]  path  Path where the dump file is to be saved
     * @return           True on success
     */
    bool dump(const std::string& path);

    /**
     * Load module from file
     * @param[im]  path  Path where the module file is located
     * @return           True on success
     */
    bool load(const std::string& path);

    /**
     * Save module to file
     * @param[im]  path  Path where the module file is to be saved
     * @return           True on success
     */
    bool save(const std::string& path);
};

}  // namespace hir
}  // namespace cpu
