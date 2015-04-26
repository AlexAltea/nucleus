/**
 * (c) 2015 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_decoder.h"
#include "nucleus/emulator.h"
#include "nucleus/cpu/ppu/ppu_instruction.h"
#include "nucleus/cpu/ppu/ppu_state.h"
#include "nucleus/cpu/ppu/ppu_tables.h"

#include "llvm/ADT/Triple.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar.h"

#include <algorithm>
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

bool Block::is_split() const
{
    const Instruction lastInstr = { nucleus.memory.read32(address + size - 4) };
    if (!lastInstr.is_branch() || lastInstr.is_call() || (lastInstr.opcode == 0x13 && lastInstr.op19 == 0x210) /*bcctr*/) {
        return true;
    }
    return false;
}

/**
 * PPU Function methods
 */
void Function::do_register_analysis(Analyzer* status)
{
    // This function already went through the analyzer
    if (status->analyzedFunctions.find(address) != status->analyzedFunctions.end()) {
        return;
    }
    status->analyzedFunctions.insert(address);

    // Analyze read/written registers
    Block block = blocks[address];
    for (u32 i = block.address; i < (block.address + block.size); i += 4) {
        Instruction code = { nucleus.memory.read32(i) };

        // Check if called functions use any other registers
        if (code.is_call_known()) {
            Function& targetFunc = parent->functions[code.get_target(i)];
            targetFunc.do_register_analysis(status);
        }
        // Otherwise, get instruction analyzer and call it
        else {
            auto method = get_entry(code).analyze;
            (status->*method)(code);
        }

        if (code.is_branch_conditional() || code.is_return()) {
            break;
        }
        if (code.is_branch_unconditional() && !code.is_call()) {
            block = blocks[block.branch_a];
            i = block.address;
        }
    }
}

bool Function::analyze_cfg()
{
    blocks.clear();
    type_in.clear();

    std::queue<u32> labels({ address });

    Instruction code;  // Current instruction
    Block current;     // Current block
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

        u32 maxSize = 0xFFFFFFFF;
        bool continueLoop = false;
        for (auto& item : blocks) {
            Block& block_a = item.second;
            // Determine maximum possible size for the current block
            if ((block_a.address - current.address) < maxSize) {
                maxSize = block_a.address - current.address;
            }
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
        while ((!code.is_branch() || code.is_call()) && (current.size < maxSize)) {
            addr += 4;
            current.size += 4;
            code.instruction = nucleus.memory.read32(addr);
        }

        // Push new labels
        if (code.is_branch_conditional() && !code.is_call()) {
            const u32 target_a = code.get_target(addr);
            const u32 target_b = addr + 4;
            if (!parent->contains(target_a) || !parent->contains(target_b)) {
                return false;
            }
            labels.push(target_a);
            labels.push(target_b);
            current.branch_a = target_a;
            current.branch_b = target_b;
        }
        if (code.is_branch_unconditional() && !code.is_call()) {
            const u32 target = code.get_target(addr);
            if (!parent->contains(target)) {
                return false;
            }
            labels.push(target);
            current.branch_a = target;
        }

        blocks[labels.front()] = current;
        labels.pop();
    }
}

void Function::analyze_type()
{
    // Determine function arguments/return types
    Analyzer status;
    do_register_analysis(&status);

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

llvm::Function* Function::declare()
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
    std::vector<llvm::Type*> params = {
        // NOTE: Remove once MCJIT + TLS is supported in LLVM
        llvm::PointerType::get(State::type(), 0)
    };
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

    llvm::FunctionType* ftype = llvm::FunctionType::get(result, params, false);

    // Declare function in module
    function = llvm::Function::Create(ftype, llvm::Function::ExternalLinkage, name, parent->module);
    return function;
}

llvm::Function* Function::recompile()
{
    Recompiler recompiler(parent, this);
    recompiler.returnType = type_out;

    std::queue<u32> labels({ address });

    // Create LLVM basic blocks
    prolog = llvm::BasicBlock::Create(llvm::getGlobalContext(), "prolog", function);
    for (auto& item : blocks) {
        Block& block = item.second;
        const std::string name = format("block_%X", block.address);
        block.bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), name, function);
    }
    recompiler.createProlog();

    // Recompile basic clocks
    while (!labels.empty()) {
        auto& block = blocks[labels.front()];
        if (block.recompiled) {
            labels.pop();
            continue;
        }

        // Recompile block instructions
        recompiler.setInsertPoint(block.bb);
        for (u32 offset = 0; offset < block.size; offset += 4) {
            recompiler.currentAddress = block.address + offset;
            const Instruction code = { nucleus.memory.read32(recompiler.currentAddress) };
            auto method = get_entry(code).recompile;
            (recompiler.*method)(code);
        }

        // Block was splitted
        if (block.is_split()) {
            const u32 target = block.address + block.size;
            if (blocks.find(target) != blocks.end()) {
                recompiler.createBranch(blocks[target]);
            }
            // Required for .sceStub.text (single-block functions ending on bctr)
            else {
                recompiler.createReturn();
            }
        }

        block.recompiled = true;
        if (block.branch_a) {
            labels.push(block.branch_a);
        }
        if (block.branch_b) {
            labels.push(block.branch_b);
        }
        labels.pop();
    }

    // Validate the generated code, checking for consistency (TODO: Remove this once the recompiler is stable)
    llvm::verifyFunction(*function, &llvm::outs());
    return function;
}

/**
 * PPU Segment methods
 */
void Segment::analyze()
{
    // Lists of labels
    std::set<u32> labelBlocks;  // Detected immediately
    std::set<u32> labelCalls;   // Direct target of a {bl*, bcl*} instruction (call)
    std::set<u32> labelJumps;   // Direct or indirect target of a {b, ba, bc, bca} instruction (jump)

    // Basic Block Slicing
    u32 currentBlock = 0;
    for (u32 i = address; i < (address + size); i += 4) {
        const Instruction code = { nucleus.memory.read32(i) };

        // New block appeared
        if (code.is_valid() && currentBlock == 0) {
            currentBlock = i;
        }

        // Block is corrupt
        if (currentBlock != 0 && !code.is_valid()) {
            currentBlock = 0;
        }

        // Function call detected
        if (currentBlock != 0 && code.is_call()) {
            labelCalls.insert(code.get_target(i));
        }

        // Block finished
        if (currentBlock != 0 && code.is_branch() && !code.is_call()) {
            if (code.is_branch_conditional()) {
                labelJumps.insert(code.get_target(i));
                labelJumps.insert(i + 4);
            }
            if (code.is_branch_unconditional()) {
                labelJumps.insert(code.get_target(i));
            }
            labelBlocks.insert(currentBlock);
            currentBlock = 0;
        }
    }

    // Functions := ((Blocks \ Jumps) U Calls)
    std::set<u32> labelFunctions;
    std::set_difference(labelBlocks.begin(), labelBlocks.end(), labelJumps.begin(), labelJumps.end(), std::inserter(labelFunctions, labelFunctions.end()));
    std::set_union(labelFunctions.begin(), labelFunctions.end(), labelCalls.begin(), labelCalls.end(), std::inserter(labelFunctions, labelFunctions.end()));

    // List the functions and get their CFG
    for (const auto& label : labelFunctions) {
        if (this->contains(label)) {
            Function function(label, this);
            if (function.analyze_cfg()) {
                functions[label] = function;
            }
        }
    }
    // Get type of every listed function
    for (auto& item : functions) {
        Function& function = item.second;
        function.analyze_type();
    }
}

void Segment::recompile()
{
    module = new llvm::Module(name, llvm::getGlobalContext());

    // Optimization passes
    fpm = new llvm::FunctionPassManager(module);
    fpm->add(llvm::createPromoteMemoryToRegisterPass());  // Promote allocas to registers
    fpm->add(llvm::createInstructionCombiningPass());     // Simple peephole and bit-twiddling optimizations
    fpm->add(llvm::createReassociatePass());              // Reassociate expressions
    fpm->add(llvm::createGVNPass());                      // Eliminate Common SubExpressions
    fpm->add(llvm::createCFGSimplificationPass());        // Simplify the Control Flow Graph (e.g.: deleting unreachable blocks)
    fpm->doInitialization();

    // Global variables
    module->getOrInsertGlobal("memoryBase", llvm::Type::getInt64Ty(llvm::getGlobalContext()));
    memoryBase = module->getNamedGlobal("memoryBase");

    // Declare all functions
    for (auto& item : functions) {
        Function& function = item.second;
        function.declare();
    }

    // Recompile and optimize all functions
    for (auto& item : functions) {
        Function& function = item.second;
        llvm::Function* func = function.recompile();
        fpm->run(*func);
    }

    // NOTE: Avoid generating COFF objects on Windows which are not supported by MCJIT
    llvm::Triple triple(llvm::sys::getProcessTriple());
    if (triple.getOS() == llvm::Triple::OSType::Win32) {
        triple.setObjectFormat(llvm::Triple::ObjectFormatType::ELF);
    }
    module->setTargetTriple(triple.str());
    module->dump();

    // Create execution engine
    llvm::EngineBuilder engineBuilder(module);
    engineBuilder.setEngineKind(llvm::EngineKind::JIT);
    engineBuilder.setOptLevel(llvm::CodeGenOpt::Default);
    engineBuilder.setUseMCJIT(true);
    executionEngine = engineBuilder.create();
    executionEngine->addModule(nucleus.cell.module);
    executionEngine->finalizeObject();
}

bool Segment::contains(u32 addr) const
{
    const u32 from = address;
    const u32 to = address + size;
    return from <= addr && addr < to;
}

}  // namespace ppu
}  // namespace cpu
