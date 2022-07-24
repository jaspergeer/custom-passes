#ifndef MULDIV_TO_BITSHIFT_H_
#define MULDIV_TO_BITSHIFT_H_

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

// New PM interface 
struct MulToBitshift : public llvm::PassInfoMixin<MulToBitshift> {
    llvm::PreservedAnalyses run(llvm::Function &F,
                                llvm::FunctionAnalysisManager &);
    bool runOnBasicBlock(llvm::BasicBlock &B);
};

#endif