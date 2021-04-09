/*
 * SCTransSystem.cpp
 *
 *  Created on: 10-Jun-2015
 *      Author: jaganmohini
 */

#include "SCTransSystem.h"
#include <boost/foreach.hpp>
#include <boost/assert.hpp>
#include <queue>

extern "C" {
#include <stdio.h>
}

#define DBGPRNT

typedef std::tuple<struct autstate* ,std::tuple<std::set<struct autstate*,newsetofstatescomparator>,bool>> newstateinfo;

struct newstateinfocompare{
	bool operator() (const newstateinfo* one, const newstateinfo* two) const
		{
		bool first = std::get<1>(std::get<1>(*one));
		bool second = std::get<1>(std::get<1>(*two));
		std::set<struct autstate*,newsetofstatescomparator> firstset = std::get<0>(std::get<1>(*one));
		std::set<struct autstate*,newsetofstatescomparator> secondset = std::get<0>(std::get<1>(*two));
		if(first!=second)
			return first<second;
		else
			return firstset<secondset;
//			if(std::get<1>(std::get<1>(*one))!=std::get<1>(std::get<1>(*two)))
//			return (std::get<1>(*one))<(std::get<1>(*two));
		}
};


 SCTransSystem::SCTransSystem(Program& P, z3::solver& s):mProgram(P), mSolver(s) {
}

 /*
  * Unused function: Delete it later
  */
/*std::tuple<std::string,z3::expr> SCTransSystem::GetAcceptedWordWithEndState()
{
	struct autstate* accstate=NULL;
	char* word;
	size_t length;
	fa_example_withendstate(mMerged,&word,&length,&accstate);
	std::string accword(word,length);
	std::cout<<"word is "<<accword<<" with length = "<< length<<std::endl;
	BOOST_ASSERT_MSG(mShuffautAssnMap.find(accstate)!=mShuffautAssnMap.end(),"SHould not be possible as this accepted state must have been set properly, check out");
	z3::expr ex = mShuffautAssnMap.find(accstate)->second;
	return (std::tuple<std::string,z3::expr>(accword,ex));
}*/

z3::expr SCTransSystem::GetEndStateAssertionFromWord(std::string afaword)
{
	size_t length=afaword.length();
	std::string lastsym(afaword.substr(length-1,1));
	std::map<std::string,z3::expr> assnMap = mProgram.GetAssnMapForAllProcesses();
	BOOST_ASSERT_MSG(assnMap.find(lastsym)!=assnMap.end(),"Some serious issue as the last char must be the one where assn is defined");
	return assnMap.find(lastsym)->second;

}


template <typename T>
using AdjacencyList = std::vector<std::vector<std::pair<int, T> > >;



void SCTransSystem::CreateFAutomataFaudes(const AdjacencyList<int>& adj, faudes::Generator& generator) {
    // Some assertions
    assert(adj.size() >= 4 &&
           adj[0].size() == 1 &&
           adj[0][0] == std::make_pair(3, -1));

    for (unsigned source = 0; source < adj.size(); source++) {
        //Find all connected vertices from this vertex and iterate over them.

        std::string srcstr = std::to_string(source);
        generator.InsState(srcstr);

        if(source==2) //This is an expectation that the third entry will correspond to an accepting state.
            generator.SetMarkedState(srcstr);
        if(source==3) //This is an expectation that the fourth entry will correspond to initial state.
            generator.SetInitState(srcstr);
    }
    for (unsigned source = 0; source < adj.size(); source++) {
        for (std::pair<int, int> edge : adj[source]) {
            std::string srcstr,dststr;
            unsigned destination = edge.first;
            unsigned symbolval = edge.second;
            srcstr = std::to_string(source);
            dststr = std::to_string(destination);
            char sym = 'A';
            if (symbolval < 26) sym = 'A' + symbolval;
            else sym = 'a' + (symbolval - 26);
            std::string symstr(1,sym);
            generator.InsEvent(symstr);
            //States corresponding to source and destination have already been added in the generator by previous loop.
            // so just insert an edge between those states.
            generator.SetTransition(srcstr, symstr, dststr);
        }
    }
}

void SCTransSystem::BuildSCTS(faudes::Generator& lGenerator){
  std::map<std::string,z3::expr> assnMap = mProgram.GetAssnMapForAllProcesses();
  std::vector<AdjacencyList<int> > adjlists = mProgram.GetAutomata();

  std::vector<faudes::Generator> faudesAutomata;

    for (unsigned i = 0; i < adjlists.size(); i++) {
        faudes::Generator faudesAut;
        CreateFAutomataFaudes(adjlists[i], faudesAut);
        //Assert that the constructed automaton should not be empty.
        //push the returned automaton to the vector. If needed clone the generator as well.
        faudesAutomata.push_back(faudesAut);

        //To test, print a word accepted by this generator.
        std::cout << "An example accepted word is " <<std::endl;
        std::cout << GetWord(faudesAut)<<std::endl;
        faudesAut.DotWrite("./auto"+std::to_string(i+1)+".dot");
    //BOOST_ASSERT_MSG(aut != NULL, "could not construct automaton");

  }
    BOOST_ASSERT_MSG(faudesAutomata.size()>0, " No process given as input.. Exiting");
    faudes::Generator result = faudesAutomata.at(0);
    for(unsigned i=1; i<adjlists.size(); i++) {
        faudes::Generator tmpresult;
        faudes::Parallel(result,faudesAutomata.at(i),tmpresult);
        result = tmpresult;
    }
    /*
     * IMP: Clear all marked states. Then set only those states as marked states which are one-step reachable
     * by a transition on a symbol that is prsent in assnMap variable.
     */
    result.ClearMarkedStates();
    //Iterate over all transitions.
    //Iterate over all relations of original and insert them in reverse in rev generator.
    faudes::TransSet::Iterator transit;
    for(transit = result.TransRelBegin(); transit != result.TransRelEnd(); ++transit) {
        std::string sym = result.EventName(transit->Ev);
        if(assnMap.find(sym)!=assnMap.end()){
            //then set transit->X2 as marked state.
            result.SetMarkedState(transit->X2);
        }
    }


    //To test, print a word accepted by this generator.
    std::cout << "An example accepted word of the parallely composed generator is "<<std::endl;
    std::cout << GetWord(result)<<std::endl;
    result.DotWrite("./autoParallel.dot");
    faudes::Generator tmpGen;

    faudes::Deterministic(result,tmpGen);
    ReverseGenerator(tmpGen,lGenerator);
    //lGenerator=tmpGen;
    //To test, print a word accepted by this reversed generator.
    std::cout << "An example accepted word of the reversed parallely composed generator is "<<std::endl;
    std::cout << GetWord(lGenerator)<<std::endl;
    lGenerator.DotWrite("./autoParallelReversed.dot");


}

/**
* This method does a breadth first search of the input automaton to find the shortest accepting word starting from the
* initial state.
*/
std::string SCTransSystem::GetWord(faudes::Generator generator){
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
/*
 * This method takes a faudes generator and return another generate which accepts the reverse language of the original one.
 */
void SCTransSystem::ReverseGenerator(faudes::Generator& original, faudes::Generator& rev){
    //To create a reverse automaton we
    //1. Reverse all transitions
    //2. Switch initial and marked states.

    //Iterate over all states of original and insert them in rev generator
    faudes::StateSet::Iterator lit;
    for(lit = original.StatesBegin(); lit != original.StatesEnd(); ++lit) {
        rev.InsState(*lit);
    }
    //Switch initial and marked states.
    for(lit = original.InitStatesBegin(); lit != original.InitStatesEnd(); ++lit) {
        rev.SetMarkedState(*lit);
    }
    for(lit = original.MarkedStatesBegin(); lit != original.MarkedStatesEnd(); ++lit) {
        rev.SetInitState(*lit);
    }
    //Iterate over all relations of original and insert them in reverse in rev generator.
    faudes::TransSet::Iterator transit;
    for(transit = original.TransRelBegin(); transit != original.TransRelEnd(); ++transit) {
        rev.InsEvent(transit->Ev);
        rev.SetTransition(transit->X2,transit->Ev,transit->X1);
    }
    //Done with filling generator rev.
}
/*
struct fa* SCTransSystem::FA_Merge(std::vector<struct fa*>& autset,std::map<std::string,z3::expr>& assnMap, faudes::Generator& lGenerator)
{
	std::set<struct autstate*, newsetofstatescomparator> initset;
	std::map<struct autstate*, z3::expr> stateAssnMap;


	BOOST_FOREACH(auto elem, autset){
		initset.insert(elem->initial);
	}
//add all symbols to generator

	BOOST_FOREACH(auto t, mProgram.mAllSyms){
		//add events
		lGenerator.InsEvent(t);
	}
	//std::cout<<"Size of initset is "<<initset.size()<<std::endl;
	std::map<newstateinfo*, newstateinfo*, newstateinfocompare> seenmap;
		struct fa* merged = fa_make_empty();
		//create a new state, init
		struct autstate* init = add_autstate(merged,0);//add a non-accepting state to this automaton
		std::stringstream st;
		st<<init;
		lGenerator.InsState(st.str());
//		lGenerator.SetInitState(st.str());
    lGenerator.SetMarkedState(st.str());

		//set init as initial state of merged
		merged->initial=init;
		//now for each character in mProgram.mInitString create sequential transitions from init..by adding new states.
		//and transitions .. the last state will be added to newstatinfo object create later..
		struct autstate* last = init;
		std::string laststr(st.str());
		st.str("");
		for(std::string::iterator it = mProgram.mInitString.begin();it!=mProgram.mInitString.end();it++)
		{
			char sym = *it;
#ifdef DBGPRNT
      std::cout << sym << std::endl;
#endif
			std::string symstr(1,sym);
			struct autstate* curr = add_autstate(merged,0);
			st<<curr;
			lGenerator.InsState(st.str());
			add_new_auttrans(last,curr,sym,sym);
			lGenerator.SetTransition(st.str(),symstr,laststr);
			last=curr;
			laststr=std::string(st.str());
			st.str("");
		}
		//map last to initset
		newstateinfo* initinfo = new newstateinfo;
		std::get<0>(*initinfo) = last;
		std::get<0>(std::get<1>(*initinfo)) = initset;
		std::get<1>(std::get<1>(*initinfo)) = false;
		//initset must be non-accepting (some weak link here) so set bool as false.
		seenmap[initinfo]=initinfo;
		//put init to a worklist, arraylist of struct autstate*
				std::list<newstateinfo*> worklist;
				worklist.push_back(initinfo);
				while(worklist.size()!=0)
				{
					newstateinfo* picked = worklist.front();
					worklist.pop_front();
					//get state of shuffled automaton.
					struct autstate* shuffstate = std::get<0>(*picked);
					st.str("");
					st<<shuffstate;
					std::string currstring(st.str());
					st.str("");
					std::set<struct autstate*, newsetofstatescomparator> productset = std::get<0>(std::get<1>(*picked));
					//no need to extract bool here..
					BOOST_FOREACH(struct autstate* procstate, productset)
					{
						for(int i=0; i< procstate->tused; i++)
						{
							struct auttrans trans = procstate->trans[i];
							//find the destination state
							struct autstate* dest = trans.to;
							//construct new reachable product states set
							std::set<struct autstate*, newsetofstatescomparator> newproductset = productset;
							newproductset.erase(procstate);
							newproductset.insert(dest);
							//check if the new product set is in seenmap
							newstateinfo* newstepstateinfo = new newstateinfo;
							//before that check if this transition was assertion causing one and hence this state becomes accepting state as well.
							std::string searched(1,trans.min);
							bool isacc=false;
							if(assnMap.find(searched)!=assnMap.end()){
								//means set this as accepting
								//std::cout<<"Found an accepting state\n";
								isacc=true;
								std::get<1>(std::get<1>(*newstepstateinfo))=true;
							}else
								std::get<1>(std::get<1>(*newstepstateinfo))=false;

							std::get<0>(std::get<1>(*newstepstateinfo))=newproductset;
							struct autstate* newshuffledstate = NULL;
							if(seenmap.find(newstepstateinfo)!=seenmap.end()){
								//means the key was found..and accept/nonaccept was also matched
								//std::cout<<"No new element is added "<<std::endl;

								//get the associated shuffled state.
								newstateinfo* tempnewstepstateinfo = seenmap[newstepstateinfo];
								delete newstepstateinfo;//because now this if of no use..
								newstepstateinfo = tempnewstepstateinfo;
								newshuffledstate = std::get<0>(*newstepstateinfo);
								if(assnMap.find(searched)!=assnMap.end()){
									BOOST_ASSERT_MSG(newshuffledstate->accept==1,"This must have been set earlier otherwise serious error");
								}
							}else
							{
								//std::cout<<"New element is added "<<std::endl;
								//else newstepstateinfo was correctly allocated..
								//create a new state in shuffled automaton
								newshuffledstate = add_autstate(merged,0);
								st.str("");
								st<<newshuffledstate;
								std::string  deststring(st.str());
								st.str("");
								lGenerator.InsState(deststring);
								if(assnMap.find(searched)!=assnMap.end()){//check if the transition caused by this symbol by this procstate causes it to reach to assertion point
									//means key found
									z3::expr ex = assnMap.find(searched)->second;
									newshuffledstate->accept=1;
									//mShuffautAssnMap.insert(std::make_pair(newshuffledstate,ex));
								}else
									newshuffledstate->accept=0;
								std::get<0>(*newstepstateinfo) = newshuffledstate;
								//insert it to seenmap as well as in worklist
								seenmap[newstepstateinfo]=newstepstateinfo;
								worklist.push_back(newstepstateinfo);
							}
							st.str("");
							st<<newshuffledstate;
							std::string  deststring(st.str());
							st.str("");
							if(isacc)
								lGenerator.SetInitState(deststring);//because we are creating reversed generator..
							//add a transition from shuffstate to this shuffled state with label extracted from this transition
							add_new_auttrans(shuffstate, newshuffledstate,trans.min,trans.max);
							lGenerator.SetTransition(deststring,searched,currstring);
							std::string tmpw = lGenerator.GetWord();
							//std::cout<<"Word is "<<tmpw<<"\n";
						}
					}
				}
				//after we are done.. call delete on every key element of the map seenmap
				//no need to delete value as key is same as value.
				BOOST_FOREACH(auto d, seenmap)
				{
					delete d.first;
				}
#ifdef DBGPRNT
        // struct fa* testfa = nullptr;
        // std::string testregex = "0123ABT";
        // fa_compile(testregex.c_str(), testregex.size(), &testfa);
        // testfa = fa_intersect(testfa, merged);
        // char* regi = "";
        // size_t regi_len = 0;
        // std::cout << "Accepts Test Regex: " << fa_as_regexp(testfa, &regi, &regi_len);
        // std::cout << " " << regi << std::endl;
				FILE *fp;
				fp = fopen("./originalMerged.dot", "w");
				BOOST_ASSERT_MSG(fp!=NULL,"Can't open input file in.list!\n");

				//no need to determinize here as by construction it is determinzed;
				fa_dot(fp,merged);
				fclose(fp);
#endif

	return merged;
}
*/


/**
 * Commenting out this method as it is never used.
 * @param productset
 * @param stateAssnMap
 * @return
 */
/*std::tuple<bool,z3::expr> SCTransSystem::GetAcceptAssn(std::set<struct autstate*, newsetofstatescomparator>&  productset, std::map<struct autstate*, z3::expr>& stateAssnMap)
{
	bool found=false;
	z3::context& ctx = mSolver.ctx();
	z3::expr trueexp = ctx.bool_val(true);
	BOOST_FOREACH(struct autstate* st, productset)
		{
			if(stateAssnMap.find(st)!=stateAssnMap.end())
			{
				found=true;
				z3::expr ex = stateAssnMap.find(st)->second;
				trueexp = trueexp && ex;
			}
		}
	if(found==false)
		return std::tuple<bool,z3::expr>(false,trueexp);
	else
		return std::tuple<bool,z3::expr>(true,trueexp.simplify());
}*/

SCTransSystem::~SCTransSystem() {


}
