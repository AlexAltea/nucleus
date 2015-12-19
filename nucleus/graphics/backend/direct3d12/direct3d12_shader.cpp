/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "direct3d12_shader.h"
#include "nucleus/assert.h"
#include "nucleus/format.h"
#include "nucleus/logger/logger.h"
#include "nucleus/graphics/hir/block.h"
#include "nucleus/graphics/hir/function.h"
#include "nucleus/graphics/hir/instruction.h"
#include "nucleus/graphics/hir/opcodes.h"

#include <iostream>

namespace gfx {
namespace direct3d12 {

using namespace gfx::hir;

// Conversion
const char* Direct3D12Shader::getType(Type type) {
    switch (type) {
    case TYPE_VOID: return "void";
    case TYPE_I16:  return "int";
    case TYPE_I32:  return "int";
    case TYPE_F16:  return "float";
    case TYPE_F32:  return "float";
    case TYPE_V128: return "float4";
    default:
        assert_always("Unimplemented case");
        return "UNIMPLEMENTED";
    }
}

std::string Direct3D12Shader::compile(Module* module) {
    std::string source;
    return source;
}

bool Direct3D12Shader::initialize(const ShaderDesc& desc) {
    const std::string source = compile(desc.module);
    return true;
}

}  // namespace direct3d12
}  // namespace gfx
