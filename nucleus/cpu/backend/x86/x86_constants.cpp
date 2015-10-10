/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_constants.h"

namespace cpu {
namespace backend {
namespace x86 {

// Utilities
bool isPowerOfTwo(S64 value) {
    return value && !(value & (value - 1));
}

int getLog2OfPowerOfTwo(S64 value) {
    int n = 0;
    int pow2n = 1;
    while (value & pow2n) {
        pow2n <<= 1;
        n += 1;
    }
    return n;
}

// General-Purpose Register constants
// TODO

// XMM constants
void getXmmConstant(X86Emitter& e, Xbyak::Xmm dest, const F32 constant) {
    if (!constant) {
        e.vpxor(dest, dest);
    } else {
        e.mov(e.eax, (U32&)constant);
        e.vmovd(dest, e.eax);
    }
}

void getXmmConstant(X86Emitter& e, Xbyak::Xmm dest, const F64 constant) {
    if (!constant) {
        e.vpxor(dest, dest);
    } else {
        e.mov(e.rax, (U64&)constant);
        e.vmovq(dest, e.rax);
    }
}

void getXmmConstant(X86Emitter& e, Xbyak::Xmm dest, const V128& constant) {
    if (constant.u64[0] == 0ULL && constant.u64[1] == 0ULL) {
        e.vpxor(dest, dest);
    } else if (constant.u64[0] == ~0ULL && constant.u64[1] == ~0ULL) {
        e.vpcmpeqd(dest, dest);
    } else {
        e.mov(e.rax, constant.u64[0]);
        e.mov(e.qword[e.rsp + 0], e.rax);
        e.mov(e.rax, constant.u64[1]);
        e.mov(e.qword[e.rsp + 8], e.rax);
        e.vmovdqa(dest, e.ptr[e.rsp]);
    }
}

void getXmmConstantCompI32(X86Emitter& e, Xbyak::Xmm dest, const S32 constant) {
    if (constant == 0) {
        e.vpxor(dest, dest);
    } else if (constant == -1) {
        e.vpcmpeqd(dest, dest);
    } else if (isPowerOfTwo(constant)) {
        int power = getLog2OfPowerOfTwo(constant);
        e.vpcmpeqd(dest, dest);
        e.vpsrld(dest, 31);
        if (power != 0) {
            e.vpslld(dest, power);
        }
    } else if (isPowerOfTwo(constant + 1)) {
        int power = getLog2OfPowerOfTwo(constant + 1);
        e.vpcmpeqd(dest, dest);
        e.vpsrld(dest, 32 - power);
    } else {
        e.mov(e.eax, constant);
        e.vmovd(dest, e.eax);
        e.vpshufd(dest, dest, 0);
    }
}

void getXmmConstantCompI64(X86Emitter& e, Xbyak::Xmm dest, const S64 constant) {
    if (constant == 0) {
        e.vpxor(dest, dest);
    } else if (constant == -1) {
        e.vpcmpeqd(dest, dest);
    } else if (isPowerOfTwo(constant)) {
        int power = getLog2OfPowerOfTwo(constant);
        e.vpcmpeqd(dest, dest);
        e.vpsrld(dest, 63);
        if (power != 0) {
            e.vpslld(dest, power);
        }
    } else if (isPowerOfTwo(constant + 1)) {
        int power = getLog2OfPowerOfTwo(constant + 1);
        e.vpcmpeqd(dest, dest);
        e.vpsrld(dest, 64 - power);
    } else {
        e.mov(e.rax, constant);
        e.vmovq(dest, e.rax);
        e.vpunpcklqdq(dest, dest);
    }
}

void getXmmConstantCompF32(X86Emitter& e, Xbyak::Xmm dest, const F32 constant) {
    if (constant == 0) {
        e.vpxor(dest, dest);
    } else {
        e.mov(e.eax, constant);
        e.movd(dest, e.eax);
        e.vshufps(dest, dest, 0);
    }
}

void getXmmConstantCompF64(X86Emitter& e, Xbyak::Xmm dest, const F64 constant) {
    if (constant == 0) {
        e.vpxor(dest, dest);
    } else {
        e.mov(e.rax, constant);
        e.movq(dest, e.rax);
        e.vshufpd(dest, dest, 0);
    }
}

}  // namespace x86
}  // namespace backend
}  // namespace cpu
