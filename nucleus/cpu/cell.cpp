/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "cell.h"

namespace cpu {

Cell::Cell(std::shared_ptr<mem::Memory> memory) : CPU(std::move(memory)) {
}

}  // namespace cpu
