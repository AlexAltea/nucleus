/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_recompiler.h"

namespace cpu {
namespace ppu {

llvm::Value* Recompiler::getGPR(int reg)
{
    // TODO: ?
    return nullptr;
}

void Recompiler::setGPR(int reg, llvm::Value* value)
{
    // TODO: ?
    return;
}

llvm::Value* Recompiler::getFPR(int reg)
{
    // TODO: ?
    return nullptr;
}

void Recompiler::setFPR(int reg, llvm::Value* value)
{
    // TODO: ?
    return;
}

llvm::Value* getVR_u8(int reg)
{
    // TODO: ?
    return nullptr;
}

llvm::Value* getVR_u16(int reg)
{
    // TODO: ?
    return nullptr;
}

llvm::Value* getVR_u32(int reg)
{
    // TODO: ?
    return nullptr;
}

llvm::Value* getVR_f32(int reg)
{
    // TODO: ?
    return nullptr;
}

void setVR(int reg, llvm::Value* value)
{
    // TODO: ?
    return;
}

}  // namespace ppu
}  // namespace cpu
