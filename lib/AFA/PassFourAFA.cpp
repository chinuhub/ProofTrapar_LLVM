/*
 * PassFourAFA.cpp

 *
 *  Created on: 23-Sep-2015
 *      Author: jaganmohini
 */

#include "AFA/AFAut.h"


void AFAut::PassFourNew(AFAStatePtr init, std::set<AFAStatePtr>& tobedeleted, int cases,faudes::Generator& lGenerator, std::map<z3::expr, bool,mapexpcomparator>& mUnsatMemoization)
{
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

	init->PassFourPhaseOne(ANDORStates,ORLitStates,toANDLink/*,assumeinfomap*/,mUnsatMemoization);
	//when done in seenmap we have Eq classes for every distinct amap.
	//delete all states present in the set allstates.., No we cant as we are currently in a state only..
	//so its better to return it..
	/*BOOST_FOREACH(auto t, allStates){
		delete(t.first);
	}*/
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


#ifdef DBGPRNT
		std::cout<<"Pass 2 over"<<std::endl;
		tm->PrintToDot("Pass4Phase2.dot");
		std::cin>>i;
#endif
		//	std::cin>>i;



#ifdef DBGPRNT
	std::cout<<"Phase eqclass over"<<std::endl;
	tm->PrintToDot(std::string("Pass4PhaseEqClass.dot"));
#endif
    tm->PrintToDot(std::string("Pass4EpsilonClosure.dot"));

//std::cin>>i;




//Now convert this AFA to DFA
tm->createDFA(lGenerator);


std::cout<<"Marked states size = "<<lGenerator.MarkedStatesSize()<<","<<lGenerator.MarkedStatesToString()<<std::endl;
#ifdef DBGPRNT
	std::cout<<"Complement done "<<std::endl;
	tm->PrintToDot(std::string("Pass4PhaseComplement.dot"));
#endif
	lGenerator.DotWrite("DFAConverted.dot");
	delete tm;


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
