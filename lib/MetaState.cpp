#include "MetaState.h"
#include <queue>
#include <numeric>

void MetaState::testing(){

    std::cout<<"Hi I am from MetaState"<<std::endl;
}

faudes::Generator MetaState::generator;
std::vector<AFAStatePtr> MetaState::afaRoots;


std::string MetaState::getUncoveredTrace(faudes::Generator &pAutomaton, std::vector<AFAStatePtr> &afaRoots){

    generator = pAutomaton;
        //input product automaton

    faudes::StateSet::Iterator sit;
    sit = generator.InitStatesBegin();
        //initial state of product automaton

    MetaStatePtr m = new MetaState();
        //initial meta state

    m->word="";               //"" word in the initial meta state
    m->Pnode = *sit;         //product automaton state for the initial meta state

    std::set<AFAStatePtr> t;
    for(auto x: afaRoots)
        t.insert(x);        //afa roots for initial meta state

    m->cnfList.insert(t);
        //all afa roots are in disjunction with each other

    std::queue<MetaStatePtr> todo;		//to-do queue
    std::set<MetaStatePtr> seen;		//seen set

        //initial meta state inserted in queue
    todo.push(m);
    seen.insert(m);

    while(!todo.empty()){

        MetaStatePtr curr_meta = todo.front();
        todo.pop();
            //meta state popped from queue

        faudes::TransSet::Iterator tit;
        faudes::TransSet::Iterator tit_end;
            //for iterating over the transitions of product automaton

        tit = generator.TransRelBegin(curr_meta->Pnode);
        tit_end = generator.TransRelEnd(curr_meta->Pnode);

        if(tit!=tit_end){
                //if PA has at least one transition

           std::vector<MetaStatePtr> children = getChildren(curr_meta);
                //getChildren() will return the next meta states created from current meta state

            for(auto child: children){

                if(child->cnfList.empty()){
                        //if no AFA is able to simulate PA transition
                        //get accepting word of PA continuing from current PA state
                        //accepting trace is our uncovered trace, return it

                    std::string trace = getAcceptingWord(child->Pnode, child->word);
                    return trace;
                }

                //if AFAs are able to simulate then continue the process
                if(seen.find(child)== seen.end()){
                        //add next meta state in queue only if it is not added earlier

                    todo.push(child);
                    seen.insert(child);
                }
            }
        }
    }
}



std::vector<MetaStatePtr> MetaState::getChildren(MetaStatePtr curr_meta) {
        //This is a helper method to getUncoveredTrace() method. It will take one meta state as
        //input and will return the successors meta states generated from input meta state

    std::vector<MetaStatePtr> successors;
        //This will store the successor meta states

    faudes::TransSet::Iterator tit;
    faudes::TransSet::Iterator tit_end;
        //to iterate over the transitions of product automaton state present in input meta state

    tit = generator.TransRelBegin(curr_meta->Pnode);
    tit_end = generator.TransRelEnd(curr_meta->Pnode);


    //for each transition of PA, we will get one new meta state
    //we will pick each transition of PA one by one and will create new meta states
    for (; tit != tit_end; ++tit) {

        faudes::Idx nxt = tit->X2;
            //destination state on transition

        std::string s = generator.EventName(tit->Ev);
            //symbol on transition

        MetaStatePtr m = new MetaState();
            //create new meta state
            //with above destination state as PA state in new meta state
            //with word seen so far followed by above transition symbol as word of new meta state

        m->word = curr_meta->word + s;
        m->Pnode = nxt;

            //create cnflist for new meta state from cnflist of input meta state simulating PA transition
            //cnf_list: [{1,2,3},{5,6,7}]
            // { }: in OR
            // [ ]: in AND

        //NOTE: We have current cnflist as AND of ORs i.e. [{ }, { }]. So, to create new cnflist
        //we will pick one disjunction term at a time and generate appropriate cnf form from it
        //We will add this cnf form into new state cnflist and will repeat same procedure for all disjunction terms
        //We will append results of each disjunction term one by one
        //and finally combined cnflist will also be in CNF form.

        for (auto x: curr_meta->cnfList) {

            //x is a disjunction term
            //x: {1,2,3}

            std::vector<std::set<AFAStatePtr, mapstatecomparator>> temp;
                //temp is intermediate form to store AND/OR behaviour from state
                //inner set of temp represents conjunction term
                //outer vector of temp represents disjunction term

            for (auto y: x) {
                // y is one AFAStatePtr present in disjunction term x
                // y: 1
                // now add next AFA states from state y in temp(maintaining AND/OR relation )

                auto itr = y->mTransitions.equal_range(s);
                    //looking for transitions from AFAState 'y' on symbol 's'
                    //it returns pair of iterators

                for (auto it = itr.first; it != itr.second; it++) {
                    //add all map entries on 's' in temp
                    //within 1 entry: AND behaviour with states
                    //between multiple entries: OR behaviour with entries

                    temp.push_back(it->second);
                }

                    //Convert the AND/OR relation of AFA states into CNF form
                    //make_CNF() method will do this for us
                std::set<std::set<AFAStatePtr>> cnf = make_CNF(temp);

                    //Add the obtained cnf form in the cnflist of new meta state
                for (auto c: cnf) {

                    if(c.size()!=0)
                        m->cnfList.insert(c);
                }
            }
        }

        //we get one new meta state for one transition of PA and added in successors
        //we will create new meta states for all transitions of PA and will add in successors
        successors.push_back(m);
    }

    return successors;
}



// This getAcceptingWord() method is a helper method to getUncoveredTrace() method
// This method is taken from already implemented GetWord() method and few small changes are
// made as per our need in getUncoveredTrace() method
// Unlike to GetWord() method, it will not return accepting word from PA but it will return
// accepting word starting from a particular state of PA
std::string MetaState::getAcceptingWord(faudes::Idx node, std::string word){

    //std::cout<<"In Accepting Method: "<<word<<std::endl;

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
    //list has structure of OR of ANDs


    std::set<std::set<AFAStatePtr>> cnf = {{}};
            //This will store the cnf form
            //inner set represents disjunction and outer represents conjunction


    // just do cartesian product of each state in AND term with already computed CNF list

    for(auto p: list){
            //p: conjunction term

        std::set<std::set<AFAStatePtr>> temp;

        // p: [5,6]
        // cnf: [{1,2}, {3,4}]

        //only AND case
        for(auto t: cnf){
                //t is a disjunction term

            for(auto q: p){

                std::set<AFAStatePtr> x = t;
                x.insert(q);

                temp.insert(x);	//x is a disjunction term
            }
        }
        // temp = cnf 'OR' p
        // temp = [{1,2,5},{1,2,6},{3,4,5},{3,4,6}]

        cnf = temp;
    }

    return cnf;
}

std::string MetaState::Verify(faudes::Generator generator) {
// initialize todo queue. It is a queue of pairs of state and the string reached so far.
    std::queue<std::pair<faudes::Idx,std::string>> todo;
    std::set<faudes::Idx> seenSet;
    faudes::StateSet::Iterator sit;
    for(sit = generator.InitStatesBegin(); sit != generator.InitStatesEnd(); ++sit) {
        std::pair<faudes::Idx,std::string> tmp;
        tmp.first=*sit;
        tmp.second= "";
        FD_DG("Pushing initial "<<StateName(tmp.first)<<"\n");
        todo.push(tmp);
    }
    // loop variables
    faudes::TransSet::Iterator tit;
    faudes::TransSet::Iterator tit_end;
    // loop
    while(!todo.empty()) {
        // pop
        std::pair<faudes::Idx, std::string> IdxStr = todo.front();
        faudes::Idx x1 = IdxStr.first;
        //Put x1 in the seen set.
        seenSet.insert(x1);
        std::string syms = IdxStr.second;
        //if we have reached any marked state then we are done. Just return syms string.
        faudes::StateSet::Iterator lit;
        for(lit = generator.MarkedStatesBegin(); lit != generator.MarkedStatesEnd(); ++lit) {
            FD_DG("Checking if "<<StateName(x1)<<" is accepting or not, checking against "<<StateName(*lit)<<"\n");
            if (*lit == x1) {
                FD_DG(" Found accepting state, returning with "<<syms<<"\n");
                return syms;
            }
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

            FD_DG(" Checking transition from "<<StateName(x1)<<" to "<<StateName(x2)<<" on "<<EventName(tit->Ev)<<"\n");
            //if the destination state is not present in the seen set then add the pair (destination state, sym.symbol on this transition) to the todo queue
            if (seenSet.find(x2) == seenSet.end()) {
                std::pair<faudes::Idx,std::string> tmp;
                tmp.first=x2;
                tmp.second=syms+symtrans;
                FD_DG(" Pushing next state "<<StateName(x2)<<" with trace as "<<tmp.second<<" at the end of the todo queue\n");
                //Add this pair to the end of the todo queue.
                todo.push(tmp);
            }
        }

    }
    //if reached here without return in between then no word is accepted here. So return empty word.
    return "None";
}

