/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "x86_proxy.h"
#include "nucleus/cpu/backend/x86/x86_assembler.h"

using namespace Xbyak;
using namespace cpu::frontend::x86;

#define XORSWAP(RA, RB) \
    a.xor_(RA, RB); \
    a.xor_(RB, RA); \
    a.xor_(RA, RB);

namespace cpu {namespace native {
namespace x86 {

void setStateAndEnter64(X86State state, void* target) {
    X86Assembler a(4096);
    
    // Copy GPRs
    a.mov(a.rax, reinterpret_cast<uintptr_t>(&state));
    XORSWAP(a.rcx, a.qword[a.rax + offsetof(X86State, r[0x2])]);
    XORSWAP(a.rdx, a.qword[a.rax + offsetof(X86State, r[0x2])]);
    XORSWAP(a.rbx, a.qword[a.rax + offsetof(X86State, r[0x3])]);
    XORSWAP(a.rsp, a.qword[a.rax + offsetof(X86State, r[0x4])]);
    XORSWAP(a.rbp, a.qword[a.rax + offsetof(X86State, r[0x5])]);
    XORSWAP(a.rsi, a.qword[a.rax + offsetof(X86State, r[0x6])]);
    XORSWAP(a.rdi, a.qword[a.rax + offsetof(X86State, r[0x7])]);
    XORSWAP(a.r8,  a.qword[a.rax + offsetof(X86State, r[0x8])]);
    XORSWAP(a.r9,  a.qword[a.rax + offsetof(X86State, r[0x9])]);
    XORSWAP(a.r10, a.qword[a.rax + offsetof(X86State, r[0xA])]);
    XORSWAP(a.r11, a.qword[a.rax + offsetof(X86State, r[0xB])]);
    XORSWAP(a.r12, a.qword[a.rax + offsetof(X86State, r[0xC])]);
    XORSWAP(a.r13, a.qword[a.rax + offsetof(X86State, r[0xD])]);
    XORSWAP(a.r14, a.qword[a.rax + offsetof(X86State, r[0xE])]);
    XORSWAP(a.r15, a.qword[a.rax + offsetof(X86State, r[0xF])]);

    // Copy
    a.call(target);

    // Restore GPRs
    a.mov(a.rax, U64(&state));
    XORSWAP(a.rcx, a.qword[a.rax + offsetof(X86State, r[0x1])]);
    XORSWAP(a.rdx, a.qword[a.rax + offsetof(X86State, r[0x2])]);
    XORSWAP(a.rbx, a.qword[a.rax + offsetof(X86State, r[0x3])]);
    XORSWAP(a.rsp, a.qword[a.rax + offsetof(X86State, r[0x4])]);
    XORSWAP(a.rbp, a.qword[a.rax + offsetof(X86State, r[0x5])]);
    XORSWAP(a.rsi, a.qword[a.rax + offsetof(X86State, r[0x6])]);
    XORSWAP(a.rdi, a.qword[a.rax + offsetof(X86State, r[0x7])]);
    XORSWAP(a.r8,  a.qword[a.rax + offsetof(X86State, r[0x8])]);
    XORSWAP(a.r9,  a.qword[a.rax + offsetof(X86State, r[0x9])]);
    XORSWAP(a.r10, a.qword[a.rax + offsetof(X86State, r[0xA])]);
    XORSWAP(a.r11, a.qword[a.rax + offsetof(X86State, r[0xB])]);
    XORSWAP(a.r12, a.qword[a.rax + offsetof(X86State, r[0xC])]);
    XORSWAP(a.r13, a.qword[a.rax + offsetof(X86State, r[0xD])]);
    XORSWAP(a.r14, a.qword[a.rax + offsetof(X86State, r[0xE])]);
    XORSWAP(a.r15, a.qword[a.rax + offsetof(X86State, r[0xF])]);

    // Return
    a.ret();

    // Execute
    auto callerFunc = reinterpret_cast<void(*)()>(a.getCode());
    callerFunc();
}

}  // namespace x86
}  // namespace native
}  // namespace cpu
