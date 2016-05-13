Conventions
===========

This document describes the coding-style rules that apply on this repository.

## Syntax

* No tabs or trailing spaces.
* Surround arithmetic and logical operations (`+`, `-`, `*`, `/`, `&`, `|`, `^`) with spaces.
* Always place a space after a comma (`,`).
* Indentation size of 4 spaces for *.cpp, *.h files.
* Indentation size of 2 spaces for *.xml, *.json files.


## Coding

* High-level emulated primitive types should be defined through the following fixed-sized types:
    * `S08`, `S16`, `S32`, `S64` for signed integer types.
    * `U08`, `U16`, `U32`, `U64` for unsigned integer types.
    * `F32`, `F64` for floating-point types.
* High-level emulated primitive types bigger than 8-bit byte should be wrapped with `LE` or `BE` depending on the emulated architecture:
    * `LE<T>` is a little-endian representation of the type `T`.
    * `BE<T>` is a big-endian representation of the type `T`.
* High-level emulated constants should be defined via ```enum : type { ... }```, where _type_ represents any of the fixed-sized integer types listed above.
* Always use `using` instead of `typedef`.
* Always use a range-based `for` if possible.


## Naming

* All type names must start with an uppercase character and avoid underscores. High-level emulated types should be declared exactly as they are in the corresponding system, even if it breaks this rule.
* All variable names must start with a lowercase character. High-level emulated variables should be declared exactly as they are in the corresponding system, even if it breaks this rule.


## Layout

### C++ files

The following layout appplies both to source files (_.cpp_) and header files (_.h_).

__License__. Mandatory comment:
```cpp
/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */
```

__Include guard__. Only for headers. Do this with:
```cpp
#pragma once
```

__Includes__. Only if necessary. The following order is preferred:

1. Header corresponding to this source file.
2. Header files of Nucleus.
3. Header files of external dependencies.
4. Header files of the Standard Template Library.

__Namespace (begin)__. They do not affect indentation in any way. E.g.:
```cpp
namespace name {
```

__Defines__.

__Constants__.

__Types__.

__Functions__.

__Namespace (end)__. Place in a comment what namespace are they closing. E.g.:
```cpp
}  // namespace name
```
