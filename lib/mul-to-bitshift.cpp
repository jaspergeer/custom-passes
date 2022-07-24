#include "mul-to-bitshift.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

#define DEBUG_TYPE "mul-to-bitshift"

bool MulToBitshift::runOnBasicBlock(BasicBlock &BB) {
    bool Changed = false;

    for(auto Inst = BB.begin(), IE = BB.end(); Inst != IE; ++Inst) {
        auto *BinOp = dyn_cast<BinaryOperator>(Inst);

        // skip non-binary instructions
        if (!BinOp)
            continue;
        
        // skip instructions other than mul
        if (BinOp->getOpcode() != Instruction::Mul)
            continue;

        // if there is an integer operand place it at index 1
        if (!BinOp->getOperand(1)->getType()->isIntegerTy())
            BinOp->swapOperands();

        if (!BinOp->getOperand(1)->getType()->isIntegerTy())
            continue;

        // skip instruction if operand is not a power of 2
        uint64_t IntOperand = dyn_cast<ConstantInt>(BinOp->getOperand(1))->getSExtValue();
        if (!isPowerOf2_64(IntOperand))
            continue;

        uint64_t LShiftAmount = static_cast<uint64_t>(log2(IntOperand));

        IRBuilder<> Builder(BinOp);

        // create left shift instruction
        Instruction *NewInst = BinaryOperator::CreateShl(
            BinOp->getOperand(0),
            ConstantInt::get(Type::getInt8Ty(BB.getContext()), LShiftAmount)
        );

        LLVM_DEBUG(dbgs() << *BinOp << " -> " << *NewInst << "\n");
        
        // replace mul instruction with equivalent bitshift instruction
        ReplaceInstWithInst(BB.getInstList(), Inst, NewInst);
        Changed = true;
    }
    return Changed;
}

// new PM registration
llvm::PassPluginLibraryInfo getMulToBitshiftPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "mul-to-bitshift", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM,
            ArrayRef<PassBuilder::PipelineElement>) {
                if (Name == "mul-to-bitshift") {
                    FPM.addPass(MulToBitshift());
                    return true;
                }
                return false;
            });
        }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getMulToBitshiftPluginInfo();
}