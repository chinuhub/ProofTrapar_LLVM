/*
 * PassFourAFA.cpp

 *
 *  Created on: 23-Sep-2015
 *      Author: jaganmohini
 */

#include "AFA/AFAut.h"
#include "MetaState.h"


SetAFAStatesPtr flatten_set(std::set<SetAFAStatesPtr> setOfSets){
    SetAFAStatesPtr setStates;
    BOOST_FOREACH(auto stState, setOfSets){
        BOOST_FOREACH(auto state, stState){
            setStates.insert(state);
        }
    }
    return setStates;
}
/**
 * This function traverses the autoamaton starting from state (input) and returns a graph/AFA that does not contain epsilon starting from input node.
 * It returns the set of starting states of the modified AFA (because the graph can be broken into set of disjoint graphs as well due to epsilon removal
 */
std::pair<StateType,std::set<SetAFAStatesPtr>> AFAut::EpsilonClosure(AFAStatePtr state) {
    //Base case: If state is accepting state then return state.
    std::set<SetAFAStatesPtr> setResultSet;
    bool thisNotAddedYet=true;
    std::multimap<std::string,SetAFAStatesPtr> tmpTransRel;
    if (state->mIsAccepted) {
        SetAFAStatesPtr stmp;
        stmp.insert(state);
        setResultSet.insert(stmp);
        return std::make_pair(state->mType, setResultSet);
    } else {
        //Else for every sym, stateset in transition of state.
        BOOST_FOREACH(auto t, state->mTransitions) {
                //If the transition is on 0 symbol
                if(t.first.compare("0")==0) {
                    //Iterate over all next state of this transition, get their SetSetAFAStatesPtr, flatten them, add to a single SetAFAStatesPtr
                    //At the end of iteration put this SetAFAStatesPtr in the setResultSet.
                    SetAFAStatesPtr resset;
                    BOOST_FOREACH(auto st, t.second) {
                        std::pair<StateType,std::set<SetAFAStatesPtr>> tmp = EpsilonClosure(st);
                        //If t.second is singleton then it means there is only one entry for this symbol and that is OR.
                        //Therefore it is fine even if tmp.second returns more than one set of states [e.g. {{a,b},{c,d,e}} (individual ones are in AND and outside onese are in OR)
                        //In this case we will simply create entries for 0-> {a,b} and 0->{c,d,e}
                        //If that is not the case (i.e. t.second is not singleton) then tmp.second should be singleton [e.g. {{a,b}}]
                        // It can have multiple states inside it though. In that case we create 0->{a,b}
                        if(t.second.size()==1){
                            BOOST_FOREACH(auto setst, tmp.second){
                                setResultSet.insert(setst);
                            }
                        }else{
                            BOOST_ASSERT_MSG(tmp.second.size()==1,"ERR: For now we don't allow OR's inside AND.!!");
                            SetAFAStatesPtr  tmps = flatten_set(tmp.second);
                            resset.insert(tmps.begin(),tmps.end());
                        }

                    }
                    //Once we are done add tmps to setResultSet.
                    if(resset.size()>0)
                        setResultSet.insert(resset);

                }
                else {//If the transition is on non-zero symbol
                    //Iterate over all next state of this transition (there can be more than one as well),
                    // get their SetSetAFAStatesPtr.
                    //For every single state in SetSetAFAStatesPtr add (sym, that single SetAFAStatesPtr) in a temporary transition map
                    //At the end of the iteration put this state in SetAFAStatesPtr in the setResultSet.(if not already present)
                    std::string sym(t.first);
                    SetAFAStatesPtr resset;
                    BOOST_FOREACH(auto st, t.second) {
                                    std::pair<StateType,std::set<SetAFAStatesPtr>> tmp = EpsilonClosure(st);
                                    //If t.second is singleton then it means there is only one entry for this symbol and that is OR.
                                    //Therefore it is fine even if tmp.second returns more than one set of states [e.g. {{a,b},{c,d,e}} (individual ones are in AND and outside onese are in OR)
                                    //In this case we will simply create entries for 0-> {a,b} and 0->{c,d,e}
                                    //If that is not the case (i.e. t.second is not singleton) then tmp.second should be singleton [e.g. {{a,b}}]
                                    // It can have multiple states inside it though. In that case we create 0->{a,b}
                                    if(t.second.size()==1){
                                        BOOST_FOREACH(auto setst, tmp.second){
                                                        //add (sym,setst) to tmptransrelation.
                                                        tmpTransRel.insert(std::make_pair(sym,setst));
                                                    }
                                    }else{
                                        BOOST_ASSERT_MSG(tmp.second.size()==1,"ERR: For now we don't allow OR's inside AND.!!");
                                        SetAFAStatesPtr  tmps = flatten_set(tmp.second);
                                        resset.insert(tmps.begin(),tmps.end());
                                    }

                                }
                    //Once we are done add (sym,resset) to tmptransrelation.
                    if(resset.size()>0)
                        tmpTransRel.insert(std::make_pair(sym,resset));
                    //Add this state to setResultSet (if not already present)
                    if(thisNotAddedYet) {
                        thisNotAddedYet=false;
                        SetAFAStatesPtr tmpst;
                        tmpst.insert(state);
                        setResultSet.insert(tmpst);
                    }
                }

        }
        //Clear this state's transition map and set temporary transition map as the transition map. (if temporary transition map is not-empty)
        if(tmpTransRel.size()>0) {
            state->mTransitions.clear();
            state->mTransitions.insert(tmpTransRel.begin(), tmpTransRel.end());
        }
        //Return this state's statetype and setResultSet to the caller.
        return std::make_pair(state->mType,setResultSet);
    }
}


/**
 * This function takes the state and look at it's successors if the following three conditions hold.
 * If these conditions hold then the symbol gets converted to epsilon.
 * Nothing gets returned only the AFA gets modified directly.
 * @param state
 */
void AFAut::ConvertToEpsilonConsecutiveSameAMap(AFAStatePtr state) {

    if (state->mIsAccepted)
        return;
    else {
        std::multimap<std::string,SetAFAStatesPtr> tmpTrans;
        //For every successor of state, sym. Check if
        //1. State has one successor (We relax it now to say that it has one successor per symbol).
        //2. AMap of state is same as that of successor
        //3. HMap(A) and HMap(B) is false.
        //If all three conditions are true then change sym to 0. and call this functiona recursively on the successor state.
        //Else for every sym, stateset in transition of state.
        BOOST_FOREACH(auto trans, state->mTransitions) {
                        if (trans.second.size() == 1) {
                            std::string sym(trans.first);
                            AFAStatePtr nextst = *trans.second.begin();
                            //If AMap of this state and the next state are same and the HMap of nextstate is false (Do we need to check about the HMap of this state as well?)
                            if (state->mAMap.hash() == nextst->mAMap.hash() && state->HelperIsUnsat(*(nextst->mHMap))) {
                                tmpTrans.insert(std::make_pair("0",trans.second));
                            }else{
                                tmpTrans.insert(std::make_pair(sym,trans.second));
                            }
                            //Now call this function recursively on nextst.
                            ConvertToEpsilonConsecutiveSameAMap(nextst);
                        } else {
                            //call this function recursively for every next state.
                            BOOST_FOREACH(auto st, trans.second) {
                                            ConvertToEpsilonConsecutiveSameAMap(state);
                                        }
                            tmpTrans.insert(std::make_pair(trans.first,trans.second));
                        }
                    }
        state->mTransitions.clear(); //Don't remove all?? What if
        state->mTransitions.insert(tmpTrans.begin(),tmpTrans.end());
    }

}


void AFAut::CollectSameAMapInfo(AFAStatePtr state, std::map<unsigned int, std::set<AFAStatePtr>> &redundancyMap){

    // redundancyMap contains <hash of AMap(a1), set of states having AMap=a1 and HMap = false> as <key,value> pair

        if(state->HelperIsUnsat(*(state->mHMap))){
                // if HMap is false

            auto it = redundancyMap.find(state->mAMap.hash());

            if(it != redundancyMap.end()){
                //add state to existing entry in redundancyMap
                it->second.insert(state);
            }
            else {
                //create a new entry in redundancyMap
                std::set<AFAStatePtr> temp;
                temp.insert(state);
                redundancyMap.insert(std::make_pair(state->mAMap.hash(), temp));
            }
        }

        //base case
        if(state->mIsAccepted)
            return;

        //recursive call to traverse complete AFA
        BOOST_FOREACH(auto trans, state->mTransitions) {
            BOOST_FOREACH(auto adj, trans.second) {
                            CollectSameAMapInfo(adj, redundancyMap);
            }
        }
}

void AFAut::ConvertToEpsilonAllSameAMap(AFAStatePtr state) {

    //This method adds epsilon edges between all states(including non-consecutive) containing same AMap and HMap as false

    // This method is upgrade to ConvertToEpsilonConsecutiveSameAMap() because it not only add epsilon edges
    // between consecutive states satisfying the criteria(mentioned in ConvertToEpsilonConsecutiveSameAMap())
    // but also add epsilon edges between  non-consecutive states

    std::map<unsigned int, std::set<AFAStatePtr>> redundancyMap;

    //This method finds all states which have same AMap  and HMap = false in the complete AFA
    CollectSameAMapInfo(state, redundancyMap);

    //all states which satisfies criteria are put in groups

    int xyz=0;

    BOOST_FOREACH(auto temp, redundancyMap) {

        // add epsilon edges between all states within a group

            for(auto it1 = temp.second.begin(); it1 !=temp.second.end(); it1++){

                auto t = it1;
                t++;
                for(auto it2 = t; it2!=temp.second.end(); it2++){

                    std::set<AFAStatePtr,mapstatecomparator> s1,s2;
                    s2.insert(*it2);
                    s1.insert(*it1);

                    //adding epsilon edge between both states
                    (*it1)->mTransitions.insert(std::make_pair("0",s2));
                    (*it2)->mTransitions.insert(std::make_pair("0",s1));

                    s1.clear();
                    s2.clear();

                }
            }
    }

}

void AFAut::GetEpsilonClosure(AFAStatePtr state, std::map<AFAStatePtr , std::set<AFAStatePtr>> &closureMap){

    // This method is a helper method for NewEpsilonClosure() method
    // This method finds epsilon closure of all states

    std::set<AFAStatePtr> temp;
    //temp set stores epsilon closure of current state

    std::queue<AFAStatePtr> todo;
    //for standard traversal by taking only epsilon edges

    temp.insert(state);
    todo.push(state);

    while(!todo.empty()){

        AFAStatePtr t = todo.front();
        todo.pop();

        //checking all transitions of state t
        BOOST_FOREACH(auto trans, t->mTransitions) {

                    std::string sym(trans.first);

                    if (sym == "0") {
                        // work for only epsilon transitions

                        AFAStatePtr adj = *trans.second.begin();
                        if (temp.find(adj) == temp.end()) {
                            //if not already added in temp set
                            temp.insert(adj);
                            todo.push(adj);
                        }
                    }
        }
    }

    //epsilon closure of state computed and is stored in temp set
    //make entry in closureMap
    closureMap.insert(std::make_pair(state, temp));

    //seeing all transitions of state
    BOOST_FOREACH(auto trans, state->mTransitions) {
                    BOOST_FOREACH(auto adj, trans.second) {

                                    //if no entry for adj in closureMap then recursive call
                                    // to compute it's epsilon closure
                                    if (closureMap.find(adj) == closureMap.end())
                                        GetEpsilonClosure(adj, closureMap);
                    }
    }
}


void AFAut::NewEpsilonClosure(AFAStatePtr state){

    //This method removes epsilon edges(cyclic epsilon edges) from AFA which are created by
    // ConvertToEpsilonAllSameAMap() for removing false dependencies from AFA

    std::map<AFAStatePtr, std::set<AFAStatePtr>> closureMap;

    //Find epsilon closure of all states
    GetEpsilonClosure(state, closureMap);

    //Till now, for all states in AFA, we have computed their epsilon closures
    // Now creating states for new AFA, each new state is a representing state

    std::map<std::set<AFAStatePtr>, AFAStatePtr> newStatesMap;

    BOOST_FOREACH(auto temp, closureMap) {

        if(newStatesMap.find(temp.second)==newStatesMap.end()){

            z3::context c;
            z3::expr exp = c.bool_val(true);

            //new AFA state created
            AFAStatePtr st = new AFAState(ORLit,exp);
            st->mAMap = temp.first->mAMap;
            st->mHMap = temp.first->mHMap;


            //AFAStatePtr st = new AFAState(temp.first->mType, temp.first->mAMap);
            newStatesMap.insert(std::make_pair(temp.second, st));
        }
    }


    //states of new AFA are created, now add edges between the new states

    BOOST_FOREACH(auto t, newStatesMap) {

        std::set<AFAStatePtr> statesSet = t.first;
        //stateSet is representing set of states

        AFAStatePtr st = t.second;
        //st is state of new AFA

        BOOST_FOREACH(auto s, statesSet) {
            //each state in stateSet will contribute to add edges from the new state

            if(s->mIsAccepted)
                st->mIsAccepted= true;

            st->mHMap = s->mHMap;

            BOOST_FOREACH(auto trans, s->mTransitions) {
                    //covering all edges from each state in stateSet

                        std::string sym(trans.first);

                        if (sym != "0") {
                                //work only for non-epsilon edges

                                std::set<AFAStatePtr,mapstatecomparator> temp;

                                BOOST_FOREACH(auto u, trans.second){

                                        //get new state corresponding to representing state of state u
                                        temp.insert(newStatesMap[closureMap[u]]);
                                }

                                st->mTransitions.insert(std::make_pair(sym, temp));
                                    //added transition entry for new state
                        }
            }
        }

    }
    //initial state of new AFA
    this->mInit = newStatesMap[closureMap[state]];

}


void AFAut::PassFourNew(AFAStatePtr init, std::set<AFAStatePtr>& tobedeleted, int cases,faudes::Generator& lGenerator, std::map<z3::expr, bool,mapexpcomparator>& mUnsatMemoization)
{/*
	int i;
	//keep all values present in statemap to be deleted after next loop.
	//Pass 1:
	std::set<std::tuple<AFAStatePtr,std::string,AFAStatePtr>, tuplecomparator> toANDLink;
	std::set<AFAStatePtr> ANDORStates;
	std::set<AFAStatePtr> ORLitStates;
  std::map<AFAStatePtr,	std::set<std::tuple<std::string,AFAStatePtr>>> assumeinfomap;
#ifdef DBGPRNT
	std::cout<<"About to construct phase1 pass4"<<std::endl;
#endif

	init->PassFourPhaseOne(ANDORStates,ORLitStates,toANDLink*//*,assumeinfomap*//*,mUnsatMemoization);
	//when done in seenmap we have Eq classes for every distinct amap.
	//delete all states present in the set allstates.., No we cant as we are currently in a state only..
	//so its better to return it..
	*//*BOOST_FOREACH(auto t, allStates){
		delete(t.first);
	}*//*
	//NO change in graph by this pass.. only collecting information to be used in phase 2
	AFAut* tm=new AFAut();
	tm->mInit=init;
#ifdef DBGPRNT
	//tm->PrintToDot("Pass4Phase1.dot");
	std::cout<<"Pass 1 over"<<std::endl;
	//std::cin>>i;
#endif

	//tm->PrintToDot("Pass4Phase1.dot");
	//	std::cout<<"Pass 1 over"<<std::endl;
	//Pass0: propoage 0's on OR/AND states so that after this every 0's transition from OR/AND goest to ORLit state only.
	//we can use the fact that by this time we have a nice tree like structure of FA
  std::map<AFAStatePtr,std::set<std::set<AFAStatePtr>>> donemap;
  //std::cout<<"About to construct phase0 pass4"<<std::endl;
	std::set<std::set<AFAStatePtr>> retset = init->PassFourPhaseZero(donemap);
	//if init was OR/AND then all this set will be on 0,
	//if init was ORLit then it will be single state (init) in the returned set


		tm->mInit=init;
#ifdef DBGPRNT
		//tm->PrintToDot("Pass4Phase0.dot");
		std::cout<<"Pass 0 over"<<std::endl;
		//std::cin>>i;
#endif


		tobedeleted.insert(ANDORStates.begin(),ANDORStates.end());
		tobedeleted.insert(ORLitStates.begin(),ORLitStates.end());


	//Pass 2: //now we will add transitions  toANDLink,

	//for cases when orlit has a sym labeled edge to OR/AND replace that OR/AND by its transitions which will certainly be OR/AND
	//create epsilon closure for 0 labeled edges coming out from ANDOR eq states..
	//sufficient to look at ORLIt states and check if they have an edge to ANDORStates.. if yes on some symbol say sym
	//then add that ANDOR state's transitions on sym to this state.after remvoing zero to that ANDORState..

	BOOST_FOREACH(auto t, toANDLink){
		AFAStatePtr src = std::get<0>(t);
		std::string sym = std::get<1>(t);
		AFAStatePtr dest = std::get<2>(t);
	 // BOOST_ASSERT_MSG(dest->mType!=ORLit,"Some serious error");
		std::set<std::pair<std::string,std::set<AFAStatePtr>>,transitionscomparatorraw> transsetthisone;
		//asssert AFAut* PassFourNew(AFAStatePtr init, std::set<AFAStatePtr>& allStates , int, faudes::Generator&);
		for(std::multimap<std::string,SetAFAStatesPtr>::iterator it= src->mTransitions.equal_range(sym).first;it!=src->mTransitions.equal_range(sym).second;it++)
		{
			if(it->second.find(dest)!=it->second.end()){
				BOOST_ASSERT_MSG(it->second.size()==1,"Seriour error");
			}else
			{
				std::set<AFAStatePtr> custset(it->second.begin(),it->second.end());
				transsetthisone.insert(std::make_pair(it->first,custset));
			}
		}
		src->mTransitions.erase(sym);
		BOOST_FOREACH(auto t, src->mTransitions){
			std::set<AFAStatePtr> own(t.second.begin(),t.second.end());
			transsetthisone.insert(std::make_pair(t.first,own));
		}

    if(dest->mType!=ORLit)
    {
  		BOOST_FOREACH(auto t, dest->mTransitions){
	  		//BOOST_ASSERT_MSG(t.first=="0","SOmer serious error");
		  	std::set<AFAStatePtr> custset(t.second.begin(),t.second.end());
        //if t.second is singleton and is same as src then dont add...
        if(t.second.size()==1)
        {
          AFAStatePtr ss= *(t.second.begin());
         // if(src->mAMap.hash()==ss->mAMap.hash() && src->mHMap->hash()==ss->mHMap->hash())
         //   continue;
        }
    		transsetthisone.insert(std::make_pair(sym,custset));
		  }
    }else
    {
      std::set<AFAStatePtr> dset;
      dset.insert(dest);
      transsetthisone.insert(std::make_pair(sym,dset));
    }
  	src->mTransitions.clear();
		BOOST_FOREACH(auto t, transsetthisone){
			SetAFAStatesPtr custset(t.second.begin(),t.second.end());
			src->mTransitions.insert(std::make_pair(t.first,custset));
			//also change the type of src node to the same type as of dest.
			src->mType = dest->mType;
			//ORLitStates.erase(src);//erase this element from OrLitStates set because we have changed its type to AND.

		}
	//	src->mTransitions.insert(transsetthisone.begin(),transsetthisone.end());
	}
*/

#ifdef DBGPRNT
		std::cout<<"Pass 2 over"<<std::endl;
		tm->PrintToDot("Pass4Phase2.dot");
		std::cin>>i;
#endif

    std::pair<StateType,std::set<SetAFAStatesPtr>> res = this->EpsilonClosure(this->mInit);
    SetAFAStatesPtr tmp = flatten_set(res.second);
    BOOST_ASSERT_MSG(tmp.size()==1,"ERR: There can only be one initial state of AFA.");
    this->mInit = *(tmp.begin());
    //this->PrintToDot(std::string("Pass4Phase1EpsilonClosure.dot"));
    //this->PrintToDot(std::string("AFA_1.dot"));

    AFAut*  temp_afa = this->DeepCopyAFA(this);

    if(MetaState::proof_no==0)
        MetaState::combinedAFA = temp_afa;
    else{

        //MetaState::combinedAFA->PrintToDot(std::string("combination_phase1.dot"));

        for(auto x: temp_afa->mInit->mTransitions){

            auto sym = x.first;
            auto trans = x.second;

            MetaState::combinedAFA->mInit->mTransitions.insert(std::make_pair(sym,trans));
        }

    }

    //MetaState::combinedAFA->PrintToDot(std::string("my1.dot"));
    AFAut*  afa = this->DeepCopyAFA(MetaState::combinedAFA);

    this->mInit = afa->mInit;

    //this->PrintToDot(std::string("AFA_2.dot"));
    //MetaState::combinedAFA->PrintToDot(std::string("combination.dot"));

    //Step 1- Add epsilon edges by connecting states having same AMap.
    this->ConvertToEpsilonAllSameAMap(this->mInit);
    std::cout<<"Phase 4- Adding Epsilon Edges over"<<std::endl;

    //this->PrintToDot(std::string("Pass4WithEpsilonEdges.dot"));
    //this->PrintToDot(std::string("AFA_3.dot"));

    //Step 2- Create epsilon closure by removing epsilon edges from AFA.f
        this->NewEpsilonClosure(this->mInit);
    std::cout<<"Phase 4- Removing Epsilon Edges over"<<std::endl;

    //this->PrintToDot(std::string("Pass4EpsilonClosed.dot"));
    //this->PrintToDot(std::string("AFA_4.dot"));


    //Step 3
    this->AFAmoreTrans(this->mInit);
    //this->PrintToDot(std::string("AFA_5.dot"));


    //Step 4- Introduce self loops in the AFA
    std::set<AFAStatePtr> seen;
    this->AFASelfLoop(this->mInit, seen);

    this->PrintToDot(std::string("Pass4SelfLooped.dot"));
    //this->PrintToDot(std::string("AFA_6.dot"));
    //afa->PrintToDot(std::string("my2.dot"));

    MetaState::proof_no++;


//    std::cout<<MetaState::proof_no<<std::endl;


//         if(MetaState::proof_no==2){
//
//        MetaState::afaRoots.push_back(this->mInit);
//
//        AFAut* afa = new AFAut();
////        z3::context c;
////        z3::expr exp = c.bool_val(true);
////        AFAStatePtr st = new AFAState(ORLit,exp);
////        afa->mInit = st;
////
//
//        auto afa1 = MetaState::afaRoots[0];
//        auto afa2 = MetaState::afaRoots[1];
//
//        for(auto x: afa2->mTransitions){
//
//            auto sym = x.first;
//            auto trans = x.second;
//
//            afa1->mTransitions.insert(std::make_pair(sym,trans));
//        }
//
//        afa->mInit = afa1;
//
//
//        for(auto afa_temp : MetaState::afaRoots) {
//
//            std::set<AFAStatePtr, mapstatecomparator> temp;
//
//            temp.insert(afa_temp);
//
//            st->mTransitions.insert(std::make_pair("0", temp));
//        }
//
//        std::cout<<"Combined together"<<std::endl;
//
//        afa->PrintToDot(std::string("combined1.dot"));
//
//        afa->ConvertToEpsilonAllSameAMap(afa->mInit);
//
//        afa->PrintToDot(std::string("combined2.dot"));
//
//        afa->NewEpsilonClosure(afa->mInit);
//
//        afa->PrintToDot(std::string("combined3.dot"));
//
//        std::cout<<"Done"<<std::endl;
//
//    }


#ifdef DBGPRNT
	std::cout<<"Phase eqclass over"<<std::endl;
	tm->PrintToDot(std::string("Pass4PhaseEqClass.dot"));
#endif

//std::cin>>i;




//Now convert this AFA to DFA
//this->createDFA(lGenerator);


std::cout<<"Marked states size = "<<lGenerator.MarkedStatesSize()<<","<<lGenerator.MarkedStatesToString()<<std::endl;
#ifdef DBGPRNT
	std::cout<<"Complement done "<<std::endl;
	tm->PrintToDot(std::string("Pass4PhaseComplement.dot"));
#endif
	//lGenerator.DotWrite("DFAConverted.dot");
	//delete tm;


return;

}





std::set<std::set<AFAStatePtr>> AFAut::GetConjunctedTransitions(std::set<AFAStatePtr> stateset, std::string sym){
			std::set<std::set<AFAStatePtr>> tobeaddedset;
			std::set<AFAStatePtr> emptyset;
			tobeaddedset.insert(emptyset);
			BOOST_FOREACH(auto state, stateset){
				if(state->mTransitions.find(sym)==state->mTransitions.end()){
					tobeaddedset.clear();//This is necessary to avoid executing the loop with partially filled tobeaddedset
					break;//means now we can not proceed to have an outgoing edge on this  symbol because at least one
					//does not have that..
				}
				std::set<std::set<AFAStatePtr>> tmptobeaddedset;
				BOOST_FOREACH(auto w, tobeaddedset){
					for(std::multimap<std::string, SetAFAStatesPtr>::iterator it= state->mTransitions.equal_range(sym).first; it!= state->mTransitions.equal_range(sym).second; it++)
					{
						std::set<AFAStatePtr> tmp(it->second.begin(),it->second.end());
						std::set<AFAStatePtr> tobeadded(w.begin(),w.end());
						tobeadded.insert(tmp.begin(),tmp.end());
						tmptobeaddedset.insert(tobeadded);
					}

				}
				tobeaddedset.clear();
				tobeaddedset.insert(tmptobeaddedset.begin(),tmptobeaddedset.end());

			}
			return tobeaddedset;
}


std::set<AFAStatePtr> AFAut::GetDisjunctedTransitions(std::set<AFAStatePtr> stateset, std::string sym){
			std::set<AFAStatePtr> tobeaddedset;
			BOOST_FOREACH(auto state, stateset){
				if(state->mTransitions.find(sym)!=state->mTransitions.end()){
					for(std::multimap<std::string,SetAFAStatesPtr>::iterator it= state->mTransitions.equal_range(sym).first;it!=state->mTransitions.equal_range(sym).second;it++)
					{
						SetAFAStatesPtr st = it->second;
						tobeaddedset.insert(st.begin(),st.end());
					}
				}
			}
			return tobeaddedset;
}
