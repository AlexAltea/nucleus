/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/macro.h"

// Common
#define TEST_METHOD_CATEGORY(method, category) \
    BEGIN_TEST_METHOD_ATTRIBUTE(method) \
        TEST_METHOD_ATTRIBUTE(L"Category", category) \
    END_TEST_METHOD_ATTRIBUTE() \
    TEST_METHOD(method)
