/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

// Visual Studio testing dependencies
#include "CppUnitTest.h"

// Target
#include "nucleus/common.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(UnitTests) {

public:
    TEST_METHOD(U128Tests)
    {
        // Operator +
        Assert::IsTrue((u128{0,0} + 1) == u128{1,0});
        Assert::IsTrue((u128{0xFFFFFFFFFFFFFFFF,0} + 1) == u128{0,1});
        Assert::IsTrue((u128{0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF} + 1) == u128{0,0});
        Assert::IsTrue((u128{0,0} + 0xFFFFFFFFFFFFFFFF) == u128{0xFFFFFFFFFFFFFFFF,0});
        Assert::IsTrue((u128{1,0} + 0xFFFFFFFFFFFFFFFF) == u128{0,1});

        // Operator -
        Assert::IsTrue((u128{1,0} - 1) == u128{0,0});
        Assert::IsTrue((u128{0,1} - 1) == u128{0xFFFFFFFFFFFFFFFF,0});
        Assert::IsTrue((u128{0,0} - 1) == u128{0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF});
        Assert::IsTrue((u128{0xFFFFFFFFFFFFFFFF,0} - 0xFFFFFFFFFFFFFFFF) == u128{0,0});
        Assert::IsTrue((u128{0,1} - 0xFFFFFFFFFFFFFFFF) == u128{1,0});

        // Operator <<
        Assert::IsTrue((u128{1,0} <<   1) == u128{2,0});
        Assert::IsTrue((u128{1,0} <<  64) == u128{0,1});
        Assert::IsTrue((u128{1,0} << 128) == u128{0,0});
        Assert::IsTrue((u128{0,1} <<   0) == u128{0,1});
        Assert::IsTrue((u128{0,1} <<   1) == u128{0,2});
        Assert::IsTrue((u128{0,1} << -64) == u128{1,0});
        Assert::IsTrue((u128{1,1} <<   0) == u128{1,1});
        Assert::IsTrue((u128{1,1} <<   1) == u128{2,2});

        // Operator >>
        Assert::IsTrue((u128{2,0} >>   1) == u128{1,0});
        Assert::IsTrue((u128{0,1} >>  64) == u128{1,0});
        Assert::IsTrue((u128{0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF} >> 128) == u128{0,0});
        Assert::IsTrue((u128{0,1} >>   0) == u128{0,1});
        Assert::IsTrue((u128{0,2} >>   1) == u128{0,1});
        Assert::IsTrue((u128{1,0} >> -64) == u128{0,1});
        Assert::IsTrue((u128{1,1} >>   0) == u128{1,1});
        Assert::IsTrue((u128{2,2} >>   1) == u128{1,1});
    }
};
