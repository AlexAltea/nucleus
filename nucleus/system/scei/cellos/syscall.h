/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"

namespace sys {

// Forward declaration
class LV2;

// Syscall arguments
#define ARG_GPR(T,n) (T)(std::is_pointer<T>::value ? (U64)memoryBase + state.r[3+n] : state.r[3+n])

// Base class for HLE syscalls
class Syscall
{
public:
    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase)=0;
    virtual ~Syscall(){};
};

// Derived class
template <typename TR, typename... TA>
class SyscallBinder;

template <typename TR>
class SyscallBinder<TR> : public Syscall
{
    TR(*m_func)(LV2&);

public:
    SyscallBinder(TR(*func)(LV2&)) : m_func(func) {}

    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase) {
        state.r[3] = m_func(kernel);
    }
};

template <typename TR, typename T1>
class SyscallBinder<TR, T1> : public Syscall {
    TR(*m_func)(LV2&, T1);

public:
    SyscallBinder(TR(*func)(LV2&, T1)) : m_func(func) {}

    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase) {
        state.r[3] = m_func(kernel, ARG_GPR(T1,0));
    }
};

template <typename TR, typename T1, typename T2>
class SyscallBinder<TR, T1, T2> : public Syscall {
    TR(*m_func)(LV2&, T1, T2);

public:
    SyscallBinder(TR(*func)(LV2&, T1, T2)) : m_func(func) {}

    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase) {
        state.r[3] = m_func(kernel, ARG_GPR(T1,0), ARG_GPR(T2,1));
    }
};

template <typename TR, typename T1, typename T2, typename T3>
class SyscallBinder<TR, T1, T2, T3> : public Syscall {
    TR(*m_func)(LV2&, T1, T2, T3);

public:
    SyscallBinder(TR(*func)(LV2&, T1, T2, T3)) : m_func(func) {}

    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase) {
        state.r[3] = m_func(kernel, ARG_GPR(T1,0), ARG_GPR(T2,1), ARG_GPR(T3,2));
    }
};

template <typename TR, typename T1, typename T2, typename T3, typename T4>
class SyscallBinder<TR, T1, T2, T3, T4> : public Syscall {
    TR(*m_func)(LV2&, T1, T2, T3, T4);

public:
    SyscallBinder(TR(*func)(LV2&, T1, T2, T3, T4)) : m_func(func) {}

    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase) {
        state.r[3] = m_func(kernel, ARG_GPR(T1,0), ARG_GPR(T2,1), ARG_GPR(T3,2), ARG_GPR(T4,3));
    }
};

template <typename TR, typename T1, typename T2, typename T3, typename T4, typename T5>
class SyscallBinder<TR, T1, T2, T3, T4, T5> : public Syscall {
    TR(*m_func)(LV2&, T1, T2, T3, T4, T5);

public:
    SyscallBinder(TR(*func)(LV2&, T1, T2, T3, T4, T5)) : m_func(func) {}

    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase) {
        state.r[3] = m_func(kernel, ARG_GPR(T1,0), ARG_GPR(T2,1), ARG_GPR(T3,2), ARG_GPR(T4,3), ARG_GPR(T5,4));
    }
};

template <typename TR, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
class SyscallBinder<TR, T1, T2, T3, T4, T5, T6> : public Syscall {
    TR(*m_func)(LV2&, T1, T2, T3, T4, T5, T6);

public:
    SyscallBinder(TR(*func)(LV2&, T1, T2, T3, T4, T5, T6)) : m_func(func) {}

    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase) {
        state.r[3] = m_func(kernel, ARG_GPR(T1,0), ARG_GPR(T2,1), ARG_GPR(T3,2), ARG_GPR(T4,3), ARG_GPR(T5,4), ARG_GPR(T6,5));
    }
};

template <typename TR, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
class SyscallBinder<TR, T1, T2, T3, T4, T5, T6, T7> : public Syscall {
    TR(*m_func)(LV2&, T1, T2, T3, T4, T5, T6, T7);

public:
    SyscallBinder(TR(*func)(LV2&, T1, T2, T3, T4, T5, T6, T7)) : m_func(func) {}

    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase) {
        state.r[3] = m_func(kernel, ARG_GPR(T1,0), ARG_GPR(T2,1), ARG_GPR(T3,2), ARG_GPR(T4,3), ARG_GPR(T5,4), ARG_GPR(T6,5), ARG_GPR(T7,6));
    }
};

template <typename TR, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
class SyscallBinder<TR, T1, T2, T3, T4, T5, T6, T7, T8> : public Syscall {
    TR(*m_func)(LV2&, T1, T2, T3, T4, T5, T6, T7, T8);

public:
    SyscallBinder(TR(*func)(LV2&, T1, T2, T3, T4, T5, T6, T7, T8)) : m_func(func) {}

    virtual void call(cpu::frontend::ppu::PPUState& state, LV2& kernel, void* memoryBase) {
        state.r[3] = m_func(kernel, ARG_GPR(T1,0), ARG_GPR(T2,1), ARG_GPR(T3,2), ARG_GPR(T4,3), ARG_GPR(T5,4), ARG_GPR(T6,5), ARG_GPR(T7,6), ARG_GPR(T8,7));
    }
};

/**
 * Wraps a syscall.
 * @tparam  TR  Type of return value
 * @tparam  TA  Type of arguments
 */
template <typename TR, typename... TA>
Syscall* wrap(TR(*func)(LV2&, TA...)) {
    return new SyscallBinder<TR, TA...>(func);
}

}  // namespace sys
