/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "transitions.h"

namespace ui {
namespace transition {

double easeIn(double t) {
    return 0.0;
}

double easeOut(double t) {
    return -1.0 * t * (t - 2.0);
}

}  // namespace transition
}  // namespace ui
