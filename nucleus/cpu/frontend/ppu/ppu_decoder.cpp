/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "ppu_decoder.h"
#include "nucleus/memory/memory.h"
#include "nucleus/cpu/util.h"
#include "nucleus/cpu/hir/builder.h"
#include "nucleus/cpu/hir/function.h"
#include "nucleus/cpu/frontend/ppu/ppu_instruction.h"
#include "nucleus/cpu/frontend/ppu/ppu_state.h"
#include "nucleus/cpu/frontend/ppu/ppu_tables.h"

#include <algorithm>
#include <iterator>
#include <queue>

namespace cpu {
namespace frontend {
namespace ppu {

/**
 * PPU Block methods
 */
bool Block::is_split() const
{
    Instruction lastInstr;
    lastInstr.value = parent->parent->parent->memory->read32(address + size - 4);
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
    Block currentBlock = static_cast<Block&>(*blocks[address]);
    for (U32 i = currentBlock.address; i < (currentBlock.address + currentBlock.size); i += 4) {
        Instruction code;
        code.value = parent->parent->memory->read32(i);

        // Check if called functions use any other registers
        if (code.is_call_known()) {
            auto& targetFunc = static_cast<Function&>(*parent->functions[code.get_target(i)]);
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
            currentBlock = *blocks[currentBlock.branch_a];
            i = currentBlock.address;
        }
    }
}

bool Function::analyze_cfg()
{
    blocks.clear();
    type_in.clear();

    std::queue<U32> labels({ address });

    Instruction code;  // Current instruction
    Block current;     // Current block
    current.parent = this;
    current.initial = false;

    // Control Flow Graph generation
    while (!labels.empty()) {
        U32 addr = labels.front();
        code.value = parent->parent->memory->read32(addr);

        // Check if block was already processed
        if (blocks.find(addr) != blocks.end()) {
            labels.pop();
            continue;
        }

        // Initial Block properties
        current.address = addr;
        current.size = 4;
        current.branch_a = 0;
        current.branch_b = 0;

        U32 maxSize = 0xFFFFFFFF;
        bool continueLoop = false;
        for (auto& item : blocks) {
            auto& block_a = *item.second;
            // Determine maximum possible size for the current block
            if ((block_a.address - current.address) < maxSize) {
                maxSize = block_a.address - current.address;
            }
            // Split block if label (Block B) is inside an existing block (Block A)
            if (block_a.contains(addr)) {
                auto block_b = block_a.split(addr);
                blocks[addr] = new Block(block_b);
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
            code.value = parent->parent->memory->read32(addr);
        }

        // Push new labels
        if (code.is_branch_conditional() && !code.is_call()) {
            const U32 target_a = code.get_target(addr);
            const U32 target_b = addr + 4;
            if (!parent->contains(target_a) || !parent->contains(target_b)) {
                return false;
            }
            labels.push(target_a);
            labels.push(target_b);
            current.branch_a = target_a;
            current.branch_b = target_b;
        }
        if (code.is_branch_unconditional() && !code.is_call()) {
            const U32 target = code.get_target(addr);
            if (!parent->contains(target)) {
                return false;
            }
            labels.push(target);
            current.branch_a = target;
        }

        blocks[labels.front()] = new Block(current);
        labels.pop();
    }
    return true;
}

void Function::analyze_type()
{
    // Determine function arguments/return types
    Analyzer status;
    do_register_analysis(&status);

    // Determine type of function arguments
    for (U32 reg = 0; reg < 13; reg++) {
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

void Function::declare()
{
    hir::Module* hirModule = parent->hirModule;

    // Return type
    hir::Type result = hir::TYPE_VOID;
    switch (type_out) {
    case FUNCTION_OUT_INTEGER:
        result = hir::TYPE_I64;
        break;
    case FUNCTION_OUT_FLOAT:
        result = hir::TYPE_F64;
        break;
    case FUNCTION_OUT_FLOAT_X2:
        result = hir::TYPE_F64; // TODO
        break;
    case FUNCTION_OUT_FLOAT_X3:
        result = hir::TYPE_F64; // TODO
        break;
    case FUNCTION_OUT_FLOAT_X4:
        result = hir::TYPE_F64; // TODO
        break;
    case FUNCTION_OUT_VECTOR:
        result = hir::TYPE_V128;
        break;
    case FUNCTION_OUT_VOID:
        result = hir::TYPE_VOID;
        break;
    }

    // Arguments type
    std::vector<hir::Type> params;
    for (auto& type : type_in) {
        switch (type) {
        case FUNCTION_OUT_INTEGER:
            params.push_back(hir::TYPE_I64);
            break;
        case FUNCTION_OUT_FLOAT:
            params.push_back(hir::TYPE_F64);
            break;
        case FUNCTION_OUT_VECTOR:
            params.push_back(hir::TYPE_V128);
            break;
        }
    }

    // Declare function in module
    hirFunction = new hir::Function(hirModule, result, params);
}

void Function::recompile()
{
    Recompiler recompiler(parent->parent, this);

    hir::Builder& builder = recompiler.builder;

    hirFunction->reset();

    // Declare CFG blocks
    for (const auto& item : blocks) {
        const U32 labelAddr = item.first;
        recompiler.blocks[labelAddr] = new hir::Block(hirFunction);
    }

    // Generate prolog/epilog blocks
    recompiler.blocks[address]->flags |= hir::BLOCK_IS_ENTRY;
    recompiler.createProlog();
    recompiler.createEpilog();

    // Recompile basic clocks
    std::queue<U32> labels({ address });
    for (auto& item : blocks) {
        auto& block = static_cast<Block&>(*item.second);

        // Recompile block instructions
        builder.setInsertPoint(recompiler.blocks[block.address]);

        // Get function (TODO: This gets loaded multiple times into the module)
        //hir::Function* logFunc = builder.getExternFunction(nucleusLog);

        for (U32 offset = 0; offset < block.size; offset += 4) {
            recompiler.currentAddress = block.address + offset;
            Instruction instr;
            instr.value = parent->parent->memory->read32(recompiler.currentAddress);
            auto method = get_entry(instr).recompile;
            //builder.createCall(logFunc, {builder.getConstantI64(recompiler.currentAddress)}, hir::CALL_EXTERN);
            (recompiler.*method)(instr);
        }

        // Block was splitted
        if (block.is_split()) {
            const U32 target = block.address + block.size;
            if (blocks.find(target) != blocks.end()) {
                builder.createBr(recompiler.blocks[target]);
            }
        }
    }

    // Validate the generated code, checking for consistency (TODO: Remove this once the recompiler is stable)
    //llvm::verifyFunction(*function.function, &llvm::outs());
}

void Function::createPlaceholder()
{
    hir::Builder builder;
    hir::Block* block = new hir::Block(hirFunction);
    builder.setInsertPoint(block);

    hir::Function* translateFunc = builder.getExternFunction(nucleusTranslate);
    hir::Value* guestFuncValue = builder.getConstantPointer(this);
    hir::Value* guestAddrValue = builder.getConstantI64(address);
    builder.createCall(translateFunc, {guestFuncValue, guestAddrValue}, hir::CALL_EXTERN);
    builder.createRet();
}

/**
 * PPU Module methods
 */
Module::Module(CPU* parent) : frontend::Module<U32>(parent) {
}

Function* Module::addFunction(U32 addr)
{
    // Return function if already present
    if (functions.find(addr) != functions.end()) {
        return static_cast<Function*>(functions[addr]);
    }

    // Create the function otherwise
    Function* function = new Function(this);
    function->name = format("func_%08X", addr);
    function->address = addr;
    function->declare();
    function->createPlaceholder();
    parent->compiler->compile(function->hirFunction);

    // Save and return the function
    functions[addr] = function;
    return function;
}

void Module::analyze()
{
    // Lists of labels
    std::set<U32> labelBlocks;  // Detected immediately
    std::set<U32> labelCalls;   // Direct target of a {bl*, bcl*} instruction (call)
    std::set<U32> labelJumps;   // Direct or indirect target of a {b, ba, bc, bca} instruction (jump)

    // Basic Block Slicing
    U32 currentBlock = 0;
    for (U32 i = address; i < (address + size); i += 4) {
        Instruction instr;
        instr.value = parent->memory->read32(i);

        // New block appeared
        if (currentBlock == 0 && instr.is_valid()) {
            currentBlock = i;
        }

        // Block is corrupt
        if (currentBlock != 0 && !instr.is_valid()) {
            currentBlock = 0;
        }

        // Function call detected
        if (currentBlock != 0 && instr.is_call()) {
            labelCalls.insert(instr.get_target(i));
        }

        // Block finished
        if (currentBlock != 0 && instr.is_branch() && !instr.is_call()) {
            if (instr.is_branch_conditional()) {
                labelJumps.insert(instr.get_target(i));
                labelJumps.insert(i + 4);
            }
            if (instr.is_branch_unconditional()) {
                labelJumps.insert(instr.get_target(i));
            }
            labelBlocks.insert(currentBlock);
            currentBlock = 0;
        }
    }

    // Functions := ((Blocks \ Jumps) U Calls)
    std::set<U32> labelFunctions;
    std::set_difference(labelBlocks.begin(), labelBlocks.end(), labelJumps.begin(), labelJumps.end(), std::inserter(labelFunctions, labelFunctions.end()));
    std::set_union(labelFunctions.begin(), labelFunctions.end(), labelCalls.begin(), labelCalls.end(), std::inserter(labelFunctions, labelFunctions.end()));

    // List the functions and get their CFG
    for (const auto& label : labelFunctions) {
        if (this->contains(label)) {
            Function function(this);
            function.name = format("func_%X", label);
            function.address = label;
            if (function.analyze_cfg()) {
                functions[label] = new Function(function);
            }
        }
    }
    // Get type of every listed function
    for (auto& item : functions) {
        auto& function = static_cast<Function&>(*item.second);
        function.analyze_type();
    }
}

void Module::recompile()
{
    //module = hir::Module::Create(name);

    // Optimization passes
    /*auto fpm = new llvm::legacy::FunctionPassManager(module.module);
    fpm->add(llvm::createPromoteMemoryToRegisterPass());  // Promote allocas to registers
    fpm->add(llvm::createInstructionCombiningPass());     // Simple peephole and bit-twiddling optimizations
    fpm->add(llvm::createReassociatePass());              // Reassociate expressions
    fpm->add(llvm::createGVNPass());                      // Eliminate Common SubExpressions
    fpm->add(llvm::createCFGSimplificationPass());        // Simplify the Control Flow Graph (e.g.: deleting unreachable blocks)
    fpm->doInitialization();*/

    // Global variables
    /*memoryBase = module.getOrInsertGlobal<hir::I64>("memoryBase");
    funcGetState = hir::Function::Create(
        llvm::FunctionType::get(hir::Pointer<StateType>::getType().type, false),
        llvm::Function::ExternalLinkage, "nucleusGetState", module);
    funcLogState = hir::Function::Create(
        llvm::FunctionType::get(hir::Void::getType().type, { hir::I64::getType().type }, false),
        llvm::Function::ExternalLinkage, "nucleusLogState", module);
    funcDebugState = hir::Function::Create(
        llvm::FunctionType::get(hir::Void::getType().type, { hir::I64::getType().type }, false),
        llvm::Function::ExternalLinkage, "nucleusDebugState", module);
    funcIntermodularCall = hir::Function::Create(
        llvm::FunctionType::get(hir::Void::getType().type, { hir::I64::getType().type }, false),
        llvm::Function::ExternalLinkage, "nucleusIntermodularCall", module);
    funcSystemCall = hir::Function::Create(
        llvm::FunctionType::get(hir::Void::getType().type, false),
        llvm::Function::ExternalLinkage, "nucleusSystemCall", module);*/
    /*
    // Declare all functions
    for (auto& item : functions) {
        auto& function = static_cast<Function&>(*item.second);
        function.declare(module);
    }

    // Recompile and optimize all functions
    for (auto& item : functions) {
        auto& function = static_cast<Function&>(*item.second);
        function.recompile();
        fpm->run(*function.function.function); // TODO: This is unreadable
    }

    // Add function caller
    llvm::FunctionType* callerType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(llvm::getGlobalContext()),
        llvm::Type::getInt32Ty(llvm::getGlobalContext()),
        false);
    hir::Function callerFunction = hir::Function::Create(callerType, llvm::Function::InternalLinkage, "caller", module);
    hir::Block entryBlock = hir::Block::Create("entry", callerFunction);
    hir::Block defaultBlock = hir::Block::Create("caller", callerFunction);

    hir::Builder builder;
    builder.setInsertPoint(entryBlock);
    hir::Value* state = builder.createCall(funcGetState);
    auto switchInst = builder.createSwitch(hir::Value<hir::I32>{ callerFunction.function->arg_begin() }, defaultBlock);

    for (auto& item : functions) {
        auto& function = static_cast<Function&>(*item.second);

        // Declare block
        hir::Block callerBlock = hir::Block::Create(function.name + "_caller", callerFunction);
        switchInst->addCase(llvm::ConstantInt::get(
            llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()), function.address),
            callerBlock.bb);

        // Proxy
        int index = 0;
        builder.setInsertPoint(callerBlock);
        std::vector<hir::Value*> args;
        for (auto& type : function.type_in) {
            switch (type) {
            case FUNCTION_IN_INTEGER:
                args.push_back(builder.createLoad<hir::I64>(
                    builder.createInBoundsGEP(state, {
                        builder.get<hir::I32>(0),
                        builder.get<hir::I32>(0),
                        builder.get<hir::I32>(3 + index++)
                    })
                ));
                break;

            case FUNCTION_IN_FLOAT: {
                args.push_back(builder.createLoad<hir::I64>(
                    builder.createInBoundsGEP(state, {
                        builder.get<hir::I32>(0),
                        builder.get<hir::I32>(1),
                        builder.get<hir::I32>(1 + index++)
                    })
                ));
                break;
            }
            case FUNCTION_IN_VECTOR: {
                // TODO
                break;
            }
            default:
                break;
            }
        }

        hir::Value* value = builder.createCall(function.function, args);

        // TODO: ?

        builder.createRetVoid();
    }

    builder.SetInsertPoint(defaultBlock);
    builder.createRetVoid();

    // NOTE: Debugging purposes
    module.dump();

    // Compile
    backend::Generate(static_cast<frontend::Module<U32>*>(this));*/
}

void Module::hook(U32 funcAddr, U32 fnid) {
    if (functions.find(funcAddr) == functions.end()) {
        auto* func = new Function(this);
        func->declare();
        functions[funcAddr] = func;
    }
    auto* hirFunc = functions[funcAddr]->hirFunction;
    hirFunc->reset();

    hir::Builder builder;
    hir::Block* block = new hir::Block(hirFunc);
    block->flags |= hir::BLOCK_IS_ENTRY;
    builder.setInsertPoint(block);

    hir::Function* hookFunc = builder.getExternFunction(nucleusHook);
    builder.createCall(hookFunc, { builder.getConstantI32(fnid) }, hir::CALL_EXTERN);
    builder.createRet();

    parent->compiler->compile(hirFunc);
}

}  // namespace ppu
}  // namespace frontend
}  // namespace cpu
