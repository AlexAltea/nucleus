/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "function.h"

namespace cpu {
namespace hir {

Block Function::getEntryBlock()
{
    return Block{ &(function->getEntryBlock()) };
}

}  // namespace hir
}  // namespace cpu
