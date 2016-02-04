/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "transitions.h"

namespace ui {
namespace transition {

double apply(std::function<double(double)> func, Duration time, Duration start, Duration end) {
    double elapsed = (time - start).count();
    double duration = (end - start).count();
    return func(elapsed / duration);
}

double easeIn(double x) {
    return x * x;
}

double easeOut(double x) {
    return -x * (x - 2.0);
}

}  // namespace transition
}  // namespace ui
