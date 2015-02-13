/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_decoder.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_instruction.h"
#include "nucleus/cpu/ppu/ppu_tables.h"
#include "nucleus/cpu/ppu/analyzer/ppu_analyzer.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Scalar.h"

#include <queue>

namespace cpu {
namespace ppu {

/**
 * PPU Block methods
 */
bool Block::contains(u32 addr) const
{
    const u32 from = address;
    const u32 to = address + size;
    return from <= addr && addr < to;
}

/**
 * PPU Function methods
 */
void Function::get_type()
{
    Analyzer status;
    Block block = blocks[address];

    // Analyze read/written registers
    for (u32 offset = 0; offset < block.size; offset += 4) {
        Instruction code = { nucleus.memory.read32(block.address + offset) };

        // Get instruction analyzer and call it
        auto method = get_entry(code).analyzer;
        (status.*method)(code);
        
        if (code.is_branch_conditional() || code.is_return()) {
            break;
        }
        if (code.is_branch_unconditional()) {
            block = blocks[block.branch_a];
            offset = 0;
        }
    }

    // Determine type of function arguments
    for (u32 reg = 0; reg < 13; reg++) {
        if ((status.gpr[reg + 3] & REG_READ_ORIG) && reg < 8) {
            type_in.push_back(FUNCTION_IN_INTEGER);
        }
        if ((status.fpr[reg + 1] & REG_READ_ORIG) && reg < 13) {
            type_in.push_back(FUNCTION_IN_FLOAT);
        }
        if ((status.vr[reg + 2] & REG_READ_ORIG) && reg < 12) {
            type_in.push_back(FUNCTION_IN_VECTOR);
        }
    }

    // Determine type of function return
    type_out = FUNCTION_OUT_VOID;
    if (status.gpr[3] & REG_WRITE) {
        type_out = FUNCTION_OUT_INTEGER;
    }
    if (status.vr[2] & REG_WRITE) {
        type_out = FUNCTION_OUT_VECTOR;
    }
    if (status.fpr[1] & REG_WRITE) {
        type_out = FUNCTION_OUT_FLOAT;
        if (status.fpr[2] & REG_WRITE) {
            type_out = FUNCTION_OUT_FLOAT_X2;
        }
        if (status.fpr[3] & REG_WRITE) {
            type_out = FUNCTION_OUT_FLOAT_X3;
        }
        if (status.fpr[4] & REG_WRITE) {
            type_out = FUNCTION_OUT_FLOAT_X4;
        }
    }
}

bool Function::analyze(u32 segAddress, u32 segSize)
{
    blocks.clear();
    type_in.clear();

    std::queue<u32> labels({ address });

    Segment segment(segAddress, segSize);  // Segment that contains this function (to determine containment)
    Instruction code;                      // Current instruction
    Block current;                         // Current block
    current.initial = false;

    // Control Flow Graph generation
    while (!labels.empty()) {
        u32 addr = labels.front();
        code.instruction = nucleus.memory.read32(addr);

        // Initial Block properties
        current.address = addr;
        current.size = 4;
        current.branch_a = 0;
        current.branch_b = 0;

        bool continueLoop = false;
        for (auto& item : blocks) {
            Block& block_a = item.second;
            // Split block if label (Block B) is inside an existing block (Block A)
            if (block_a.contains(addr)) {
                // Configure Block B
                Block block_b{};
                block_b.address = addr;
                block_b.size = block_a.size - (addr - block_a.address);
                block_b.branch_a = block_a.branch_a;
                block_b.branch_b = block_a.branch_b;

                // Update Block A and push Block B
                block_a.size = addr - block_a.address;
                block_a.branch_a = addr;
                block_a.branch_b = 0;
                blocks[addr] = block_b;
                continueLoop = true;
                break;
            }
        }
        if (continueLoop) {
            labels.pop();
            continue;
        }

        // Wait for the end
        while (!code.is_branch() || code.is_call()) {
            addr += 4;
            current.size += 4;
            code.instruction = nucleus.memory.read32(addr);
        }

        // Push new labels
        if (code.is_branch_conditional()) {
            const u32 target_a = code.get_target(addr);
            const u32 target_b = addr + 4;
            if (!segment.contains(target_a) || !segment.contains(target_b)) {
                return false;
            }
            labels.push(target_a);
            labels.push(target_b);
            current.branch_a = target_a;
            current.branch_b = target_b;
        }
        if (code.is_branch_unconditional()) {
            const u32 target = code.get_target(addr);
            if (!segment.contains(target)) {
                return false;
            }
            labels.push(target);
            current.branch_a = target;
        }

        blocks[labels.front()] = current;
        labels.pop();
    }
    
    // Determine function arguments/return types
    get_type();

    return true;
}

void Function::recompile()
{
    // Return type
    llvm::Type* result = nullptr;
    switch (type_out) {
    case FUNCTION_OUT_INTEGER:
        result = llvm::Type::getInt64Ty(llvm::getGlobalContext());
        break;
    case FUNCTION_OUT_FLOAT:
        result = llvm::Type::getDoubleTy(llvm::getGlobalContext());
        break;
    case FUNCTION_OUT_FLOAT_X2:
        result = llvm::Type::getDoubleTy(llvm::getGlobalContext()); // TODO
        break;
    case FUNCTION_OUT_FLOAT_X3:
        result = llvm::Type::getDoubleTy(llvm::getGlobalContext()); // TODO
        break;
    case FUNCTION_OUT_FLOAT_X4: 
        result = llvm::Type::getDoubleTy(llvm::getGlobalContext()); // TODO
        break;
    case FUNCTION_OUT_VECTOR:
        result = llvm::Type::getIntNTy(llvm::getGlobalContext(), 128);
        break;
    case FUNCTION_OUT_VOID:
        result = llvm::Type::getVoidTy(llvm::getGlobalContext());
        break;
    }

    // Arguments type
    std::vector<llvm::Type*> params;
    for (auto& type : type_in) {
        switch (type) {
        case FUNCTION_OUT_INTEGER:
            params.push_back(llvm::Type::getInt64Ty(llvm::getGlobalContext()));
            break;
        case FUNCTION_OUT_FLOAT:
            params.push_back(llvm::Type::getDoubleTy(llvm::getGlobalContext()));
            break;
        case FUNCTION_OUT_VECTOR:
            params.push_back(llvm::Type::getIntNTy(llvm::getGlobalContext(), 128));
            break;
        }
    }
    
    ftype = llvm::FunctionType::get(result, params, false);
    function = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, name, nullptr/*TODO*/);
}

/**
 * PPU Segment methods
 */
Segment::Segment(u32 address, u32 size) : address(address), size(size) {
    std::string name = format("seg_%d", address);
    module = new llvm::Module(name, llvm::getGlobalContext());

    /**
     * Optimizations
     */
    fpm = new llvm::FunctionPassManager(module);

    // Promote allocas to registers
    fpm->add(llvm::createPromoteMemoryToRegisterPass());

    // Simple peephole and bit-twiddling optimizations
    fpm->add(llvm::createInstructionCombiningPass());

    // Reassociate expressions
    fpm->add(llvm::createReassociatePass());

    // Eliminate Common SubExpressions
    fpm->add(llvm::createGVNPass());

    // Simplify the Control Flow Graph (e.g.: deleting unreachable blocks)
    fpm->add(llvm::createCFGSimplificationPass());

    fpm->doInitialization();
}

Segment::~Segment() {
    delete module;
}

void Segment::analyze()
{
    // List of labels, stored as blocks.
    std::map<u32, Block> blocks;

    // Current basic block being processed
    Block current;

    // Basic Block Slicing: Classify them as starting or non-starting blocks
    for (u32 i = address; i < (address + size); i += 4) {
        const Instruction code = { nucleus.memory.read32(i) };

        // New block appeared
        if (code.is_valid() && current.address == 0) {
            if (blocks.find(i) != blocks.end()) {
                current = blocks[i];
            } else {
                current.address = i;
            }
        }

        // Block is corrupt
        if (current.address != 0 && !code.is_valid()) {
            current.address = 0;
        }

        // Block finished
        if (current.address != 0 && code.is_branch() && !code.is_call()) {
            if (code.is_branch_conditional()) {
                const u32 target_a = code.get_target(i);
                const u32 target_b = i + 4;
                blocks.emplace(target_a, Block{});
                blocks.emplace(target_b, Block{});
                blocks[target_a].address = target_a;
                blocks[target_b].address = target_b;
                blocks[target_a].initial = false;
                blocks[target_b].initial = false;
            }
            if (code.is_branch_unconditional()) {
                const u32 target = code.get_target(i);
                blocks.emplace(target, Block{});
                blocks[target].address = target;
                blocks[target].initial = false;
            }
            blocks[current.address] = current;
            current = Block{};
        }
    }

    // List the functions and analyze them
    for (const auto& item : blocks) {
        const Block& block = item.second;
        if (block.initial) {
            Function function(block.address);
            if (function.analyze(address, size)) {
                functions.push_back(function);
            }
        }
    }
}

void Segment::recompile() {
    for (auto& function : functions) {
        function.recompile();
    }
}

bool Segment::contains(u32 addr) const
{
    const u32 from = address;
    const u32 to = address + size;
    return from <= addr && addr < to;
}

}  // namespace ppu
}  // namespace cpu
