/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <chrono>
#include <functional>

namespace ui {
namespace transition {

// Transition utilities
using Duration = std::chrono::high_resolution_clock::duration;

/**
 * Apply the given paremetrized transition effect at the provided timestamp
 * @param[in]  func   Transition function (see definition below)
 * @param[in]  time   Current time to sample the transition
 * @param[in]  start  Starting point of the effect
 * @param[in]  end    Ending point of the effect
 */
double apply(std::function<double(double)> f, Duration t, Duration start, Duration end);

/**
 * Transitions
 * ===========
 * Collection of functions f : [0,1] -> [0,1] that satisfy: f(0) = 0 and f(1) = 1.
 */

// Ease In: f(x) = x^2
double easeIn(double x);

// Ease Out: f(x) = -x(x-2)
double easeOut(double x);

}  // namespace transition
}  // namespace ui
