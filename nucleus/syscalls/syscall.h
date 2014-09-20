/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"
#include "nucleus/cpu/ppu/ppu_thread.h"

// Syscall arguments
#define ARG_GPR(n) thread.gpr[3+n]

// Base class for HLE syscalls
class Syscall
{
public:
	virtual void call(PPUThread& thread)=0;
	virtual ~Syscall(){};
};

// Derived class
template<typename TR, typename... TA>
class SyscallBinder;

template<typename TR>
class SyscallBinder<TR> : public Syscall
{
	TR(*m_func)();

public:
	SyscallBinder(TR(*func)()) : m_func(func) {}

	virtual void call(PPUThread& thread)
    {
		thread.gpr[3] = m_func();
	}
};

template<typename TR, typename T1>
class SyscallBinder<TR, T1> : public Syscall
{
	TR(*m_func)(T1);

public:
	SyscallBinder(TR(*func)(T1)) : m_func(func) {}

	virtual void call(PPUThread& thread)
    {
		thread.gpr[3] = m_func(ARG_GPR(0));
	}
};

template<typename TR, typename T1, typename T2>
class SyscallBinder<TR, T1, T2> : public Syscall
{
	TR(*m_func)(T1, T2);

public:
	SyscallBinder(TR(*func)(T1, T2)) : m_func(func) {}

	virtual void call(PPUThread& thread)
    {
		thread.gpr[3] = m_func(ARG_GPR(0), ARG_GPR(1));
	}
};

template<typename TR, typename... TA>
class SyscallBinder;

template<typename TR, typename T1, typename T2, typename T3>
class SyscallBinder<TR, T1, T2, T3> : public Syscall
{
	TR(*m_func)(T1, T2, T3);

public:
	SyscallBinder(TR(*func)(T1, T2, T3)) : m_func(func) {}

	virtual void call(PPUThread& thread)
    {
		thread.gpr[3] = m_func(ARG_GPR(0), ARG_GPR(1), ARG_GPR(2));
	}
};

template<typename TR, typename T1, typename T2, typename T3, typename T4>
class SyscallBinder<TR, T1, T2, T3, T4> : public Syscall
{
	TR(*m_func)(T1, T2, T3, T4);

public:
	SyscallBinder(TR(*func)(T1, T2, T3, T4)) : m_func(func) {}

	virtual void call(PPUThread& thread)
    {
		thread.gpr[3] = m_func(ARG_GPR(0), ARG_GPR(1), ARG_GPR(2), ARG_GPR(3));
	}
};

template<typename TR, typename... TA>
Syscall* wrap(TR(*func)(TA...))
{
	return new SyscallBinder<TR, TA...>(func);
}
