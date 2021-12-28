#include "Utils.h"
#include "AFA/AFAState.h"
#include "AFA/AFAut.h"
#include <map>
#include <stdlib.h>
#include "MetaState.h"
#include <SCTransSystem.h>

int main(){


    //PA created for on the fly testing
    //std::string pa = "myPA_now";
    std::string pa = "pa1 ";
    faudes::Generator gen = SCTransSystem::MakePAFromDot(pa+".dot");
    //gen.DotWrite(pa+"_out.dot");
    MetaState::generator = gen;


    //AFAs created for on the fly testing
    //std::vector<AFAStatePtr> afaRoots;
    std::vector<std::string> inputs = {"afa1"};
    //std::vector<std::string> inputs = {"check_afa"};
    //std::vector<std::string> inputs = {"afa21", "afa22"};

    for(auto inp : inputs){

        AFAut* afa = AFAut::MakeAFAutFromDot(inp+".dot");
        //afa->DotWrite(inp+"_out.dot");
        MetaState::afaRoots.push_back(afa->mInit);
    }

    std::cout<<"\nOn the fly algo starts"<<std::endl;

    std::string trace = MetaState::getUncoveredTrace(MetaState::generator, MetaState::afaRoots);

    std::cout<<"Done"<<std::endl;
    std::cout<<"Uncovered Trace: "<<trace;

}
