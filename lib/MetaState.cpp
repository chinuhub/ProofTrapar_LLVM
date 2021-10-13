#include "MetaState.h"
#include <queue>
#include <numeric>

void MetaState::testing(){

    std::cout<<"Hi I am from MetaState"<<std::endl;
}

faudes::Generator MetaState::generator;


std::string MetaState::getUncoveredTrace(faudes::Generator &pAutomaton, std::vector<AFAStatePtr> &afaRoots){

    generator = pAutomaton;

    faudes::StateSet::Iterator sit;
    sit = generator.InitStatesBegin();

    MetaStatePtr m = new MetaState();
        //initial meta state

    m->word="";
    m->Pnode = *sit;         //initial state of product automaton

    std::set<AFAStatePtr> t;
    for(auto x: afaRoots)
        t.insert(x);

    m->cnfList.insert(t);
        //all afa roots are in disjunction

    std::queue<MetaStatePtr> todo;		//to-do queue
    std::set<MetaStatePtr> seen;		//seen set

    todo.push(m);
    seen.insert(m);

    while(!todo.empty()){

        MetaStatePtr curr_meta = todo.front();
        todo.pop();

        faudes::TransSet::Iterator tit;
        faudes::TransSet::Iterator tit_end;

        tit = generator.TransRelBegin(curr_meta->Pnode);
        tit_end = generator.TransRelEnd(curr_meta->Pnode);

        if(tit!=tit_end){

           std::vector<MetaStatePtr> children = getChildren(curr_meta);

            for(auto child: children){

                if(child->cnfList.empty()){

                    std::string trace = getAcceptingWord(child->Pnode, child->word);
                    return trace;

                }

                if(seen.find(child)== seen.end()){

                    todo.push(child);
                    seen.insert(child);
                }

            }

        }

    }

}



std::vector<MetaStatePtr> MetaState::getChildren(MetaStatePtr curr_meta) {

    std::vector<MetaStatePtr> successors;

    faudes::TransSet::Iterator tit;
    faudes::TransSet::Iterator tit_end;

    tit = generator.TransRelBegin(curr_meta->Pnode);
    tit_end = generator.TransRelEnd(curr_meta->Pnode);

    for (; tit != tit_end; ++tit) {

        faudes::Idx nxt = tit->X2;

        std::string s = generator.EventName(tit->Ev);

        MetaStatePtr m;

        m->word = curr_meta->word + s;
        m->Pnode = nxt;

        for (auto x: curr_meta->cnfList) {

            //x is a disjunction term

            std::vector<std::set<AFAStatePtr, mapstatecomparator>> temp;    //temp is intermediate form

            //std::set<AFAStatePtr,mapstatecomparator> temp;

            for (auto y: x) {

                // y is a AFAStatePtr

                auto itr = y->mTransitions.equal_range(s);

                //returns pair of iterators

                for (auto it = itr.first; it != itr.second; it++) {

                    temp.push_back(it->second);

                }

                std::set<std::set<AFAStatePtr>> cnf = make_CNF(temp);

                for (auto c: cnf)
                    m->cnfList.insert(c);

            }
        }

        successors.push_back(m);

    }

    return successors;

}





std::string MetaState::getAcceptingWord(faudes::Idx node, std::string word){


        // todo queue is a queue of pairs of state and the string reached so far.
    std::queue<std::pair<faudes::Idx,std::list<std::string>>> todo;
    std::set<faudes::Idx> seenSet;

    std::pair<faudes::Idx,std::list<std::string>> tmp;
    tmp.first=node;
    tmp.second.push_back(word);
    todo.push(tmp);
    seenSet.insert(node);

    //store marked states for testing later inside while loop.
    faudes::StateSet::Iterator lit;
    std::set<faudes::Idx> markedStatesSet;

    for(lit = generator.MarkedStatesBegin(); lit != generator.MarkedStatesEnd(); ++lit) {
        FD_DG("Checking if "<<StateName(x1)<<" is accepting or not, checking against "<<StateName(*lit)<<"\n");
        markedStatesSet.insert(*lit);
    }

    // loop variables
    faudes::TransSet::Iterator tit;
    faudes::TransSet::Iterator tit_end;

    while(!todo.empty()) {

        // pop
        std::pair<faudes::Idx, std::list<std::string>> IdxStr = todo.front();
        faudes::Idx x1 = IdxStr.first;

        std::list<std::string> symlist = IdxStr.second;

        //if we have reached any marked state then we are done. Just return syms string.
        faudes::StateSet::Iterator lit;

        if(markedStatesSet.find(x1)!=markedStatesSet.end()){
            //means x1 is an accepting state, return syms constructed out of symlist. It is constructed by concatenating the content of the list together.
            //and skipping if it is empty character.
            std::string res =  std::accumulate(
                    symlist.begin(),
                    symlist.end(),
                    std::string{});

            return res;
        }

        //If we reached here then this is not the marked state. So continue exploring further.
        todo.pop();
        tit = generator.TransRelBegin(x1);
        tit_end = generator.TransRelEnd(x1);

        //Iterate over all transitions of x1..
        for (; tit != tit_end; ++tit) {

            //get the destination state
            faudes::Idx x2 = tit->X2;
            //get the symbol on the transition
            std::string symtrans = generator.EventName(tit->Ev);

            //if the destination state is not present in the seen set then add the pair (destination state, sym.symbol on this transition) to the todo queue
            if (seenSet.find(x2) == seenSet.end()) {
                std::pair<faudes::Idx,std::list<std::string>> tmp;
                tmp.first=x2;
                tmp.second = symlist;
                tmp.second.push_back(symtrans);

                //Put x1 in the seen set.
                seenSet.insert(x2);
                //Add this pair to the end of the todo queue.
                todo.push(tmp);
            }
        }

    }
    //if reached here without return in between then no word is accepted here. So return empty word.
    return "None";

}


std::set<std::set<AFAStatePtr>> MetaState::make_CNF(std::vector<std::set<AFAStatePtr,mapstatecomparator>> list){

    std::set<std::set<AFAStatePtr>> cnf = {{}};

    for(auto p: list){

        std::set<std::set<AFAStatePtr>> temp;

        //only AND case
        for(auto t: cnf){		//t is a disjunction term

            for(auto q: p){

                std::set<AFAStatePtr> x = t;
                x.insert(q);

                temp.insert(x);	//x is a disjunction term
            }
        }

        cnf = temp;
    }

    return cnf;
}
