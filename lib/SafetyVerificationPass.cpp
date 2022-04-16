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
#include "MetaState.h"

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
  //Storing PA Generator in MetaState for onthefly construction.
  MetaState::generator = generator;
  MetaState::generator.DotWrite("my__PA.dot");

  //store accepting states of PA in markedStatesSet and use markedStatesSet in on-the-fly algorithm
  faudes::StateSet::Iterator lit;

  for(lit = MetaState::generator.MarkedStatesBegin(); lit != MetaState::generator.MarkedStatesEnd(); ++lit) {

      MetaState::markedStatesSet.insert(*lit);          //accepting states of PA inserted into markedStatesSet set
  }


#ifdef DBGPRNT
  std::cout<<"Minimization done "<<std::endl;
#endif
	size_t length;
	//fa_example(revmerged,&word,&length);
	//std::string revword(word,length);
  int i;
  int cases=0;


    faudes::StateSet::Iterator sit;
    sit = MetaState::generator.InitStatesBegin();

    auto mystart1 =  boost::chrono::system_clock::now();

    std::string rev = MetaState::getAcceptingWord(*sit, "");

    auto myend1 = boost::chrono::system_clock::now();
    auto myelapsed1 = boost::chrono::duration_cast<boost::chrono::duration<double> >(myend1- mystart1).count();
    std::cout<<"************************ Iteration no = "<<MetaState::proof_no + 1<<" ********************"<<std::endl;
    std::cout << "Time spent in On-The-fly = "<<myelapsed1 << "seconds "<<'\n';
    std::cout<<"Metastates created = "<<MetaState::count_metastates<<std::endl;


    std::string trace;

//  std::string rev = MetaState::getUncoveredTrace(MetaState::generator, MetaState::afaRoots);
//  std::string trace;

  while( rev.compare("None")!=0) //Keep on iterating until no uncovered trace is found.
  {
#ifdef DBGPRNT
    std::cout<<"Original is "<<original<<std::endl;
#endif

#ifdef DBGPRNT
    std::cout<<"Getting accepted state for "<<rev<<std::endl;
#endif

    trace = Utils::ReverseWord(rev);

    z3::expr wordphi= T->GetEndStateAssertionFromWord(trace);
    std::string exword = trace;


    std::cout<<"Checking word "<<exword<<" with postcondition phi = "<<wordphi<<std::endl;

		z3::expr negphi = !wordphi;
		//struct fa* prooffa = AFAut::MakeAFAutProof(exword,negphi,P,cases);
		bool result=false;
		faudes::Generator proofgens;
		AFAut* afa = AFAut::MakeAFAutProof(exword,negphi,P,cases,result,proofgens);
		if(result==false)//return null if hmap of initial state is unsat.
		{
			std::cout<<"An errorneous trace "<<std::endl;
			std::cout<<exword<<std::endl;
			auto end = boost::chrono::system_clock::now();
			auto elapsed = boost::chrono::duration_cast<boost::chrono::duration<double> >(end- start).count();
			std::cout << "Time spent = "<<elapsed << "seconds "<<'\n';
			std::exit(-1);
		}

        //MetaState::afaRoots.push_back(afa->mInit);

        if(MetaState::afaRoots.empty())
            MetaState::afaRoots.push_back(afa->mInit);
        else{
            MetaState::afaRoots.pop_back();
            MetaState::afaRoots.push_back(afa->mInit);
        }

        std::cout<<"AFAs for on the fly = "<<MetaState::afaRoots.size()<<std::endl;

#ifdef SANITYCHKASSN
    std::cout<<"AFA construction over-complement.. dumping to complemented.dot file.Press any int to continue"<<std::endl;
    bool res= checkAccepting(original,prooffa);
    BOOST_ASSERT_MSG(res==false,"Some serious error as this word must not be accepted by this complemented FA");
#endif
#ifdef DBGPRNT
    std::cout<<"Original: States="<<generator.States().Size()<<" Transitions = "<<generator.TransRel().Size()<<std::endl;
#endif


      auto mystart2 =  boost::chrono::system_clock::now();

      rev = MetaState::getUncoveredTrace(MetaState::generator, MetaState::afaRoots);

      auto myend2 = boost::chrono::system_clock::now();
      auto myelapsed2 = boost::chrono::duration_cast<boost::chrono::duration<double> >(myend2- mystart2).count();
      std::cout<<"****************************  Iteration no = "<<MetaState::proof_no + 1<<" *********************"<<std::endl;
      std::cout << "Time spent in On-The-fly = "<<myelapsed2 << "seconds "<<'\n';
      std::cout<<"Metastates created = "<<MetaState::count_metastates<<std::endl;



      std::cout<<"Trace ==== "<<rev<<std::endl;
      cases++;
      std::cout<<cases<<std::endl;
	}
	std::cout<<"Total cases = "<<cases<<std::endl;
 	//delete P;
 	//delete T;
 	auto end = boost::chrono::system_clock::now();
 	auto   elapsed = boost::chrono::duration_cast<boost::chrono::duration<double> >(end- start).count();
 	std::cout << "Time spent = "<<elapsed << "seconds "<<'\n';
 	//std::cout << "Cas instruction has been successfully implemented";
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
