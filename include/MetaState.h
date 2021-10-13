#ifndef LLVM_VERIFY_PASS_METASTATE_H
#define LLVM_VERIFY_PASS_METASTATE_H

#include <libfaudes.h>
#include "AFA/AFAState.h"


class MetaState;
typedef MetaState* MetaStatePtr;

class MetaState {
public:

    static faudes::Generator generator;

    std::string word;
    faudes::Idx Pnode;
    std::set<std::set<AFAStatePtr>> cnfList;

    MetaState(){}


    static std::string getUncoveredTrace(faudes::Generator &pAutomaton, std::vector<AFAStatePtr> &afaRoots);

    static std::vector<MetaStatePtr> getChildren(MetaStatePtr curr_meta);

    static std::string getAcceptingWord(faudes::Idx node, std::string word);

    static std::set<std::set<AFAStatePtr>> make_CNF(std::vector<std::set<AFAStatePtr,mapstatecomparator>> temp);

    static void testing();

};

#endif //LLVM_VERIFY_PASS_METASTATE_H
