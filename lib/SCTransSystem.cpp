/*
 * SCTransSystem.cpp
 *
 *  Created on: 10-Jun-2015
 *      Author: jaganmohini
 */

#include "SCTransSystem.h"
#include <boost/foreach.hpp>
#include <boost/assert.hpp>
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
std::tuple<std::string,z3::expr> SCTransSystem::GetAcceptedWordWithEndState()
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
}

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

struct fa* CreateFAutomata(const AdjacencyList<int>& adj) {
  // Some assertions
  assert(adj.size() >= 4 &&
         adj[0].size() == 1 &&
         adj[0][0] == std::make_pair(3, -1));
  struct fa* ans = fa_make_empty();
  std::map<unsigned, struct autstate*> state_map;
  for (unsigned i = 0; i < adj.size(); i++) {
    struct autstate* temp = add_autstate(ans, (i == 2 ? 1 : 0));
    state_map.insert(
      std::make_pair(
        i,
        temp
      )
    );
    if (i == 3) {
      ans->initial = temp;
    }
  }
  for (unsigned i = 1; i < adj.size(); i++) {
    auto from_it = state_map.find(i);
    assert(from_it != state_map.end());
    struct autstate* from = from_it->second;
    for (std::pair<int, int> edge : adj[i]) {
      auto to_it = state_map.find(edge.first);
      assert(to_it != state_map.end());
      struct autstate* to = to_it->second;
      char sym = 'A';
      if (edge.second < 26) sym = 'A' + edge.second;
      else sym = 'a' + (edge.second - 26);
      add_new_auttrans(from, to, sym, sym);
    }
  }
  return ans;
}

struct fa* SCTransSystem::BuildSCTS(faudes::Generator& lGenerator){
  struct fa* aut = NULL;
  std::map<std::string,z3::expr> assnMap = mProgram.GetAssnMapForAllProcesses();
  std::vector<AdjacencyList<int> > adjlists = mProgram.GetAutomata();
  std::vector<struct fa*> automata;
  for (unsigned i = 0; i < adjlists.size(); i++) {
    aut = CreateFAutomata(adjlists[i]);
    BOOST_ASSERT_MSG(aut != NULL, "could not construct automaton");
    automata.push_back(aut);
#ifdef DBGPRNT
    char* regi = "";
    size_t regi_len = 0;
    std::cout << "Accepts Regex: " << fa_as_regexp(aut, &regi, &regi_len);
    std::cout << " " << regi << std::endl;
    char* example = "";
    size_t example_len = 0;
    std::cout << "An example accepted word is ";
    std::cout << fa_example(aut, &example, &example_len) << " " << example;
    std::cout << std::endl;
#endif
  }
  mMerged = FA_Merge(automata, assnMap, lGenerator);
  struct fa* forreverse = fa_clone(mMerged);
  return forreverse;
}

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
									mShuffautAssnMap.insert(std::make_pair(newshuffledstate,ex));
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
				//minimize automaton and then return it.
				FILE *fp;
				fp = fopen("./originalMerged.dot", "w");
				BOOST_ASSERT_MSG(fp!=NULL,"Can't open input file in.list!\n");

				//no need to determinize here as by construction it is determinzed;
				fa_dot(fp,merged);
				fclose(fp);
#endif

	return merged;
}


std::tuple<bool,z3::expr> SCTransSystem::GetAcceptAssn(std::set<struct autstate*, newsetofstatescomparator>&  productset, std::map<struct autstate*, z3::expr>& stateAssnMap)
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
}

SCTransSystem::~SCTransSystem() {


}
