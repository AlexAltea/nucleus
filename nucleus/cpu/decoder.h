/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

/**
 * Instruction fields:
 * Represent a field of the 32-bit PPU or SPU instructions
 */

// Base instruction field
class CodeFieldBase
{
public:
    virtual u32 decode(u32 data) const=0;
};

// Unsigned 32-bit instruction field
template<int from, int to=from>
class UCodeField : public CodeFieldBase
{
public:
    static const u32 size = to - from + 1;
    static const u32 shift = 31 - to;
    static const u32 mask = ((1 << (to - from + 1)) - 1) << shift;
    
    virtual u32 decode(u32 data) const
    {
        return (data & mask) >> shift;
    }
};

// Signed 32-bit instruction field
template<int from, int to=from>
class SCodeField : public UCodeField<from, to>
{
public:
    virtual u32 decode(u32 data) const
    {
        const u32 value = (data & UCodeField<from, to>::mask) >> UCodeField<from, to>::shift;
        if (value & (1 << (size - 1))) {
		    return value - (1 << size);
	    }
	    return value;
    }
};

// Unsigned 32-bit instruction split-field
template<int from1, int to1, int from2, int to2>
class UCodeFieldSplit : public UCodeField<from1, to1>
{
public:
    static const u32 shift2 = 31 - to2;
    static const u32 mask2 = ((1 << ((to2 - from2) + 1)) - 1) << shift2;
    static const u32 offset = to1 - from1 + 1;
    
    virtual u32 decode(u32 data) const
    {
        return ((data & UCodeField<from1, to1>::mask) >> UCodeField<from1, to1>::shift) | (((data & mask2) >> shift2) << offset);
    }
};

/**
 * Instruction caller:
 * Base of instruction lists: Table of more instruction callers
 * Base of instruction binders: Decodes the fields and calls the corresponding instruction
 */
template<typename TO>
class InstrCaller
{
public:
	virtual ~InstrCaller<TO>() = default;

    // Jump to the next instruction or instruction list
	virtual void call(TO* operation, u32 code) const = 0;
};

/**
 * Instruction lists:
 */
template<int count, typename TO>
class InstrList : public InstrCaller<TO>
{
    const CodeFieldBase& m_field;
    InstrCaller<TO>* m_callers[count];

public:
    InstrList(const CodeFieldBase& field, InstrCaller<TO>* errorFunc) : m_field(field)
    {
        for (auto& caller : m_callers) {
            caller = errorFunc;
        }
    }

    virtual void call(TO* operation, u32 instruction) const
    {
        const u32 opcode = m_field.decode(instruction);
        m_callers[opcode]->call(operation, instruction);
    }
};

/**
 * Instruction binders:
 */
template<typename TO, typename T1>
InstrCaller<TO>* instr_bind(void (TO::*func)(T1), const CodeFieldBase& f1)
{
	return new InstrBinder_1<TO, T1>(func, f1);
}

template<typename TO>
class InstrBinder_0 : public InstrCaller<TO>
{
    typedef void (TO::*func_t)();
    func_t m_func;

    InstrBinder_0(func_t func)
        : m_func(func)
    {
    }

    void call(TO* operation, u32 instruction) const
    {
        (operation->m_func)();
    }
};

template<typename TO, typename T1>
class InstrBinder_1 : public InstrCaller<TO>
{
    typedef void (TO::*func_t)(T1);
    func_t m_func;
    const CodeFieldBase& m_field1;

public:
    InstrBinder_1(func_t func, const CodeFieldBase& f1)
        : m_func(func), m_field1(f1)
    {
    }

    void call(TO* operation, u32 instruction) const
    {
        (operation->*m_func)(
            (T1)m_field1.decode(instruction)
        );
    }
};

template<typename TO, typename T1, typename T2>
class InstrBinder_2 : public InstrCaller<TO>
{
    typedef void (TO::*func_t)(T1, T2);
    func_t m_func;
    const CodeFieldBase& m_field1;
	const CodeFieldBase& m_field2;

    InstrBinder_2(func_t func, T1 f1, T2 f2)
        : m_func(func), m_field1(f1), m_field2(f2)
    {
    }

    void call(TO* operation, u32 instruction) const
    {
        (operation->m_func)(
            (T1)m_field1.decode(instruction),
            (T2)m_field2.decode(instruction)
        );
    }
};

template<typename TO, typename T1, typename T2, typename T3>
class InstrBinder_3 : public InstrCaller<TO>
{
    typedef void (TO::*func_t)(T1, T2, T3);
    func_t m_func;
    const CodeFieldBase& m_field1;
	const CodeFieldBase& m_field2;
	const CodeFieldBase& m_field3;

    InstrBinder_3(func_t func, T1 f1, T2 f2, T3 f3)
        : m_func(func), m_field1(f1), m_field2(f2), m_field3(f3)
    {
    }

    void call(TO* operation, u32 instruction) const
    {
        (operation->m_func)(
            (T1)m_field1.decode(instruction),
            (T2)m_field2.decode(instruction),
            (T3)m_field3.decode(instruction)
        );
    }
};

template<typename TO, typename T1, typename T2, typename T3, typename T4>
class InstrBinder_4 : public InstrCaller<TO>
{
    typedef void (TO::*func_t)(T1, T2, T3, T4);
    func_t m_func;
    const CodeFieldBase& m_field1;
	const CodeFieldBase& m_field2;
	const CodeFieldBase& m_field3;
	const CodeFieldBase& m_field4;

    InstrBinder_4(func_t func, T1 f1, T2 f2, T3 f3, T4 f4)
        : m_func(func), m_field1(f1), m_field2(f2), m_field3(f3), m_field4(f4)
    {
    }

    void call(TO* operation, u32 instruction) const
    {
        (operation->m_func)(
            (T1)m_field1.decode(instruction),
            (T2)m_field2.decode(instruction),
            (T3)m_field3.decode(instruction),
            (T4)m_field4.decode(instruction)
        );
    }
};

template<typename TO, typename T1, typename T2, typename T3, typename T4, typename T5>
class InstrBinder_5 : public InstrCaller<TO>
{
    typedef void (TO::*func_t)(T1, T2, T3, T4, T5);
    func_t m_func;
    const CodeFieldBase& m_field1;
	const CodeFieldBase& m_field2;
	const CodeFieldBase& m_field3;
	const CodeFieldBase& m_field4;
	const CodeFieldBase& m_field5;

    InstrBinder_5(func_t func, T1 f1, T2 f2, T3 f3, T4 f4, T5 f5)
        : m_func(func), m_field1(f1), m_field2(f2), m_field3(f3), m_field4(f4), m_field5(f5)
    {
    }

    void call(TO* operation, u32 instruction) const
    {
        (operation->m_func)(
            (T1)m_field1.decode(instruction),
            (T2)m_field2.decode(instruction),
            (T3)m_field3.decode(instruction),
            (T4)m_field4.decode(instruction),
            (T5)m_field5.decode(instruction)
        );
    }
};

template<typename TO, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
class InstrBinder_6 : public InstrCaller<TO>
{
    typedef void (TO::*func_t)(T1, T2, T3, T4, T5, T6);
    func_t m_func;
    const CodeFieldBase& m_field1;
	const CodeFieldBase& m_field2;
	const CodeFieldBase& m_field3;
	const CodeFieldBase& m_field4;
	const CodeFieldBase& m_field5;
	const CodeFieldBase& m_field6;

    InstrBinder_6(func_t func, T1 f1, T2 f2, T3 f3, T4 f4, T5 f5, T6 f6)
        : m_func(func), m_field1(f1), m_field2(f2), m_field3(f3), m_field4(f4), m_field5(f5), m_field6(f6)
    {
    }

    void call(TO* operation, u32 instruction) const
    {
        (operation->m_func)(
            (T1)m_field1.decode(instruction),
            (T2)m_field2.decode(instruction),
            (T3)m_field3.decode(instruction),
            (T4)m_field4.decode(instruction),
            (T5)m_field5.decode(instruction),
            (T6)m_field6.decode(instruction)
        );
    }
};
