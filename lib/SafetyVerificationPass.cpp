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
#include "libfaudes.h"
#include "boost/chrono.hpp"
#include "boost/assert.hpp"

#include "AFA/AFAut.h"
#include "SCTransSystem.h"
#include "Program.h"

#include <vector>
#include <sstream>

using namespace llvm;

//------------------------------------------------------------------------------
// Test Bench implementation
//------------------------------------------------------------------------------
bool test_bench(Module& M) {
//------------------------------------------------------------------------------
// Old Interface here
//------------------------------------------------------------------------------
  auto start =  boost::chrono::system_clock::now();
	faudes::Generator generator,lGenerator;
	Program* P = new Program(M);
	z3::context& ctx = P->context_;
	z3::solver s(ctx);
	SCTransSystem* T = new SCTransSystem(*P,s);
	T->BuildSCTS(lGenerator);//it has initial values as well with them..This will construct reverse of shuffle
	//it fills the generator as well..
  //lGenerator.StateNamesEnabled(false);
  faudes::Deterministic(lGenerator,generator);
  generator.DotWrite("DetermOriginal.dot");
  std::cout<<"Determinization done "<<std::endl;
  generator.StateNamesEnabled(false);
  faudes::aStateMin(generator);
#ifdef DBGPRNT
  std::cout<<"Minimization done "<<std::endl;
#endif
	char* word;
	size_t length;
	//fa_example(revmerged,&word,&length);
	//std::string revword(word,length);
  int i;
  int cases=0;
  while(!faudes::IsEmptyLanguage(generator))
  {
    std::string revword(T->GetWord(generator));
    std::string original(revword);//for some debugging purposes
#ifdef DBGPRNT
    std::cout<<"Original is "<<original<<std::endl;
#endif

    std::string rev= Utils::ReverseWord(revword); //reverse the word to get it back.

#ifdef DBGPRNT
    std::cout<<"Getting accepted state for "<<rev<<std::endl;
#endif

    z3::expr wordphi= T->GetEndStateAssertionFromWord(rev);
    std::string exword = rev;

    std::cout<<"Checking word "<<exword<<" with postcondition phi = "<<wordphi<<std::endl;

		z3::expr negphi = !wordphi;
		//struct fa* prooffa = AFAut::MakeAFAutProof(exword,negphi,P,cases);
		bool result=false;
		faudes::Generator proofgens;
		AFAut::MakeAFAutProof(exword,negphi,P,cases,result,proofgens);
		//what we got is already complemented version..
		if(result==false)//return null if hmap of initial state is unsat.
		{
			std::cout<<"An errorneous trace "<<std::endl;
			std::cout<<exword<<std::endl;
			auto end = boost::chrono::system_clock::now();
			auto elapsed = boost::chrono::duration_cast<boost::chrono::duration<double> >(end- start).count();
			std::cout << "Time spent = "<<elapsed << "seconds "<<'\n';
			std::exit(-1);
		}

#ifdef SANITYCHKASSN
    std::cout<<"AFA construction over-complement.. dumping to complemented.dot file.Press any int to continue"<<std::endl;
    bool res= checkAccepting(original,prooffa);
    BOOST_ASSERT_MSG(res==false,"Some serious error as this word must not be accepted by this complemented FA");
#endif
#ifdef DBGPRNT
    std::cout<<"Original: States="<<generator.States().Size()<<" Transitions = "<<generator.TransRel().Size()<<std::endl;
#endif
    //We have the result of the AFA in DFA variable proofgens.
    //We will first complement it and then intersect it with generator.
    //The result will be stored again in variable generator.
    //Check if proofgens has accepted states or not.
      faudes::LanguageComplement(proofgens);
      proofgens.DotWrite("Complemented.dot");
      faudes::Generator res;
      faudes::LanguageIntersection(generator,proofgens,res);

      faudes::Deterministic(res,generator);
       faudes::aStateMin(generator);
#ifdef DBGPRNT
     std::cout<<"Intersection: States = "<<generator.States().Size()<<" Transitions = "<<generator.TransRel().Size()<<std::endl;
#endif
    //generator.Assign(intersectionres);
 		cases++;
	}
	std::cout<<"Total cases = "<<cases<<std::endl;
 	//delete P;
 	delete T;
 	auto end = boost::chrono::system_clock::now();
 	auto   elapsed = boost::chrono::duration_cast<boost::chrono::duration<double> >(end- start).count();
 	std::cout << "Time spent = "<<elapsed << "seconds "<<'\n';
 	std::cout << "Cas instruction has been successfully implemented";
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
