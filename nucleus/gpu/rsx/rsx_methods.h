/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

namespace gpu {
namespace rsx {

enum MethodType {
#define METHOD(name, offset) name = offset,
#include "rsx_methods.inl"
#undef METHOD
};

union Method {
    U32 value;

    // Fields
#define FIELD(from, to, type) struct{ U32:(32-to-1); type:(to-from+1); U32:from; }
    FIELD( 1,  1, U32 flag_ni);        // Non-Incrementing method
    FIELD( 2,  2, U32 flag_jump);      // Jump command
    FIELD(14, 14, U32 flag_ret);       // Return command
    FIELD(30, 30, U32 flag_call);      // Call command
    FIELD( 3, 13, U32 method_count);   // Method arguments count
    FIELD(16, 29, U32 method_offset);  // Method offset (>> 2)
    FIELD( 3, 31, U32 jump_offset);    // Jump IO offset
    FIELD( 0, 29, U32 call_offset);    // Call IO offset (>> 2)
#undef FIELD

    // Debugging
    static const char* getName(U32 offset) {
        switch (offset) {
#define METHOD(name, offset) case name: return #name;
#include "rsx_methods.inl"
#undef METHOD
        default:
            return "UNKNOWN";
        }
    }
};

}  // namespace rsx
}  // namespace gpu
