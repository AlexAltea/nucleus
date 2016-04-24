/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/common.h"

#include <chrono>

namespace ui {

/**
 * Event
 * =====
 * Event base class.
 */
class Event {
    using Clock = std::chrono::high_resolution_clock;

public:
    Clock::time_point timestamp;

    enum Type {
        TYPE_MOUSE_CLICK,
        TYPE_MOUSE_DBLCLICK,
        TYPE_MOUSE_MOVE,
        TYPE_MOUSE_WHEEL,
        TYPE_KEY_DOWN,
        TYPE_KEY_UP,
    } type;

    Event(Type type) : timestamp(Clock::now()), type(type) {}
};

/**
 * Keyboard Event
 * ==============
 * Represents a keyboard event.
 */
class KeyEvent : public Event {
public:
    int keycode;

    KeyEvent(Type type) : Event(type) {}
};

/**
 * Mouse Event
 * ===========
 * Represents a mouse event.
 */
class MouseEvent : public Event {
public:
    enum Button {
        BUTTON_LEFT,
        BUTTON_RIGHT,
        BUTTON_MIDDLE,
        BUTTON_X1,
        BUTTON_X2,
    } button;

    int x;
    int y;
    int dx;
    int dy;

    MouseEvent(Type type) : Event(type) {}
};

class MouseWheelEvent : public Event {
public:
    int x;
    int y;
    int delta;

    MouseWheelEvent() : Event(TYPE_MOUSE_WHEEL) {}
};

}  // namespace ui
