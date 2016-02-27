/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace cpu {
namespace backend {

class Register {
public:
    enum Type {
        TYPE_INVALID = 0,
        TYPE_INTEGER = (1 << 0),
        TYPE_FLOAT   = (1 << 1),
        TYPE_VECTOR  = (1 << 2),
    };

    unsigned code;
    unsigned size;
    Type type;

    Register() : code(0), size(0), type(TYPE_INVALID) {}
    Register(unsigned code, unsigned size, Type type) :
        code(code), size(size), type(type) {}

    bool is8Bit()  const { return size == 8; }
    bool is16Bit() const { return size == 16; }
    bool is32Bit() const { return size == 32; }
    bool is64Bit() const { return size == 64; }
};

class Assembler {
public:
    void* curAddr;
    Size curSize;

    void* codeAddr;
    Size codeSize;

    Assembler(Size codeSize = 4096, void* codeAddr = nullptr);

    template <typename T>
    void emit(T data) {
        *reinterpret_cast<T*>(curAddr) = data;
        curAddr = reinterpret_cast<void*>(reinterpret_cast<intptr_t>(curAddr) + sizeof(T));
        curSize += sizeof(T);
    }
    void emit8(U8 data)   { emit(data); }
    void emit16(U16 data) { emit(data); }
    void emit32(U32 data) { emit(data); }
    void emit64(U64 data) { emit(data); }
};

class Label {
public:
};

}  // namespace backend
}  // namespace cpu
