/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

// Base class for PPU and SPU interpreters and recompilers
class CellTranslator
{
public:
    virtual ~CellTranslator() = default;

    // Decode and execute a single instruction, block, method or game
    virtual void step()=0;
};
