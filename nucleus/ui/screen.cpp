/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen.h"

Screen::Screen()
{
    time_creation = Clock::now();
}

void Screen::prologue()
{
    time_current = Clock::now();
    dtime = std::chrono::duration_cast<std::chrono::milliseconds>(time_current - time_creation).count();
}

void Screen::epilogue()
{
    frame += 1;
}
