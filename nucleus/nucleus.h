/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

void nucleusConfigure(int argc, char **argv);

int nucleusInitialize(int argc, char **argv);

// Events
void nucleusOnResize(unsigned int w, unsigned int h, unsigned int dpi, unsigned int hz);
