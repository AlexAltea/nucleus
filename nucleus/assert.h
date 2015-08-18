/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#define _NUCLEUS_ARGUMENT_EXTRACT(a1, a2, a3, a4, a5, a6, a7, a8, N, ...) \
	N
#define _NUCLEUS_ARGUMENT_COUNT(...) \
	_NUCLEUS_ARGUMENT_EXTRACT(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define _NUCLEUS_MACRO_SELECT(function, argc) \
	function##argc
#define _NUCLEUS_MACRO_DISPATCH(function, ...) \
	_NUCLEUS_MACRO_SELECT(function, _NUCLEUS_ARGUMENT_COUNT(__VA_ARGS__))

// Trigger an exception if expression is false
#define assert_true(...) \
	_NUCLEUS_MACRO_DISPATCH(assert_true, __VA_ARGS__)(__VA_ARGS__)
#define assert_true1(expr) assert(expr)
#define assert_true2(expr, message) assert(expr && message)

// Trigger an exception if expression is true
#define assert_false(...) \
	_NUCLEUS_MACRO_DISPATCH(assert_false, __VA_ARGS__)(__VA_ARGS__)
#define assert_false1(expr) assert(expr)
#define assert_false2(expr, message) assert(!(expr) && message)

// Trigger an exception if expression is zero
#define assert_zero(...) \
	_NUCLEUS_MACRO_DISPATCH(assert_zero, __VA_ARGS__)(__VA_ARGS__)
#define assert_zero1(expr) assert(expr)
#define assert_zero2(expr, message) assert((expr) == 0  && message)

// Trigger an exception if expression is non-zero
#define assert_nonzero(...) \
	_NUCLEUS_MACRO_DISPATCH(assert_true, __VA_ARGS__)(__VA_ARGS__)
#define assert_nonzero1(expr) assert_nonzero(expr)
#define assert_nonzero2(expr, message) assert((expr) != 0  && message)

// Trigger an exception if expression is a nullptr
#define assert_null(...) \
	_NUCLEUS_MACRO_DISPATCH(assert_null, __VA_ARGS__)(__VA_ARGS__)
#define assert_null1(expr) assert(expr)
#define assert_null2(expr, message) assert((expr) == nullptr && message)

// Trigger an exception if expression is not a nullptr
#define assert_nonnull(...) \
	_NUCLEUS_MACRO_DISPATCH(assert_nonnull, __VA_ARGS__)(__VA_ARGS__)
#define assert_nonnull1(expr) assert(expr)
#define assert_nonnull2(expr, message) assert((expr) != nullptr && message)

// Trigger an exception
#define assert_always(...) assert(0)
