/**
 * @file SafetyVerificationPass.cpp
 *
 * @brief Implements New PM and Legacy PM registeration and Test Bench
 *
 * @author Yaduraj Rao
 */
#include "SafetyVerificationPass.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "z3++.h"

#include "Program.h"

using namespace llvm;

//------------------------------------------------------------------------------
// Test Bench implementation
//------------------------------------------------------------------------------
bool test_bench(Module& M) {
  Program program(M);
  return false;
}

PreservedAnalyses SafetyVerificationPass::run(Module& M, ModuleAnalysisManager& AM) {
  test_bench(M);

  return PreservedAnalyses::all();
}

bool LegacySafetyVerificationPass::runOnModule(Module& M) {
  test_bench(M);

  return false;
}

//------------------------------------------------------------------------------
// New PM Registeration
//------------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getSafetyVerificationPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "SafetyVerificationPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "verify-module-safety") {
                    MPM.addPass(SafetyVerificationPass(llvm::errs()));
                    return true;
                  }
                  return false;
                });
          }
        };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getSafetyVerificationPassPluginInfo();
}

//------------------------------------------------------------------------------
// Legacy PM Registeration
//------------------------------------------------------------------------------
char LegacySafetyVerificationPass::ID = 0;

static RegisterPass<LegacySafetyVerificationPass> X("legacy-verify-module-safety",
                                                    "Legacy Safety Verification Pass",
                                                    true,
                                                    false);
