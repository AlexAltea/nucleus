/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

// Visual Studio testing dependencies
#include "CppUnitTest.h"

// Target
#include "nucleus/common.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CommonTests) {

public:
    TEST_METHOD(Common_SwappedEndianTests) {
    }

    TEST_METHOD(Common_U128Tests) {
        // Operator +
        Assert::IsTrue((U128{0,0} + 1) == U128{1,0});
        Assert::IsTrue((U128{0xFFFFFFFFFFFFFFFF,0} + 1) == U128{0,1});
        Assert::IsTrue((U128{0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF} + 1) == U128{0,0});
        Assert::IsTrue((U128{0,0} + 0xFFFFFFFFFFFFFFFF) == U128{0xFFFFFFFFFFFFFFFF,0});
        Assert::IsTrue((U128{1,0} + 0xFFFFFFFFFFFFFFFF) == U128{0,1});

        // Operator -
        Assert::IsTrue((U128{1,0} - 1) == U128{0,0});
        Assert::IsTrue((U128{0,1} - 1) == U128{0xFFFFFFFFFFFFFFFF,0});
        Assert::IsTrue((U128{0,0} - 1) == U128{0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF});
        Assert::IsTrue((U128{0xFFFFFFFFFFFFFFFF,0} - 0xFFFFFFFFFFFFFFFF) == U128{0,0});
        Assert::IsTrue((U128{0,1} - 0xFFFFFFFFFFFFFFFF) == U128{1,0});

        // Operator <<
        Assert::IsTrue((U128{1,0} <<   1) == U128{2,0});
        Assert::IsTrue((U128{1,0} <<  64) == U128{0,1});
        Assert::IsTrue((U128{1,0} << 128) == U128{0,0});
        Assert::IsTrue((U128{0,1} <<   0) == U128{0,1});
        Assert::IsTrue((U128{0,1} <<   1) == U128{0,2});
        Assert::IsTrue((U128{0,1} << -64) == U128{1,0});
        Assert::IsTrue((U128{1,1} <<   0) == U128{1,1});
        Assert::IsTrue((U128{1,1} <<   1) == U128{2,2});

        // Operator >>
        Assert::IsTrue((U128{2,0} >>   1) == U128{1,0});
        Assert::IsTrue((U128{0,1} >>  64) == U128{1,0});
        Assert::IsTrue((U128{0xFFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFFF} >> 128) == U128{0,0});
        Assert::IsTrue((U128{0,1} >>   0) == U128{0,1});
        Assert::IsTrue((U128{0,2} >>   1) == U128{0,1});
        Assert::IsTrue((U128{1,0} >> -64) == U128{0,1});
        Assert::IsTrue((U128{1,1} >>   0) == U128{1,1});
        Assert::IsTrue((U128{2,2} >>   1) == U128{1,1});
    }
};
