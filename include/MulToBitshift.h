#ifndef MULDIV_TO_BITSHIFT_H_
#define MULDIV_TO_BITSHIFT_H_

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

/**
 * Transform pass replacing mul instructions with a power of 2 operand with
 * an equivalent shl instruction
 */
struct MulToBitshift : public llvm::PassInfoMixin<MulToBitshift> {
    llvm::PreservedAnalyses run(llvm::Function &F,
                                llvm::FunctionAnalysisManager &);
    bool runOnBasicBlock(llvm::BasicBlock &B);
    // a similar pass is included with -O1 so allow this to run on IR generated with -O0
    static bool isRequired() { return true; };
};

#endif