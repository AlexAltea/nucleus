/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Size literals
constexpr unsigned long long int operator"" _B(unsigned long long int value) {
    return (value * 1);
}
constexpr unsigned long long int operator"" _KB(unsigned long long int value) {
    return (value * 1024);
}
constexpr unsigned long long int operator"" _MB(unsigned long long int value) {
    return (value * 1024 * 1024);
}
constexpr unsigned long long int operator"" _GB(unsigned long long int value) {
    return (value * 1024 * 1024 * 1024);
}
constexpr unsigned long long int operator"" _TB(unsigned long long int value) {
    return (value * 1024 * 1024 * 1024 * 1024);
}
constexpr unsigned long long int operator"" _PB(unsigned long long int value) {
    return (value * 1024 * 1024 * 1024 * 1024 * 1024);
}
constexpr unsigned long long int operator"" _EB(unsigned long long int value) {
    return (value * 1024 * 1024 * 1024 * 1024 * 1024 * 1024);
}
constexpr unsigned long long int operator"" _ZB(unsigned long long int value) {
    return (value * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024);
}
constexpr unsigned long long int operator"" _YB(unsigned long long int value) {
    return (value * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024);
}
