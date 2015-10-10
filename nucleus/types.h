/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

using S8 = char;
using S16 = short;
using S32 = int;
using S64 = long long;

using U8 = unsigned char;
using U16 = unsigned short;
using U32 = unsigned int;
using U64 = unsigned long long;

using F32 = float;
using F64 = double;


struct U128 {
    U64 lo, hi;

    bool operator == (const U128& r) const { return (lo == r.lo) && (hi == r.hi); }
    bool operator != (const U128& r) const { return (lo != r.lo) || (hi != r.hi); }

    U128 operator + (const U64 r) const { return U128{lo + r, hi + ((lo + r < lo) ? 1 : 0)}; }
    U128 operator - (const U64 r) const { return U128{lo - r, hi - ((lo < lo - r) ? 1 : 0)}; }
    U128 operator | (const U128& r) const { return U128{lo | r.lo, hi | r.hi}; }
    U128 operator & (const U128& r) const { return U128{lo & r.lo, hi & r.hi}; }
    U128 operator ^ (const U128& r) const { return U128{lo ^ r.lo, hi ^ r.hi}; }
    U128 operator ~ () const { return U128{~lo, ~hi}; }

    U128 operator >> (const S64 r) const {
        if (r >= 128) { return U128{0, 0}; }
        if (r == 0)   { return *this; }
        if (r < 0)    { return *this << -r; }
        U64 _hi = (r < 64) ? (hi >> r) : 0;
        U64 _lo = (r < 64) ? (0xFFFFFFFFFFFFFFFFULL << (64-r)) : (0xFFFFFFFFFFFFFFFFULL >> (r-64));
        _lo = (_lo & ((hi << (64-r)) | (hi >> r))) | ((r < 64) ? (lo >> r) : 0);
        return U128{_lo, _hi};
    }

    U128 operator << (const S64 r) const {
        if (r >= 128) { return U128{0, 0}; }
        if (r == 0)   { return *this; }
        if (r < 0)    { return *this >> -r; }
        U64 _lo = (r < 64) ? (lo << r) : 0;
        U64 _hi = (r < 64) ? (0xFFFFFFFFFFFFFFFFULL >> (64-r)) : (0xFFFFFFFFFFFFFFFFULL << (r-64));
        _hi = (_hi & ((lo << r) | (lo >> (64-r)))) | ((r < 64) ? (hi << r) : 0);
        return U128{_lo, _hi};
    }
};

// Vector types

/**
 * 128-bit vector
 * Allows component-wise signed and unsigned 8-bit, 16-bit and 32-bit
 * integer operations and component-wise 32-bit floating point operations.
 */
union V128 {
    F64 f64[2];
    F32 f32[4];
    U64 u64[2];
    S64 s64[2];
    U32 u32[4];
    S32 s32[4];
    U16 u16[8];
    S16 s16[8];
    U8 u8[16];
    S8 s8[16];
};

/**
* 256-bit vector
* Allows component-wise signed and unsigned 8-bit, 16-bit, 32-bit and 64-bit
* integer operations and component-wise 32-bit and 64-bit floating point operations.
*/
union V256 {
    F64 f64[4];
    F32 f32[8];
    U64 u64[4];
    S64 s64[4];
    U32 u32[8];
    S32 s32[8];
    U16 u16[16];
    S16 s16[16];
    U8 u8[32];
    S8 s8[32];
};
