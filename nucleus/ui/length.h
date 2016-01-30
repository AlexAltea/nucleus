/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

namespace ui {

class Length {
    using Amount = double;

public:
    Amount value;
    enum Type {
        TYPE_UNDEFINED = 0,  
        TYPE_CM,  // Centimeter
        TYPE_IN,  // Inch
        TYPE_MM,  // Millimeter
        TYPE_PC,  // Pica
        TYPE_PCT, // Percentage
        TYPE_PT,  // Point
        TYPE_PX,  // Pixel
    } type;

    // Scalar operations
    Length operator+ (Amount rhs) { return Length{value + rhs, type}; }
    Length operator- (Amount rhs) { return Length{value - rhs, type}; }
    Length operator* (Amount rhs) { return Length{value * rhs, type}; }
    Length operator/ (Amount rhs) { return Length{value / rhs, type}; }

    // Length operations // TODO: Check unit used
    Length operator+ (const Length& rhs) { return Length{value + rhs.value, type}; }
    Length operator- (const Length& rhs) { return Length{value - rhs.value, type}; }
    Length operator* (const Length& rhs) { return Length{value * rhs.value, type}; }
    Length operator/ (const Length& rhs) { return Length{value / rhs.value, type}; }
};

// Literals
constexpr Length operator"" _cm(long double value) {
    return Length{ double(value), Length::TYPE_CM };
}
constexpr Length operator"" _in(long double value) {
    return Length{ double(value), Length::TYPE_IN };
}
constexpr Length operator"" _mm(long double value) {
    return Length{ double(value), Length::TYPE_MM };
}
constexpr Length operator"" _pc(long double value) {
    return Length{ double(value), Length::TYPE_PC };
}
constexpr Length operator"" _pct(long double value) {
    return Length{ double(value), Length::TYPE_PCT };
}
constexpr Length operator"" _pt(long double value) {
    return Length{ double(value), Length::TYPE_PT };
}
constexpr Length operator"" _px(long double value) {
    return Length{ double(value), Length::TYPE_PX };
}

}  // namespace ui
