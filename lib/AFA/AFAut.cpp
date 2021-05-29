/*
 * AFAut.cpp
 *
 *  Created on: 23-Aug-2015
 *      Author: jaganmohini
 */

#include "AFA/AFAut.h"
#include "Utils.h"
#include <iterator>
#include <map>
#include<set>

template <class Name>

class myEdgeWriter {
  public:
         myEdgeWriter(Name _name) : name(_name) {}
              template <class VertexOrEdge>
                     void operator()(std::ostream& out, const VertexOrEdge& v) const {
                                   out << "[label=\"" << name[v].label << "\"]";
                                        }
  private:
                   Name name;
};


template <class Name>
class myVertWriter {
  public:
         myVertWriter(Name _name) : name(_name) {}
              template <class VertexOrEdge>
                     void operator()(std::ostream& out, const VertexOrEdge& v) const {
//                                   out << "[label=\"" << name[v].vertlabel << "\", xlabel=\" "<<name[v].xlabel<<"\", color="<<name[v].color<<",shape="<<name[v].shape<<"]";
            	  out << "[label=\"" << name[v].vertlabel << "\", color="<<name[v].color<<",shape="<<name[v].shape<<"]";
                                        }
  private:
                   Name name;
};

struct autstatecomparator{
	bool operator() (const struct autstate* one, const struct autstate*  two) const
	{
		return one<two;
	}
};

Program* AFAut::mProgram;

/**
 * Function to convert an NFA/DFA to an AFA
 * Commenting out for now as it is not being used and we want to get away with fa.h library.
 */
/*AFAut* AFAut::MakeAFAutFromFA(struct fa* nfa,Program* program, z3::context& ctx){
	mProgram= program;

	AFAut* afa = new AFAut();
	std::map<struct autstate*, AFAStatePtr> seenstates;
	AFAStatePtr init = RecMakeAFAutFromFA(nfa->initial,seenstates,ctx);

	BOOST_ASSERT_MSG(seenstates.find(nfa->initial)->second==init,"Some serious issue");
	afa->mInit=init;
*//*
	BOOST_FOREACH(auto t, init->mTransitions){
		BOOST_FOREACH(auto v, t.second)
				std::cout<<v<<","<<std::endl;
	}
	afa->PrintToDot("tryinside.dot");
*//*
	//complete the AFA
	//Now before returning, complete the AFA, i.e. for each state, get set of symbols on which it has
		 	    //transitions, then find where it doesnt have and add an edge from this state on this symbol to error state.
		 	   z3::expr trueexp = ctx.bool_val(true);
		 	   AFAStatePtr errorstate= new AFAState(OR,trueexp);
		 	   SetAFAStatesPtr seterrostate;
		 	   seterrostate.insert(errorstate);
		 	   std::set<std::string> allsyms(mProgram->mAllSyms.begin(),mProgram->mAllSyms.end());
		 	  //add self loop on every transition from errostate to itself.
 	  	 	   errorstate->mTransitions.clear();// a small convenience as we know by this time only 0 has been added
 	  	 	   //by the constructor of AFAState for this error state.
 	  	 	   BOOST_FOREACH(auto symp, allsyms)
 	  	 	   	   errorstate->mTransitions.insert(std::make_pair(symp,seterrostate));

		 	   BOOST_FOREACH(auto st, seenstates){
		 		   AFAStatePtr state = st.second;
		 		   std::set<std::string> alltransyms =state->getTransitionKeys();
		 		   std::set<std::string> diffset;
		 		   std::set_difference(allsyms.begin(),allsyms.end(),alltransyms.begin(),alltransyms.end(),std::inserter(diffset,diffset.begin()));
		 		   BOOST_FOREACH(auto ss, diffset){
		 			   state->mTransitions.insert(std::make_pair(ss,seterrostate));
		 		   }
		 	   }
	return afa;
}*/


/**
 * Commenting out this method as it was only used by MakeAFAutFromFA method (above) which is no longer present.
 * We want to avoid the usage of fa.h.
 */
/*AFAStatePtr AFAut::RecMakeAFAutFromFA(struct autstate* state, std::map<struct autstate*, AFAStatePtr>& seenstates,z3::context& ctx)
{
//std::cout<<"iter"<<std::endl;
	if(seenstates.find(state)!=seenstates.end())
	{
		return seenstates.find(state)->second;
	}
	z3::expr trueexp = ctx.bool_val(true);
	AFAStatePtr st = new AFAState(ORLit,trueexp);
	if(state->accept)
	{
#ifdef	DBGPRNT
		std::cout<<"set as an accepted state"<<std::endl;
#endif
		st->mIsAccepted=true;

	}
	seenstates.insert(std::make_pair(state,st));
	//for each outgoing edge from state.. call Recursively and get the AFAStatePtr to be added to st's
	//transition system
	for(int i=0; i< state->tused; i++)
		{
			struct auttrans trans = state->trans[i];
			//find the destination state
			struct autstate* dest = trans.to;
			//find symbol

			std::string sym(1,trans.min);
//			std::cout<<"character is "<<trans.min<<" and string symbol is "<<sym<<std::endl;
			AFAStatePtr destafa = RecMakeAFAutFromFA(dest,seenstates,ctx);
//			std::cout<<"Adding edge"<<std::endl;
			SetAFAStatesPtr setown;
			setown.insert(destafa);
			//st->mTransitions.insert(std::make_pair(sym,setown));
			st->HelperAddEdgeIfAbsent(st,destafa,sym);
		}
	return st;
}*/


/**
 * Function to construct and AFA corresponding to word w and neg phi as phi
 */
void AFAut::MakeAFAutProof(std::string& word, z3::expr& mPhi,Program* p, int count, bool& bres, faudes::Generator& generator){
	mProgram= p;
    std::map<z3::expr, bool,mapexpcomparator> mUnsatMemoization;

    //for mPhi, create a state appropriately.., add this state to a worklist.. and repeat until worklist is empty..
			//inside loop, remove an element from this worklist and invoke the function FillState on it..
			//whatever is returned is added to the worklist.
			z3::context& ctx= mPhi.ctx();
			AFAut* afa = new AFAut();
			z3::tactic t = z3::tactic(ctx,"tseitin-cnf");
			z3::goal g(ctx);
			g.add(mPhi);
			z3::apply_result res = t(g);
			std::map<AFAStatePtr,AFAStatePtr,mapstatecomparator> allStates;
			if(res.size()>1){
								//then it is surely of or type.. claus 1 or clause 2.. create an DisjAND state
				z3::expr formula = g.as_expr();
				AFAStatePtr st = new AFAState(OR, word, formula);
				allStates.insert(std::make_pair(st,st));
#ifdef	DBGPRNT
				std::cout<<mPhi<<" found to be a or b or c type "<<std::endl;
#endif
				st->PassOne(allStates);
				afa->mInit=st;
			}else if(res.size()==1){
				z3::expr formula = res[0].as_expr();
				if(formula.decl().decl_kind()==Z3_OP_AND)
				{
					AFAStatePtr st = new AFAState(AND,word,formula);
					allStates.insert(std::make_pair(st,st));
#ifdef	DBGPRNT
					std::cout<<mPhi<<" found to be a and b and c type "<<std::endl;
#endif
					st->PassOne(allStates);
					afa->mInit = st;
				}else if(formula.decl().decl_kind()==Z3_OP_OR)
				{
					AFAStatePtr st = new AFAState(OR,word,mPhi);
					allStates.insert(std::make_pair(st,st));
#ifdef	DBGPRNT
					std::cout<<mPhi<<" found to be a or b type "<<std::endl;
#endif
					st->PassOne(allStates);
					afa->mInit = st;
				}else
				{
					AFAStatePtr st = new AFAState(ORLit,word,mPhi);
					allStates.insert(std::make_pair(st,st));
#ifdef	DBGPRNT
					std::cout<<mPhi<<" found to be a literal "<<" and type is "<<st->mType<<std::endl;
#endif
					st->PassOne(allStates);
					afa->mInit = st;
				}
			}
#ifdef DBGPRNT
			afa->PrintToDot("Pass1.dot");
#endif
			/*
			 * We will now construct HMap by traversing over the AFA constructed so far. Earlier we were doing AFA construction (forward) and HMap
			 * computation simultaneously in PassOne. HMap was being computed by looking at the HMap of the children nodes of a AFA node. However,
			 * if we allow sharing of multiple AFA states with same AMap and RWord, it is necessary to postpone the computation of HMap until the whole
			 * AFA construction is over. That is why we now call another pass just to compute HMap for the given AFA.
			 */

			//afa->mInit->PassHMap();
			afa->PrintToDot("Pass1.dot");
			if(!afa->mInit->HelperIsUnsat(*(afa->mInit->mHMap)))
			{
				bres=false;
				return;//to denote that this trace is erroneous
			}
			bres=true;

#ifdef	DBGPRNT
			afa->PrintToDot("Pass1.dot");
#endif

			//Pass two..
#ifdef	DBGPRNT
			std::cout<<"Starting Pass two"<<std::endl;
#endif
			std::map<AFAStatePtr,AFAStatePtr,mapstatecomparator> passtwoallstates;
			afa->mInit->PassTwo(passtwoallstates, mUnsatMemoization);
#ifdef	DBGPRNT
			std::cout<<"Pass two ended"<<std::endl;
			afa->PrintToDot("Pass2.dot");
#endif
			afa->PrintToDot("Pass2.dot");
			//when this call is returned, passtwoallstates contain only those states which survived extinction.. So we can delete all other
			//states as thy become unreachable as well.
			// std::set<AFAStatePtr,mapstatecomparator> unreachable;
			BOOST_FOREACH(auto t, allStates){
				AFAStatePtr el = t.first;
				if(passtwoallstates.find(el)==passtwoallstates.end()){
					//means this element was not reachable after pruning of edges..
#ifdef	DBGPRNT
					std::cout<<"Found unreachable node after Pass two "<<std::endl;
#endif
					delete el;
				}
			}
#ifdef	DBGPRNT
	 	    std::cout<<"Starting Pass four "<<std::endl;
#endif

    afa->PrintToDot("Pass2AfterDeletion.dot");

    //Pass four- adding more edges respecting implications, this is a function of AFAut only not of AFAstate
	 	    //std::tuple<SetAFAStatesPtr,SetAFAStatesPtr,AFAStatePtr> retvals =afa->mInit->PassFour(afa->mInit,passtwoallstates);
	 	   //struct fa* complementedaut=afa->mInit->PassFour(afa->mInit,passtwoallstates);
//	 	   struct fa* complementedaut=
	 	    std::set<AFAStatePtr> allStatesDel;
	 	    afa->PassFourNew(afa->mInit,allStatesDel,count,generator,mUnsatMemoization);
	 	   //delete alls tates in passtwoallstates;-- Do it later;;
	 	  /*BOOST_FOREACH(auto t, allStatesDel){
	 	  		delete(t);
	 	  	}*/
	 	  afa->mInit=NULL;
	 	  delete afa;//no longer in use..
	 	  std::cout<<"Proofafa done"<<std::endl;
	 	  return;
}
/**
 * Complement this AFA and changes init accordingly==
*/

AFAStatePtr AFAut::Complement(AFAStatePtr falseacc){
	std::map<AFAStatePtr,bool> mapseenset;
  //falseacc->mIsAccepted=false;
	z3::expr falsexp = mInit->mAMap.ctx().bool_val(false);
	AFAStatePtr newerrorstate=new AFAState(ORLit,falsexp);
	newerrorstate->mHMap=new z3::expr(falsexp);
	newerrorstate->mIsAccepted=true;
	SetAFAStatesPtr errorstateset;
	errorstateset.insert(newerrorstate);
	BOOST_FOREACH(auto sym, mProgram->mAllSyms)
		newerrorstate->mTransitions.insert(std::make_pair(sym,errorstateset));

	if(mInit->mType!=ORLit){
		//means it will have only 0 labeled edges.. we must do the necessary part here (cnverting and/or and then call
		//recomplement function on each of them..
		//assert that all keys from this state is of 0 form..
		std::set<std::string> transset = mInit->getTransitionKeys();

		BOOST_ASSERT_MSG(transset.size()==1 && transset.find("0")!=transset.end(),"Some issue as all transitions from such states must be on 0");
		//convert these transitions to OR
		//Same code as used in RecComplement function in next code
		std::set<SetAFAStatesPtr> tobeaddedset;
		SetAFAStatesPtr empty;
		tobeaddedset.insert(empty);
		for(std::multimap<std::string, SetAFAStatesPtr>::iterator it= mInit->mTransitions.equal_range("0").first; it!=mInit->mTransitions.equal_range("0").second; it++)
					{
//			std::cout<<"Inside one"<<std::endl;
						std::set<SetAFAStatesPtr> tmptobeaddedset;
						SetAFAStatesPtr curr = it->second;
						BOOST_FOREACH(auto singlesetSetStates, tobeaddedset)
						{
							for(SetAFAStatesPtr::iterator itset=curr.begin();itset!=curr.end();itset++)
							{
								SetAFAStatesPtr tmp;
								tmp.insert(singlesetSetStates.begin(),singlesetSetStates.end());
#ifdef	DBGPRNT
								std::cout<<"Invoking recomplement with "<<*itset<<std::endl;
#endif
								RecComplement(*itset,mapseenset,errorstateset,falseacc);
								tmp.insert(*itset);
								tmptobeaddedset.insert(tmp);
							}
						}
						tobeaddedset.clear();
						tobeaddedset.insert(tmptobeaddedset.begin(),tmptobeaddedset.end());
					}

					//add new ones..
					mInit->mTransitions.erase("0");//remove all elements of this symbol..
					BOOST_FOREACH(auto ele, tobeaddedset)
						mInit->mTransitions.insert(std::make_pair("0",ele));


	}else
		RecComplement(mInit,mapseenset,errorstateset,falseacc);

  return newerrorstate;
}

void AFAut::RecComplement(AFAStatePtr state, std::map<AFAStatePtr,bool>& mapseenset, SetAFAStatesPtr errorstateset,AFAStatePtr falseacc)
{

	if(mapseenset.find(state)!=mapseenset.end())
	{
		return;
	}else
	{
		mapseenset.insert(std::make_pair(state,true));

		if(state->mIsAccepted)
			state->mIsAccepted=false;
		else
			state->mIsAccepted=true;
		std::set<std::string> allsyms(mProgram->mAllSyms.begin(),mProgram->mAllSyms.end());

		for(std::set<std::string>::iterator it= allsyms.begin(); it!=allsyms.end(); it++){
			std::string sym = *it;
			std::set<SetAFAStatesPtr> tobeaddedset;
			SetAFAStatesPtr emptyset;
			tobeaddedset.insert(emptyset);
			//INV: Following loop must be entered for every symbol.
//			std::cout<<"Checking for edge on "<<sym<<" from "<<state<<std::endl;
#ifdef	DBGPRNT
			if(state->mTransitions.find(sym)==state->mTransitions.end())
				std::cout<<"No transition from "<<state<<" on symbol "<<sym<<std::endl;
			BOOST_ASSERT_MSG(state->mTransitions.find(sym)!=state->mTransitions.end(),"Some serious error as this state must have transition on every symbol");
#endif
      bool found=false;
      int i=0;
			for(std::multimap<std::string, SetAFAStatesPtr>::iterator it= state->mTransitions.equal_range(sym).first; it!=state->mTransitions.equal_range(sym).second; it++)
			{
        found=true;
				std::set<SetAFAStatesPtr> tmptobeaddedset;
				SetAFAStatesPtr curr = it->second;
				BOOST_FOREACH(auto singlesetSetStates, tobeaddedset)
				{
					for(SetAFAStatesPtr::iterator itset=curr.begin();itset!=curr.end();itset++)
					{
						SetAFAStatesPtr tmp;
						tmp.insert(singlesetSetStates.begin(),singlesetSetStates.end());
						RecComplement(*itset,mapseenset,errorstateset,falseacc);
						tmp.insert(*itset);
						tmptobeaddedset.insert(tmp);
					}
				}
				tobeaddedset.clear();
				tobeaddedset.insert(tmptobeaddedset.begin(),tmptobeaddedset.end());
        i++;
			}
      //Following is necessary otherwise an empty entry present in tobeaddedset
      //will cause two entris to be added for this stuff.. this was a bug..
      if(!found)
        tobeaddedset.clear();

			//add new ones..
			state->mTransitions.erase(sym);//remove all elements of this symbol..
			BOOST_FOREACH(auto ele, tobeaddedset){
        if(falseacc!=NULL && ele.find(falseacc)!=ele.end())//if old accepted state is present in the tobeadded trans continue;;
          continue;
				state->mTransitions.insert(std::make_pair(sym,ele));
      }
			//The following part is to make sure that if there was no edge labelled some sym from this state
			//then on this symbol a transition must go to erstwhile error state which has now become an accepting state
			if(!found){
				state->mTransitions.insert(std::make_pair(sym,errorstateset));
			}
		}
	}
}
/*
 * This method returns explicit transitions coming out from this state. An explicit transition is the one that is explicitly presnet in this state.
 */
std::map<std::string, SetAFAStatesPtr > AFAut::getExplicitTransitions(AFAStatePtr state) {
    std::map<std::string, SetAFAStatesPtr > res;
    BOOST_FOREACH(auto trans, state->mTransitions) {
                    //get the symbol name on which this transition takes place.
                    std::string symname = trans.first;
                    //get the destination state/states. add them to a SetAFAStatePtr and at the end insert the entry (symname, setafastatptr) into res.
                    SetAFAStatesPtr deststates;
                    BOOST_FOREACH( auto deststate, trans.second){
                                    //add symname,deststate into map result.
                                    deststates.insert(deststate);
                                }
                    //INVARIANT: if more than one destination states on a transition then this state must be of AND type.
                    if(deststates.size()>1)
                        BOOST_ASSERT_MSG(state->mType==AND || state->mType==OR,"Only an AND/OR state can have more than 2 next transitions on same symbol at this point. We found an OR/ORLit with two next transitions on the same symbol");

                    //add this information in res variable to be returned later.
                    res.insert(std::make_pair(symname,deststates));
                }
    return res;

}
/*
 * This method returns a set of implicit transitions coming out of this state. An implicit transition is not explicitly present in the transitions data
 * structure of this state but is computed as following;
 * For a symbol S, there is an explicit transition from this state on S to the same state iff
 * 1. HMap(this) is false and S is of type assume symbol. This follows from the monotonicity of the WP.
 * 2. S is an assignment and the free variables of AMap(this) and the to-be-modified variables in assignment do not intersect. This follows from
 * the fact that in WP a formula changes only for assignment operations.
 * 3. If S is LCAS then it causes AMap to be conjuncted as well as something to be modified. It is important to decide what will we do in this case. @TODO
 */
std::map<std::string, AFAStatePtr > AFAut::getImplicitTransitions(AFAStatePtr state) {
    std::map<std::string, AFAStatePtr > res;
    BOOST_FOREACH(auto sym, this->mProgram->mAllSyms){
        //get AMap of this state
        z3::expr lAMap(state->mAMap);
        //get HMap of this state
        z3::expr lHMap(*state->mHMap);
        //get FreeVar of AMap
        std::set<z3::expr,mapexpcomparator> freevars=state->HelperGetFreeVars(lAMap);
        //Check if HMap is unsat or not.
        bool isHMapFalse = false;
        if(state->HelperIsUnsat(lHMap)){
            isHMapFalse=true;
        }
        if(isHMapFalse && AFAut::mProgram->mAssumeLHRHMap.find(sym)!=AFAut::mProgram->mAssumeLHRHMap.end()) {
            //Adding transitions for case 1 above (in the comment)
            //HMap is false and this is an assume symbol. So following the monotonicity rule it is safe to add a self loop here on this symbol.
            res.insert(std::make_pair(sym,state));
        }else if(AFAut::mProgram->mRWLHRHMap.find(sym)!=AFAut::mProgram->mRWLHRHMap.end()) {
            //Addition transitions for case 2 above (in the comment)
            //means it is a read/write symbol
            //get left side of the assignment
            z3::expr left(std::get<0>((AFAut::mProgram->mRWLHRHMap.find(sym)->second)));
            if(freevars.find(left)==freevars.end()) {
                //means this lhs var of assignment doesn't conflict with the free variables in amap and hence can cause a self loop to appear on this state
                //on this symbol because WP will not be changed for this set of post condition (AMap) and sym.
                res.insert(std::make_pair(sym, state));
            }
        }else if(AFAut::mProgram->mCASLHRHMap.find(sym)!=AFAut::mProgram->mCASLHRHMap.end()){
            //means it is a cas symbol
            //get left side, old value, new value and result assignment var part from this symbol.
            z3::expr left(std::get<0>(AFAut::mProgram->mCASLHRHMap.find(sym)->second));
            z3::expr readarg(std::get<1>(AFAut::mProgram->mCASLHRHMap.find(sym)->second));
            z3::expr writearg(std::get<2>(AFAut::mProgram->mCASLHRHMap.find(sym)->second));
            z3::expr assignvar(std::get<3>(AFAut::mProgram->mCASLHRHMap.find(sym)->second)) ;

            bool safesubstitution = true;//variable to hold if any substitution in AMap of this state will happen due to CAS instruction or not.
            //Following is for the part when cas is successful
            if(freevars.find(left)!=freevars.end()) {
                //means free vars of AMap  contain left side of the cas so substitution will not be safe
                safesubstitution = safesubstitution && false;
            }

            //Following is for the part when cas is unsuccessful
            if(freevars.find(assignvar)!=freevars.end()){
                //means free vars of AMap contain the assignment variable of cas, so substitution will not be safe
                safesubstitution = safesubstitution && false;
            }
            //After substitution the only thin remaining is conjunction. We know that conjunction will be safe if HMap of this state is false (Following case 1 above)

            if(safesubstitution && isHMapFalse){
                //create a self loop on this cas symbol to the same state.
                res.insert(std::make_pair(sym,state));
            }
        }

    }

    return res;

}

/*
 * This method converts this AFA to a DFA. The resultant DFA is represented by generator. The resultant DFA is also presented by new AFAut data structure.
 * We reuse the same type to represent DFA as well because it captures the AMap present in each state as well.
 */

void AFAut::createDFA( faudes::Generator& generator){
    AFAStatePtr init = this->mInit;

    std::map<SetAFAStatesPtr, faudes::Idx> seenset;
    std::set<SetAFAStatesPtr> todoset;
    std::map<AFAStatePtr, std::map<std::string, SetAFAStatesPtr>> explicittransset;
    std::map<AFAStatePtr, std::map<std::string, AFAStatePtr>> implicittransset;
    //Put initial state into todoset as well as in seenset along with the state added to generator as well
    {
        //create an initial state in the generator and set it as initial.
        std::string initstate = std::to_string(init->mID);
        faudes::Idx idx = generator.InsState(initstate);
        generator.SetInitState(initstate);
        //prepare stuff to put into seenset and todoset
        SetAFAStatesPtr setst;
        setst.insert(init);
        seenset.insert(std::make_pair(setst,idx));
        todoset.insert(setst);
        //add self loop on generator state (idx) if possible.
        addSelfLoop(generator, implicittransset, setst, idx);
    }

    //Iterate over todoset until it be
    // comes empty
    auto setiter = todoset.begin();

    while(setiter!=todoset.end()){
        SetAFAStatesPtr states = *setiter;

        //get the generator's state index from seenset corresponding to this states.
        BOOST_ASSERT_MSG(seenset.find(states)!=seenset.end()," Invariant: If something is in todoset then it must be in the seenset as well - violated");
        faudes::Idx srcst = seenset.find(states)->second;
        //For all states in this AFAStatesSet get explicit transition symbols.
       std::set<std::string> allexplicitsyms;
        bool allaccepting = true; //make srcst as accepting state if all the states in states variable are accepting.
        BOOST_FOREACH(AFAStatePtr state, states){
            allaccepting = allaccepting && state->mIsAccepted;

            if(implicittransset.find(state)==implicittransset.end()){
                std::map<std::string, AFAStatePtr > tmpImplicit = getImplicitTransitions(state);
                    implicittransset.insert(std::make_pair(state, tmpImplicit));
            }
            if(explicittransset.find(state)==explicittransset.end()){
                std::map<std::string, SetAFAStatesPtr> tmpExplicit = getExplicitTransitions(state);
                explicittransset.insert(std::make_pair(state, tmpExplicit));
            }
            std::set<std::string> syms;
            BOOST_FOREACH(auto key, explicittransset.find(state)->second){
                syms.insert(key.first);
            }
            allexplicitsyms.insert(syms.begin(),syms.end());
        }
        if(allaccepting)
            generator.SetMarkedState(srcst);
        //At this point we have all explicit transitions from any state in states set in allexplicitsyms set. Now itereate over this set.
        BOOST_FOREACH(std::string sym, allexplicitsyms){        //for every explicit transition in allexplicitsyms set
                        bool ifNextPossibleOnSym = true;
                        SetAFAStatesPtr tmpNextStatesSet;
                        BOOST_FOREACH(AFAStatePtr state, states){         // for every state in states
                                        // check if state has an explicit or implicit transition on that symbol.
                                        bool ifExplicitPresent = (explicittransset.find(state)==explicittransset.end())?false:(explicittransset.find(state)->second.find(sym)!=explicittransset.find(state)->second.end());
                                        bool ifImplicitPresent = (implicittransset.find(state)==implicittransset.end())?false:(implicittransset.find(state)->second.find(sym)!=implicittransset.find(state)->second.end());
                                        //INV: Either both will be false or only one will be true. Both can't be true[Does not hold]
                                        //BOOST_ASS(ifExplicitPresent && ifImplicitPresent == false, " A state can't have an implicit and an explicit transition on the same symbol. Please check.");
                                        //If sym is present in implicit as well in explicit then remove from implict and keep only in explicit. (@CHOICE)
                                        if(ifExplicitPresent && ifImplicitPresent){
                                            implicittransset.find(state)->second.erase(sym);
                                            ifImplicitPresent=false;
                                        }
                                        //If none then break from the iteration of states and continue with next symbol - breakvar=true;
                                        if(!ifExplicitPresent && !ifImplicitPresent){
                                            ifNextPossibleOnSym = false;
                                            break;
                                        }
                                        //else keep on adding destination state/states in a set
                                        if(ifImplicitPresent){
                                            //get destination state on sym from implicit trans set and add to tmpSetAFAStatePtr
                                            AFAStatePtr tmp = implicittransset.find(state)->second.find(sym)->second;
                                            tmpNextStatesSet.insert(tmp);

                                        }

                                        if(ifExplicitPresent){
                                            //get destination states on sym from explicit trans set and add to tmpSetAFAStatesPtr
                                            SetAFAStatesPtr tmp = explicittransset.find(state)->second.find(sym)->second;
                                            //Note that here tmp is a set so add all elements of this set to tmpNextStatesSet
                                            tmpNextStatesSet.insert(tmp.begin(),tmp.end());
                                        }
                                    }
                        //after iteration over all states is done (without any break) - if breakvar!=true //means the inner loop was not break in between.
                        if(ifNextPossibleOnSym){
                            faudes::Idx newstate_generator;
                            //check if the resultant dest state set is already seen or not.
                            if(seenset.find(tmpNextStatesSet)==seenset.end()){ //if not, add that to todoset and seenset and create a state in the generator.
                                todoset.insert(tmpNextStatesSet);
                                //create a new state in the generator

                                std::string stname="";
                                BOOST_FOREACH(auto state, tmpNextStatesSet){
                                    std::stringstream stream;
                                    stream<<state;
                                    stname = stname + stream.str()+",";
                                }
                                newstate_generator = generator.InsState(stname);
                                seenset.insert(std::make_pair(tmpNextStatesSet,newstate_generator));
                            }else{
                                //else get the generator state from the seenset
                                newstate_generator = seenset.find(tmpNextStatesSet)->second;
                            }
                            //-- create a transition from srcst to this state in the generator on this explicit symbol.
                            faudes::Idx evnt = generator.InsEvent(sym);
                            generator.SetTransition(srcst,evnt, newstate_generator);
                            //IMP: If there is a self transition on srcst on the same symbol then remove that self-transition.
                            //INV: From srcst there should not be two outgoing transitions on evnt symbol to two different states.
                            //IMP: If there are two transitions on evnt symbol then one must be the self loop and that should be removed.
                            faudes::TransSet::Iterator start = generator.TransRelBegin(srcst,evnt);
                            faudes::TransSet::Iterator end = generator.TransRelEnd(srcst,evnt);
                            bool removeSelfLoop=false;
                            while(start!=end){
                                faudes::Idx dest = start->X2;
                                BOOST_ASSERT_MSG(dest==srcst || dest == newstate_generator," Outgoing transition on one symbol to two distinct states not possible. Check");
                                if(dest==srcst){
                                    //remove this self loop.
                                    removeSelfLoop=true; //We can't remove transition here as we are already iterating over it. Do it after the loop ends.
                                }
                                start++;
                            }
                            if(removeSelfLoop){
                                generator.ClrTransition(srcst, evnt, srcst);
                            }
                            //when to make this state as an accepting state??
                            //Make it an accepting state only if all the states in tmpNextStatesSet are accepting.
                            if(allaccepting){
                                generator.SetMarkedState(newstate_generator);
                            }
                            //also when should we add self loops on these states for implicit transitions??
                            //Add a self loop on this state for a symbol sym, if sym is in the implicit set of all the states in this set.
                            //except when there is already an explicit transition out on that symbol from that state.
                            addSelfLoop(generator, implicittransset, tmpNextStatesSet,
                                                           newstate_generator);
                            //Now we are done processing this state.
                        }
                        //else continue with the next symbol.
                    }

        todoset.erase(states);
        setiter = todoset.begin();

    }
    //Now iterate over all states (already in the seenset) and if there are no transition on a symbol sym from state s, then add a transition from s to error
    //state on symbol sym. This will make the automaton complete.
    /*std::set<std::string> allsyms;
    faudes::Idx errorstate = generator.InsState("Error");
    allsyms.insert(mProgram->mAllSyms.begin(),mProgram->mAllSyms.end());
    auto stbegin = generator.States().Begin();

    while(stbegin!=generator.States().End()){
        auto state=*stbegin;
        stbegin++;
        faudes::TransSet::Iterator beg = generator.TransRelBegin(state);
        std::set<std::string> symevents;
        while(beg!=generator.TransRelEnd(state)){
            symevents.insert(generator.EventName((*beg).Ev));
            beg++;
        }
        //Now check which all syms are in allSyms but not in symevents. Add a transition on those symbols from this state to error state.
        std::vector<std::string> res;
        std::set_intersection(allsyms.begin(), allsyms.end(), symevents.begin(), symevents.end(),  std::back_inserter(res));
        //For every symbol in res insert a transition from this state to error state on sym.
        BOOST_FOREACH(auto sym, res ){
            faudes::Idx ev = generator.InsEvent(sym);
            generator.SetTransition(state, ev, errorstate);
        }
    }*/


}

/*
 * This method adds a self loop on newstate_generator state of generator. The AFAstates corresponding to newstate_generator are in tmpNextStatesSet
 * variable. implicitTranSet map stores the implicit transitions.
 */
void AFAut::addSelfLoop(faudes::Generator &generator,
                                    std::map<AFAStatePtr, std::map<std::string, AFAStatePtr>> &implicittransset,
                                    const SetAFAStatesPtr &tmpNextStatesSet,
                                    faudes::Idx newstate_generator) {
    //Add a self loop on this state for a symbol sym, if sym is in the implicit set of all the states in this set.
    std::set<std::string> commonImplicitSyms(mProgram->mAllSyms.begin(),mProgram->mAllSyms.end());
    BOOST_FOREACH(AFAStatePtr state, tmpNextStatesSet) {
        //get implicit transition symbols on state and intersect them with commonImplicitSyms. Whatever syms remain in this set
        //at the end are the ones on which all states in tmpNextStatesSet have a transition so add self loop on those symbols.
        if (implicittransset.find(state) == implicittransset.end()) {
            std::map<std::string, AFAStatePtr> tmpImplicit = getImplicitTransitions(state);
            implicittransset.insert(std::make_pair(state, tmpImplicit));
        }
        std::set<std::string> implicitTrans;
        BOOST_FOREACH(auto key, implicittransset.find(state)->second){
                        implicitTrans.insert(key.first);
        }
        std::vector<std::string> res;
        std::set_intersection(commonImplicitSyms.begin(), commonImplicitSyms.end(), implicitTrans.begin(), implicitTrans.end(),  std::back_inserter(res));
        //now clear commonImplictSysm set and copy the content of res set to this.
        commonImplicitSyms.clear();
        commonImplicitSyms.insert(res.begin(),res.end());
    }
    //Whatever syms remain in commonImplicitSyms at the end, add a self loop on newstate_generator (newly create state) on these syms.
    BOOST_FOREACH(std::string sym, commonImplicitSyms) {
        faudes::Idx ev = generator.InsEvent(sym);
        generator.SetTransition(newstate_generator,ev, newstate_generator);
    }
    return;
}

/**
* Returns this union second-- Not used anymore
*/
void AFAut::Union(AFAut& second){
/*
	z3::context& ctx = mInit->mAMap.ctx();
	z3::expr trueexp = ctx.bool_val(true);
	AFAStatePtr newinit = new AFAState(OR,trueexp);
	SetAFAStatesPtr nextset;
	nextset.insert(mInit);
	newinit->mTransitions.insert(std::make_pair("0",nextset));
	nextset.clear();
	nextset.insert(second.mInit);
	newinit->mTransitions.insert(std::make_pair("0",nextset));
	std::set<SetAFAStatesPtr> rettrans = newinit->RecSaturateEpsilon();
	newinit->mTransitions.clear();
	BOOST_FOREACH(auto t, rettrans)
			newinit->mTransitions.insert(std::make_pair("0",t));
	//memeory leak here.. should I delete old inittial states if they were Or/AND??
	mInit=newinit;
*/
}

/**
* Returns this intersection secon-- Not used anymore
*/
void AFAut::Intersection(AFAut& second){
/*
	z3::context& ctx = mInit->mAMap.ctx();
	BOOST_ASSERT_MSG(ctx!=NULL,"Serious error");
	z3::expr trueexp = ctx.bool_val(true);
	AFAStatePtr newinit = new AFAState(AND,trueexp);
	//std::cout<<"Default value of accepting is "<<newinit->mIsAccepted<<std::endl;
		SetAFAStatesPtr nextset;
		nextset.insert(mInit);
		nextset.insert(second.mInit);
		newinit->mTransitions.insert(std::make_pair("0",nextset));
		std::set<SetAFAStatesPtr> rettrans = newinit->RecSaturateEpsilon();
		newinit->mTransitions.clear();
		BOOST_FOREACH(auto t, rettrans)
			newinit->mTransitions.insert(std::make_pair("0",t));
		//memory leak ehre .. should I delete old initial states if they were Or/And?
		mInit=newinit;
*/


}
/**
 * Print to dot file
 */
void AFAut::PrintToDot(std::string filename){
			 	    Graph graph;
			 	    std::map<AFAStatePtr,vertex_t,mapstatecomparator> mapindex;
			 	    mInit->PassThree(graph,mapindex);//fill the graph object

		  	    myEdgeWriter<Graph> ew(graph);
			 	    myVertWriter<Graph> vw(graph);
			 	    //IMPO we need to put writers after graph is filled.

		//	#ifdef DEBUG_FILE

			 	    std::ofstream outf(filename);
			 	    write_graphviz(outf, graph, vw,ew);

}
/**
 * Extract one accepted word of this AFA and returns that-- NOw not begin called.
 */
std::string AFAut::GetWord(){

	std::map<SetAFAStatesPtr,bool> seenset;
	std::set<std::pair<SetAFAStatesPtr,std::string>> workset;
	SetAFAStatesPtr initset;
	//Dont just put init in the initset if init is of type OR/AND but find all transitions on 0 from here
	if(mInit->mType==OR || mInit->mType==AND){
		std::set<std::string> transkeys  = mInit->getTransitionKeys();

		BOOST_ASSERT_MSG(transkeys.size()==1 && transkeys.find("0")!=transkeys.end(),"Some serious error as AND/OR must have only 0 labeled outgoing transitions");
		BOOST_FOREACH(auto t, mInit->mTransitions){
			workset.insert(std::make_pair(t.second,""));
			seenset.insert(std::make_pair(t.second,true));
		}
	}
	else
	{
		initset.insert(mInit);
		workset.insert(std::make_pair(initset,""));
		seenset.insert(std::make_pair(initset,true));
	}
	std::set<std::string> allsyms(mProgram->mAllSyms.begin(),mProgram->mAllSyms.end());
	while(!workset.empty())
	{
		SetAFAStatesPtr elemset = (workset.begin())->first;
		std::string word = (workset.begin())->second;
		workset.erase(workset.begin());
		bool allaccepting=true;
		BOOST_FOREACH(auto t, elemset){
			if(!(t->mIsAccepted)){
				allaccepting=false;
			}
		}
		if(allaccepting && (word.length()!=0)){
			return word;
		}

		for(std::set<std::string>::iterator it= allsyms.begin(); it!=allsyms.end(); it++){
			std::string sym = *it;
			std::set<SetAFAStatesPtr> tobeaddedset;
			SetAFAStatesPtr emptyset;
			tobeaddedset.insert(emptyset);
			for(SetAFAStatesPtr::iterator itset = elemset.begin(); itset!=elemset.end(); itset++)
			{
				AFAStatePtr state = *itset;
				BOOST_ASSERT_MSG(state->mTransitions.find("0")==state->mTransitions.end(),"Somer serious issue, as no transition should have label 0");
				BOOST_ASSERT_MSG(state->mTransitions.find(sym)!=state->mTransitions.end(),"Some serious issue, every state, after complement must have transition on every symbol");
						std::set<SetAFAStatesPtr> tmptobeaddedset;
						BOOST_FOREACH(auto singlesetSetStates, tobeaddedset){
							for(std::multimap<std::string, SetAFAStatesPtr>::iterator it= state->mTransitions.equal_range(sym).first; it!=state->mTransitions.equal_range(sym).second; it++){
								SetAFAStatesPtr curr = it->second;
								SetAFAStatesPtr tmp;
								tmp.insert(singlesetSetStates.begin(),singlesetSetStates.end());
								tmp.insert(curr.begin(),curr.end());
								tmptobeaddedset.insert(tmp);
							}
						}
						tobeaddedset.clear();
						tobeaddedset.insert(tmptobeaddedset.begin(),tmptobeaddedset.end());

			}
			BOOST_FOREACH(auto setstates, tobeaddedset){
				if(seenset.find(setstates)==seenset.end()){
					seenset.insert(std::make_pair(setstates,true));
					/*if(sym.compare("0")==0) NOW there will never be a zero on edges hence on sym of AFA.
							workset.insert(std::make_pair(setstates,(word)));
					else
					*/{
						std::string combined = word+sym;
						workset.insert(std::make_pair(setstates,combined));
					}
					}
			}
		}
	}
#ifdef	DBGPRNT
	std::cout<<"No accepted word found"<<std::endl;
#endif
	return "";//means only empty string is accepted here.
}

/**
 * Checks if the given string is accepted by this AFA or not.-- Now not called.
 */
bool AFAut::IsAccepted(std::string word){
	//We need seenset only to avoid self loopon nop symbol.. FOr that also we must keep word as well to check..
	std::multimap<SetAFAStatesPtr,std::string> seenset;
	std::set<std::pair<SetAFAStatesPtr,std::string>> workset;
	SetAFAStatesPtr initset;
	//Dont just put init in the initset if init is of type OR/AND but find all transitions on 0 from here
	if(mInit->mType==OR || mInit->mType==AND){
		std::set<std::string> transkeys  = mInit->getTransitionKeys();

		BOOST_ASSERT_MSG(transkeys.size()==1 && transkeys.find("0")!=transkeys.end(),"Some serious error as AND/OR must have only 0 labeled outgoing transitions");
		BOOST_FOREACH(auto t, mInit->mTransitions){
			workset.insert(std::make_pair(t.second,word));
			seenset.insert(std::make_pair(t.second,word));
		}
	}
	else
	{
		initset.insert(mInit);
		workset.insert(std::make_pair(initset,word));
		seenset.insert(std::make_pair(initset,word));
	}
	//IT is interesting to see that there is a huge similarity between this function and getword function of this class
	//Only difference being that here we only iterate on symbol trying to match and 0 while in getword we iterate over
	//all symbols include 0.
	while(!workset.empty())
	{
		SetAFAStatesPtr elemset = (workset.begin())->first;
		std::string word = (workset.begin())->second;

/*
		std::cout<<"Word to search is "<<word<<std::endl;
		std::cout<<"From states ";
		BOOST_FOREACH(auto t, elemset)
			std::cout<<t->mAMap<<",accepting="<<t->mIsAccepted;
		std::cout<<std::endl;
*/

		workset.erase(workset.begin());
		bool allaccepting=true;
		BOOST_FOREACH(auto t, elemset){
			if(!(t->mIsAccepted))
				allaccepting=false;
		}
		if(allaccepting && word.length()==0)
			return true;
		//NOte that this word can never contain 0 because the word accepted is also not accepting 0..
		//but our AFA can contain 0 and hence it must consider matching 0 derivatives with non consumed rest of the string
		//Otherwise we will not be able to handle things..
		if(word.length()!=0)
		{
			std::string sym(1,word.at(0));
			std::string restofword(word.substr(1));
			std::set<std::string> allsyms;
			allsyms.insert(sym);
			for(std::set<std::string>::iterator it= allsyms.begin(); it!=allsyms.end(); it++){
					std::string sym = *it;
					std::set<SetAFAStatesPtr> tobeaddedset;
					SetAFAStatesPtr emptyset;
					tobeaddedset.insert(emptyset);
					for(SetAFAStatesPtr::iterator itset = elemset.begin(); itset!=elemset.end(); itset++)
					{
						AFAStatePtr state = *itset;
/*
						std::cout<<"Checking transitions on "<<sym<<" from "<< state<<std::endl;
						BOOST_FOREACH(auto w,state->mTransitions)
						std::cout<<w.first<<",";
						std::cout<<std::endl;
*/
						BOOST_ASSERT_MSG(state->mTransitions.find("0")==state->mTransitions.end(),"Somer serious issue, as no transition should have label 0");
						BOOST_ASSERT_MSG(state->mTransitions.find(sym)!=state->mTransitions.end(),"Somer serious issue, as after complementation (which it is) every state must have ");
//							std::cout<<"checking for "<<sym<<" inside"<<std::endl;
								std::set<SetAFAStatesPtr> tmptobeaddedset;
								BOOST_FOREACH(auto singlesetSetStates, tobeaddedset){
									for(std::multimap<std::string, SetAFAStatesPtr>::iterator it= state->mTransitions.equal_range(sym).first; it!=state->mTransitions.equal_range(sym).second; it++){
										SetAFAStatesPtr curr = it->second;
										SetAFAStatesPtr tmp;
										tmp.insert(singlesetSetStates.begin(),singlesetSetStates.end());
										tmp.insert(curr.begin(),curr.end());
										tmptobeaddedset.insert(tmp);
									}
								}
								tobeaddedset.clear();
								tobeaddedset.insert(tmptobeaddedset.begin(),tmptobeaddedset.end());

						}

					BOOST_FOREACH(auto setstates, tobeaddedset){
						if(seenset.find(setstates)==seenset.end()){
							//means not seen already, add to seenset and workingset
							seenset.insert(std::make_pair(setstates,restofword));
							workset.insert(std::make_pair(setstates,restofword));

							} else if(seenset.find(setstates)!=seenset.end())//present
							{
								//now check if the word is same as the one being added here..
								std::set<std::string> sset;
	for(std::multimap<SetAFAStatesPtr,std::string>::iterator it = seenset.equal_range(setstates).first; it!=seenset.equal_range(setstates).second; it++)
	{
		sset.insert(it->second);
	}
								if(sset.find(restofword)==sset.end()){//means not in the set of string for this setstates.
//									std::cout<<"added"<<std::endl;
									seenset.insert(std::make_pair(setstates,restofword));
									workset.insert(std::make_pair(setstates,restofword));
								}
							}
					}
				}
		}
		//returning false immediately here does not make sense unless we have explored all possible cases..
	}
#ifdef	DBGPRNT
	std::cout<<"WOrd not found"<<std::endl;
#endif
 return false;//means this word was not accepted
}

typedef std::tuple<faudes::Idx,std::set<AFAStatePtr>> GenAFATuple;
struct GenAFAComparator{
	bool operator()(const GenAFATuple& one, const GenAFATuple& two) const{
		if(std::get<0>(one)==std::get<0>(two))
			return (std::get<1>(one)<std::get<1>(two));
		else
			return (std::get<0>(one)<std::get<0>(two));
	}
};

void AFAut::Intersection(faudes::Generator& rGen, faudes::Generator& rRes)
{

	std::map<GenAFATuple, faudes::Idx, GenAFAComparator> seenmap;
	std::set<GenAFATuple, GenAFAComparator> workset;

	//Insert events in rRes..
	BOOST_FOREACH(auto sym, mProgram->mAllSyms)
		rRes.InsEvent(sym);
	//get initial states of afa and mix with init states of Gen to create new tuples..
	if(mInit->mType==ORLit){
		 for(faudes::StateSet::Iterator lit = rGen.InitStatesBegin(); lit != rGen.InitStatesEnd(); ++lit)
			  {
				  faudes::Idx state=*lit;
				  std::set<AFAStatePtr> setnxt;
				  setnxt.insert(mInit);
				  GenAFATuple entry = std::make_tuple(state,setnxt);
				  faudes::Idx stid = rRes.InsState();
				  rRes.SetInitState(stid);
				  seenmap.insert(std::make_pair(entry,stid));
				  workset.insert(entry);
			  }
	}else{
	  for(faudes::StateSet::Iterator lit = rGen.InitStatesBegin(); lit != rGen.InitStatesEnd(); ++lit)
	  {
		  faudes::Idx state=*lit;
		  BOOST_FOREACH(auto trans, mInit->mTransitions){
		  	BOOST_ASSERT_MSG(trans.first=="0","Some serious error");
		  	std::set<AFAStatePtr> setnxt(trans.second.begin(),trans.second.end());
		  	GenAFATuple entry = std::make_tuple(state,setnxt);
		  	faudes::Idx stid = rRes.InsState();
		  	rRes.SetInitState(stid);
		  	seenmap.insert(std::make_pair(entry,stid));
		  	workset.insert(entry);
		  }
	  }
	}
	///Now iterate over workset till it becomes empty..
	while(!workset.empty())
	{
		GenAFATuple thistuple = *(workset.begin());
		workset.erase(workset.begin());
		BOOST_ASSERT_MSG(seenmap.find(thistuple)!=seenmap.end(),"Some serious issue, this element must be present");
		faudes::Idx mappedthistuple = seenmap.find(thistuple)->second;

		faudes::Idx gensrc = std::get<0>(thistuple);
		std::set<AFAStatePtr> afastates = std::get<1>(thistuple);
		//if genstate is marked and all states in afastaes are accepting then mark mappedthistuple as marked..
		bool allacc=true;
		BOOST_FOREACH(auto st, afastates){
			if(!st->mIsAccepted){
				allacc=false; break;
			}
		}
		if(allacc && rGen.MarkedStates().Find(gensrc)!=rGen.MarkedStatesEnd())
			rRes.SetMarkedState(mappedthistuple);


		//find transitions out of genstate.. and iterate over them..
		for(faudes::TransSet::Iterator lit = rGen.TransRelBegin(gensrc);lit!=rGen.TransRelEnd(gensrc);lit++)
		{
			std::string sym(rGen.EventName(lit->Ev));
			faudes::Idx gendest = lit->X2;
			//return the set of transitions from afastates on sym
			std::set<std::set<AFAStatePtr>> res = GetConjunctedTransitions(afastates,sym);
			BOOST_FOREACH(auto trans, res)
			{
				//preapare a new tuple with gendest and trans..
				GenAFATuple entry = std::make_tuple(gendest,trans);
				faudes::Idx residx;
				if(seenmap.find(entry)==seenmap.end())
				{
					residx = rRes.InsState();
					seenmap.insert(std::make_pair(entry,residx));
					workset.insert(entry);
				}else
				{
					residx = seenmap.find(entry)->second;
				}
				//add a transition from mappedthistuple to residx
				rRes.SetTransition(mappedthistuple,lit->Ev,residx);
			}
		}

	}
#ifdef DBGPRNT
	rGen.DotWrite("Original.dot");
	this->PrintToDot("subtracted.dot");
	std::cout<<"After Intersection before coacc: States = "<<rRes.States().Size()<<" trans = "<<rRes.TransRel().Size()<<std::endl;
#endif
	rRes.Coaccessible();//make it coaccessible..
#ifdef DBGPRNT
	std::cout<<"After Intersection after coacc: States = "<<rRes.States().Size()<<" trans = "<<rRes.TransRel().Size()<<std::endl;
	rRes.DotWrite("After Intersection.dot");
#endif
}


/**
 * Commenting out this method as it is no longer used. Also it helps us to avoid using fa.h
 * @return
 */
/*struct fa* AFAut::ConvertToNFA(){
	struct fa* nfa = fa_make_empty();
	std::map<SetAFAStatesPtr, struct autstate*> seenmap;
	std::set<SetAFAStatesPtr> workset;
	struct autstate* init;
	std::set<std::tuple<struct autstate*,struct autstate*>> epsilonset;
	if(mInit->mType!=ORLit)
	{
		init=add_autstate(nfa,0);
		BOOST_FOREACH(auto t, mInit->mTransitions){
			if(seenmap.find(t.second)!=seenmap.end()){
//				add_epsilon_trans(init,seenmap.find(t.second)->second);
				//do nothing as epsilon must have been added before putting it here from init (next block)
			}else{
				//create a new state..
				struct autstate* newst = add_autstate(nfa,0);
				seenmap.insert(std::make_pair(t.second,newst));
				workset.insert(t.second);
				epsilonset.insert(std::make_pair(init,newst));
				//add_epsilon_trans(init,newst); we cant add epsilon directly only after every thing has been added..
			}

		}
	}else{
		SetAFAStatesPtr newstset;
		newstset.insert(mInit);
		//create a new state..
		struct autstate* newst = add_autstate(nfa,0);
		seenmap.insert(std::make_pair(newstset,newst));
		workset.insert(newstset);
		init=newst;
	}


	nfa->initial=init;
	while(!workset.empty()){
		//dont forget to put some states as accepting states as well.
		SetAFAStatesPtr setstates = *(workset.begin());
		workset.erase(workset.begin());
		//if it is in workset it means some autstate* must have been assigned to this set of afa states..
#ifdef	DEBUGSTMTS
		BOOST_ASSERT_MSG(seenmap.find(setstates)!=seenmap.end(),"Some serious issue as by this time it must have been added here");
#endif
		std::set<std::string> allsyms(mProgram->mAllSyms.begin(),mProgram->mAllSyms.end());
		struct autstate* curr = seenmap.find(setstates)->second;
		//check if this can be set as an accepting state.. by checking every state in setstates is accepting..
		bool allaccepting=true;
		BOOST_FOREACH(auto t, setstates){
				if(!(t->mIsAccepted)){
						allaccepting=false;
					}
		}
		if(allaccepting)
			curr->accept=1;//set it as accepting state.

		BOOST_FOREACH(auto sym, allsyms){
			std::set<SetAFAStatesPtr> tobeaddedset;
			SetAFAStatesPtr emptyset;
			tobeaddedset.insert(emptyset);
			BOOST_FOREACH(auto state, setstates){
				if(state->mTransitions.find(sym)==state->mTransitions.end()){
					tobeaddedset.clear();//This is necessary to avoid executing the loop with partially filled tobeaddedset
					break;//means now we can not proceed to have an outgoing edge on this  symbol because at least one
					//does not have that..
				}
				std::set<SetAFAStatesPtr> tmptobeaddedset;
				BOOST_FOREACH(auto w, tobeaddedset){

					for(std::multimap<std::string, SetAFAStatesPtr>::iterator it= state->mTransitions.equal_range(sym).first; it!= state->mTransitions.equal_range(sym).second; it++)
					{
						SetAFAStatesPtr tmp = it->second;
						SetAFAStatesPtr tobeadded(w.begin(),w.end());
						tobeadded.insert(tmp.begin(),tmp.end());
						tmptobeaddedset.insert(tobeadded);
					}

				}
				tobeaddedset.clear();
				tobeaddedset.insert(tmptobeaddedset.begin(),tmptobeaddedset.end());

			}
			//now we have tobeaddedset ready.. for sym on this state transitions are to these states..
			BOOST_FOREACH(auto setst, tobeaddedset){
				if(seenmap.find(setst)!=seenmap.end()){
					struct autstate* dest = seenmap.find(setst)->second;
					add_new_auttrans(curr,dest,sym.at(0),sym.at(0));
				}else{
					//create a new aut state..
					struct autstate* newdest = add_autstate(nfa,0);
					//put to seenmap
					seenmap.insert(std::make_pair(setst,newdest));
					//put to worklist
					workset.insert(setst);
					add_new_auttrans(curr,newdest,sym.at(0),sym.at(0));
				}
			}

		}

	}

	BOOST_FOREACH(auto t, epsilonset){
		add_epsilon_trans(std::get<0>(t),std::get<1>(t));//this will trigger a saturation
	}

	return nfa;


}*/

bool AFAut::IsUnsafe(){
	z3::expr wp(*(mInit->mHMap));
	z3::expr falseformula = mInit->mAMap.ctx().bool_val(false);
	if(eq(wp,falseformula))
		return false;
	else
		return true;
}

AFAut::~AFAut() {
	//This destructor will traverse over the automaton starting from init and store the set of AFA states in a set.
	//after completing the traversal it calls delete on each one of them.
	std::map<AFAStatePtr,bool> seenset;
	SetAFAStatesPtr workset;
	if(mInit!=NULL){


			workset.insert(mInit);
			seenset.insert(std::make_pair(mInit,true));
			while(!workset.empty())
			{
				AFAStatePtr st = *(workset.begin());
				workset.erase(workset.begin());
				BOOST_FOREACH(auto v, st->mTransitions )
				{
					BOOST_FOREACH(auto t, v.second){
						if(seenset.find(t)==seenset.end())//means not seen already.. add to seenset and workset
						{
							seenset.insert(std::make_pair(t,true));
							workset.insert(t);
						}
					}
				}
			}
			//when finished.. call destructor on every key of seenset
			BOOST_FOREACH(auto t, seenset){
				delete t.first;
			}
	}
}
