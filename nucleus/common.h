/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

/**
 * Architectures
 */
#if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
#define NUCLEUS_ARCH_X86_64
#endif
#if defined(__arm__) || defined(_M_ARM)
#define NUCLEUS_ARCH_ARM
#endif

/**
 * Platforms
 */
#if defined(__linux__)
#define NUCLEUS_PLATFORM_LINUX
#endif
#if defined(_WIN32) || defined(_WIN64)
#define NUCLEUS_PLATFORM_WINDOWS
#endif
#if defined(__APPLE__) && defined(__MACH__)
#define NUCLEUS_PLATFORM_OSX
#endif
#if defined(__ANDROID_API__)
#define NUCLEUS_PLATFORM_ANDROID
#endif
#if defined(__APPLE__) && defined(TARGET_OS_IPHONE)
#define NUCLEUS_PLATFORM_IOS
#endif

/**
 * Compilers
 */
#if defined(_MSC_VER)
#define NUCLEUS_COMPILER_MSVC
#endif
#if defined(__GNUC__)
#define NUCLEUS_COMPILER_GCC
#endif
#if defined(__clang__)
#define NUCLEUS_COMPILER_CLANG
#endif

/**
 * Events & Status
 */
enum EmulatorEvent
{
    // No event
    NUCLEUS_EVENT_NONE = 0,

    // Emulator events
    NUCLEUS_EVENT_RUN,    // Start Nucleus, Cell, CellThreads, RSX, etc.
    NUCLEUS_EVENT_PAUSE,  // Pause Nucleus, Cell, CellThreads, RSX, etc.
    NUCLEUS_EVENT_STOP,   // Stop Nucleus, Cell, CellThreads, RSX, etc.
    NUCLEUS_EVENT_CLOSE,  // Exit Nucleus
};

enum EmulatorStatus
{
    // No status
    NUCLEUS_STATUS_UNKNOWN = 0,

    // Emulator status
    NUCLEUS_STATUS_RUNNING,
    NUCLEUS_STATUS_PAUSED,
    NUCLEUS_STATUS_STOPPED,
    NUCLEUS_STATUS_CLOSED,
};

/**
 * Types
 */
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;

struct u128
{
    u64 lo, hi;

    bool operator == (const u128& r) const { return (lo == r.lo) && (hi == r.hi); }
    bool operator != (const u128& r) const { return (lo != r.lo) || (hi != r.hi); }

    u128 operator + (const u64 r) const { return u128{lo + r, hi + ((lo + r < lo) ? 1 : 0)}; }
    u128 operator - (const u64 r) const { return u128{lo - r, hi - ((lo < lo - r) ? 1 : 0)}; }
    u128 operator | (const u128& r) const { return u128{lo | r.lo, hi | r.hi}; }
    u128 operator & (const u128& r) const { return u128{lo & r.lo, hi & r.hi}; }
    u128 operator ^ (const u128& r) const { return u128{lo ^ r.lo, hi ^ r.hi}; }
    u128 operator ~ () const { return u128{~lo, ~hi}; }

    u128 operator >> (const s64 r) const {
        if (r >= 128) { return u128{0, 0}; }
        if (r == 0)   { return *this; }
        if (r < 0)    { return *this << -r; }
        u64 _hi = (r < 64) ? (hi >> r) : 0;
        u64 _lo = (r < 64) ? (0xFFFFFFFFFFFFFFFFULL << (64-r)) : (0xFFFFFFFFFFFFFFFFULL >> (r-64));
        _lo = (_lo & ((hi << (64-r)) | (hi >> r))) | ((r < 64) ? (lo >> r) : 0);
        return u128{_lo, _hi};
    }

    u128 operator << (const s64 r) const {
        if (r >= 128) { return u128{0, 0}; }
        if (r == 0)   { return *this; }
        if (r < 0)    { return *this >> -r; }
        u64 _lo = (r < 64) ? (lo << r) : 0;
        u64 _hi = (r < 64) ? (0xFFFFFFFFFFFFFFFFULL >> (64-r)) : (0xFFFFFFFFFFFFFFFFULL << (r-64));
        _hi = (_hi & ((lo << r) | (lo >> (64-r)))) | ((r < 64) ? (hi << r) : 0);
        return u128{_lo, _hi};
    }
};

/**
 * Endianness
 */
#ifdef NUCLEUS_COMPILER_MSVC
#define re16(val)  _byteswap_ushort(val)
#define re32(val)  _byteswap_ulong(val)
#define re64(val)  _byteswap_uint64(val)
#define re128(val) u128{_byteswap_uint64((val).hi), _byteswap_uint64((val).lo)}
#else
#define re16(val)  __builtin_bswap16(val)
#define re32(val)  __builtin_bswap32(val)
#define re64(val)  __builtin_bswap64(val)
#define re128(val) u128{__builtin_bswap64((val).hi), __builtin_bswap64((val).lo)}
#endif

template<typename T, int size = sizeof(T)> struct se_t;
template<typename T> struct se_t<T, 1> { static inline void func(T& dst, const T src) { (u8&)dst = (u8&)src; } };
template<typename T> struct se_t<T, 2> { static inline void func(T& dst, const T src) { (u16&)dst = re16((u16&)src); } };
template<typename T> struct se_t<T, 4> { static inline void func(T& dst, const T src) { (u32&)dst = re32((u32&)src); } };
template<typename T> struct se_t<T, 8> { static inline void func(T& dst, const T src) { (u64&)dst = re64((u64&)src); } };

template<typename T, int size=sizeof(T)>
class be_t
{
    static_assert(size == 1 || size == 2 || size == 4 || size == 8, "Bad be_t type");
    T m_data;

public:
    typedef T type;

    const T& ToBE() const
    {
        return m_data;
    }

    T ToLE() const
    {
        T res;
        se_t<T>::func(res, m_data);
        return res;
    }

    void FromBE(const T& value)
    {
        m_data = value;
    }

    void FromLE(const T& value)
    {
        se_t<T>::func(m_data, value);
    }

    operator const T() const
    {
        return ToLE();
    }

    template<typename T1>
    operator const be_t<T1>() const
    {
        be_t<T1> res;
        if (sizeof(T1) < sizeof(T)) {
            res.FromBE(ToBE() >> ((sizeof(T)-sizeof(T1)) * 8));
        }
        else if (sizeof(T1) > sizeof(T)) {
            res.FromLE(ToLE());
        }
        else {
            res.FromBE(ToBE());
        }
        return res;
    }

    be_t& operator = (const T& right) { FromLE(right); return *this; }
    be_t<T,size>& operator = (const be_t<T,size>& right) = default;

    template<typename T1> be_t& operator += (T1 right) { return *this = T(*this) + right; }
    template<typename T1> be_t& operator -= (T1 right) { return *this = T(*this) - right; }
    template<typename T1> be_t& operator *= (T1 right) { return *this = T(*this) * right; }
    template<typename T1> be_t& operator /= (T1 right) { return *this = T(*this) / right; }
    template<typename T1> be_t& operator %= (T1 right) { return *this = T(*this) % right; }
    template<typename T1> be_t& operator &= (T1 right) { return *this = T(*this) & right; }
    template<typename T1> be_t& operator |= (T1 right) { return *this = T(*this) | right; }
    template<typename T1> be_t& operator ^= (T1 right) { return *this = T(*this) ^ right; }
    template<typename T1> be_t& operator <<= (T1 right) { return *this = T(*this) << right; }
    template<typename T1> be_t& operator >>= (T1 right) { return *this = T(*this) >> right; }

    template<typename T1> be_t& operator += (const be_t<T1>& right) { return *this = ToLE() + right.ToLE(); }
    template<typename T1> be_t& operator -= (const be_t<T1>& right) { return *this = ToLE() - right.ToLE(); }
    template<typename T1> be_t& operator *= (const be_t<T1>& right) { return *this = ToLE() * right.ToLE(); }
    template<typename T1> be_t& operator /= (const be_t<T1>& right) { return *this = ToLE() / right.ToLE(); }
    template<typename T1> be_t& operator %= (const be_t<T1>& right) { return *this = ToLE() % right.ToLE(); }
    template<typename T1> be_t& operator &= (const be_t<T1>& right) { return *this = ToBE() & right.ToBE(); }
    template<typename T1> be_t& operator |= (const be_t<T1>& right) { return *this = ToBE() | right.ToBE(); }
    template<typename T1> be_t& operator ^= (const be_t<T1>& right) { return *this = ToBE() ^ right.ToBE(); }

    template<typename T1> be_t operator & (const be_t<T1>& right) const { return be_t<T>{ ToBE() & right.ToBE() }; }
    template<typename T1> be_t operator | (const be_t<T1>& right) const { return be_t<T>{ ToBE() | right.ToBE() }; }
    template<typename T1> be_t operator ^ (const be_t<T1>& right) const { return be_t<T>{ ToBE() ^ right.ToBE() }; }

    template<typename T1> bool operator == (T1 right) const { return (T1)ToLE() == right; }
    template<typename T1> bool operator != (T1 right) const { return !(*this == right); }
    template<typename T1> bool operator >  (T1 right) const { return (T1)ToLE() >  right; }
    template<typename T1> bool operator <  (T1 right) const { return (T1)ToLE() <  right; }
    template<typename T1> bool operator >= (T1 right) const { return (T1)ToLE() >= right; }
    template<typename T1> bool operator <= (T1 right) const { return (T1)ToLE() <= right; }

    template<typename T1> bool operator == (const be_t<T1>& right) const { return ToBE() == right.ToBE(); }
    template<typename T1> bool operator != (const be_t<T1>& right) const { return !(*this == right); }
    template<typename T1> bool operator >  (const be_t<T1>& right) const { return (T1)ToLE() >  right.ToLE(); }
    template<typename T1> bool operator <  (const be_t<T1>& right) const { return (T1)ToLE() <  right.ToLE(); }
    template<typename T1> bool operator >= (const be_t<T1>& right) const { return (T1)ToLE() >= right.ToLE(); }
    template<typename T1> bool operator <= (const be_t<T1>& right) const { return (T1)ToLE() <= right.ToLE(); }

    be_t operator++ (int) { be_t res = *this; *this += 1; return res; }
    be_t operator-- (int) { be_t res = *this; *this -= 1; return res; }
    be_t& operator++ () { *this += 1; return *this; }
    be_t& operator-- () { *this -= 1; return *this; }
};

/**
 * Compiler warnings/errors
 */
#if defined(NUCLEUS_PLATFORM_WINDOWS)
#define NOMINMAX  // Conflicts with std::min, std::max in <algorithm> and min, max definitions in <Windows.h>
#endif

#if defined(NUCLEUS_COMPILER_MSVC)
// Reason:       Nucleus is not responsible for useful C11 features not being in C++11.
// Description:  Warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)
#endif
