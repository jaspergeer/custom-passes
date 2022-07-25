#include "MulToBitshift.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/raw_ostream.h"

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

        // find the integer operand
        unsigned int IntOpIndex = 0;

        if (!BinOp->getOperand(IntOpIndex)->getType()->isIntegerTy()) {
            ++IntOpIndex;
            if (!BinOp->getOperand(IntOpIndex)->getType()->isIntegerTy())
                continue;
        }

        // skip instruction if operand is not a power of 2
        auto *IntOperand = dyn_cast<ConstantInt>(BinOp->getOperand(IntOpIndex));
        if (!IntOperand) {
            ++IntOpIndex;
            IntOperand = dyn_cast<ConstantInt>(BinOp->getOperand(IntOpIndex));
            if (!IntOperand)
                continue;
        }

        uint64_t OpValue = IntOperand->getSExtValue();
        if (!isPowerOf2_64(OpValue))
            continue;

        uint64_t LShiftAmount = static_cast<uint64_t>(log2(OpValue));

        // create left shift instruction
        Instruction *NewInst = BinaryOperator::CreateShl(
            BinOp->getOperand(1 - IntOpIndex),
            ConstantInt::get(BinOp->getType(), LShiftAmount)
        );

        LLVM_DEBUG(dbgs() << *BinOp << " -> " << *NewInst << "\n");
        
        // replace mul instruction with equivalent bitshift instruction
        ReplaceInstWithInst(BB.getInstList(), Inst, NewInst);
        Changed = true;
    }
    return Changed;
}

PreservedAnalyses MulToBitshift::run(llvm::Function &F,
                                     llvm::FunctionAnalysisManager &) {
    bool Changed = false;
    for (auto &BB : F) {
        Changed |= runOnBasicBlock(BB);
    }
    return (Changed ? llvm::PreservedAnalyses::none()
                    : llvm::PreservedAnalyses::all());
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