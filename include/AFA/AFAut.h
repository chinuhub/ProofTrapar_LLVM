/*
 * AFAut.h
 *
 *  Created on: 23-Aug-2015
 *      Author: jaganmohini
 */

#ifndef AFA_AFAUT_H_
#define AFA_AFAUT_H_
#include "AFA/AFAState.h"
#include "Program.h"
#include <string>
#include <set>
#include <boost/assert.hpp>
#include <algorithm>
/*extern "C"
{
#include <fa.h>
}*/
#include <libfaudes.h>
class AFAut {
private:
	// Stores the initial state of AFA.

	//static AFAStatePtr RecMakeAFAutFromFA(struct autstate* state, std::map<struct autstate*, AFAStatePtr>&,z3::context& );
public:
	AFAut(){}
	AFAStatePtr mInit;


	bool IsUnsafe();
	static Program* mProgram;
	/**
	 * Function to convert an NFA/DFA to an AFA
	 */
	//static AFAut* MakeAFAutFromFA(struct fa* nfa,Program* program,z3::context&);

	static void MakeAFAutProof(std::string& word, z3::expr& mPhi,Program* p, int count, bool& bres, faudes::Generator& generator);
	struct fa* ConvertToNFA();
	/**
	 * Complement this AFA and changes init accordingly
	 */
	AFAStatePtr Complement(AFAStatePtr);
	void RecComplement(AFAStatePtr , std::map<AFAStatePtr,bool>& , SetAFAStatesPtr,AFAStatePtr);
	/**
	 * Returns this union second
	 */
	void Union(AFAut& second);

	/**
	 * Returns this intersection secon
	 */
	void Intersection(AFAut& second);
	void Intersection(faudes::Generator& , faudes::Generator& );

	void PrintToDot(std::string filename);
	/**
	 * Extract one accepted word of this AFA and returns that
	 */
	std::string GetWord();

	void PassFourNew(AFAStatePtr init, std::set<AFAStatePtr>& allStates , int, faudes::Generator&, std::map<z3::expr, bool,mapexpcomparator>&);

	std::set<std::set<AFAStatePtr>> GetConjunctedTransitions(std::set<AFAStatePtr> stateset, std::string sym);
	std::set<AFAStatePtr> GetDisjunctedTransitions(std::set<AFAStatePtr> stateset, std::string sym);

    std::map<std::string, AFAStatePtr> getImplicitTransitions(AFAStatePtr state);
    std::map<std::string, SetAFAStatesPtr> getExplicitTransitions(AFAStatePtr state);
	/**
	* Checks if the given word is accepted by this AFA or not
	*/
	bool IsAccepted(std::string word);

	void createDFA( faudes::Generator& generator);
	virtual ~AFAut();

    void addSelfLoop(faudes::Generator &generator,
                std::map<AFAStatePtr, std::map<std::string, AFAStatePtr>> &implicittransset,
                const SetAFAStatesPtr &tmpNextStatesSet, faudes::Idx newstate_generator);

    std::pair<StateType, std::set<SetAFAStatesPtr>> EpsilonClosure(AFAStatePtr state);

    void ConvertToEpsilonConsecutiveSameAMap(AFAStatePtr state);

    static AFAut* MakeAFAutFromDot(std::string filename);
};


#endif /* AFA_AFAUT_H_ */
