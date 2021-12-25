#ifndef LLVM_VERIFY_PASS_METASTATE_H
#define LLVM_VERIFY_PASS_METASTATE_H

#include <libfaudes.h>
#include "AFA/AFAState.h"


class MetaState;
typedef MetaState* MetaStatePtr;

class MetaState {
public:

    static faudes::Generator generator;
        //product automaton for our algorithm

    static std::vector<AFAStatePtr> afaRoots;
        //set of afa roots for our algorihtm

    std::string word;
    faudes::Idx Pnode;
    std::set<std::set<AFAStatePtr>> cnfList;
        //inner set makes into disjunction and outer set makes into conjunction
        //so overall CNF form

    MetaState(){}


    static std::string getUncoveredTrace(faudes::Generator &pAutomaton, std::vector<AFAStatePtr> &afaRoots);

    static std::vector<MetaStatePtr> getChildren(MetaStatePtr curr_meta);

    static std::string getAcceptingWord(faudes::Idx node, std::string word);

    static std::set<std::set<AFAStatePtr>> make_CNF(std::vector<std::set<AFAStatePtr,mapstatecomparator>> temp);

    static void testing();

    static std::string Verify(faudes::Generator generator);

};

struct metaState_comparator {
    bool operator()(const MetaStatePtr a, const MetaStatePtr b) const {

        return (a->Pnode < b->Pnode) || (a->Pnode == b->Pnode && a->cnfList < b->cnfList);
    }
};



#endif //LLVM_VERIFY_PASS_METASTATE_H
