/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include <chrono>
#include <functional>

namespace ui {

/**
 * Transition
 * ==========
 * Wraps a value providing smooth updates based on the system time
 * given a certain transition function.
 */
template <typename T>
class Transition {
    using Clock = std::chrono::high_resolution_clock;

    // Transition properties
    std::function<double(double)> func;
    Clock::duration duration;
    Clock::time_point start;
    Clock::time_point end;

public:
    T target;
    T source;

    /**
     * Constructor
     * @param[in]  func      Transition function (see definition below)
     * @param[in]  duration  Time the transition requires to complete
     */
    Transition(std::function<double(double)> func, Clock::duration duration) :
        func(func), duration(duration) {}

    /**
     * Sets a new target, updating the starting time point
     * @param[in]  value  Value to set the transition value to
     */
    Transition<T>& operator=(const T& value) {
        source = target;
        target = value;
        start = Clock::now();
        end = start + duration;
        return *this;
    }

    /**
     * Gets the current value based on the current time point
     * @return  Interpolated value based on current time since last update
     */
    operator const T() const {
        auto now = Clock::now();
        if (now <= end) {
            double elapsedTicks = (now - start).count();
            double durationTicks = duration.count();
            auto a = elapsedTicks / durationTicks;
            return (1 - a) * source + a * target;
        }
        return target;
    }

    // Operations
    template <typename U> Transition<T>& operator += (U right) { return *this = T(*this)  + right; }
    template <typename U> Transition<T>& operator -= (U right) { return *this = T(*this)  - right; }
    template <typename U> Transition<T>& operator *= (U right) { return *this = T(*this)  * right; }
    template <typename U> Transition<T>& operator /= (U right) { return *this = T(*this)  / right; }
    template <typename U> Transition<T>& operator %= (U right) { return *this = T(*this)  % right; }
    template <typename U> Transition<T>& operator &= (U right) { return *this = T(*this)  & right; }
    template <typename U> Transition<T>& operator |= (U right) { return *this = T(*this)  | right; }
    template <typename U> Transition<T>& operator ^= (U right) { return *this = T(*this)  ^ right; }
    template <typename U> Transition<T>& operator <<=(U right) { return *this = T(*this) << right; }
    template <typename U> Transition<T>& operator >>=(U right) { return *this = T(*this) >> right; }
};

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
double apply(std::function<double(double)> func, Duration time, Duration start, Duration end);

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
