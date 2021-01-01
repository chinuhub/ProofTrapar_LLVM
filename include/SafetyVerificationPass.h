/**
 * @file Program.h
 *
 * @brief Declares SafetyVerificationPass: new pass manager and legacy pass manager
 *
 * @author Yaduraj Rao
 */

#ifndef SAFETY_VERIFICATION_PASS_H_
#define SAFETY_VERIFICATION_PASS_H_

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

/**
 * New PM Interface
 */
class SafetyVerificationPass : public llvm::PassInfoMixin<SafetyVerificationPass> {
 public:
  explicit SafetyVerificationPass(llvm::raw_ostream& _OS) : OS(_OS) {}
  llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager& AM);

 private:
  llvm::raw_ostream& OS;
};

/**
 * Legacy PM Interface
 */
struct LegacySafetyVerificationPass : public llvm::ModulePass {
  static char ID;
  LegacySafetyVerificationPass() : llvm::ModulePass(ID) {}
  bool runOnModule(llvm::Module& M) override;
};

#endif
