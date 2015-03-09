/*
 *  BeliefStateCassandra.cc
 *  
 *
 *  Created by Doug Aberdeen.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "PSRCassandra.hh"

using namespace std;
using namespace libpg;

PSRCassandra::PSRCassandra (Cassandra& c) : 
    Cassandra (c.actionTransitions, c.actionObservations,  c.rewards,
	       c.numActions, c.numStates, c.numObservations, c.discount)
    
{
    
    discoverPSR();

    //init belief state -- curr state.
    belief.resize(coreTests.size());
    belief.clear ();
    
    belief[0] = 1.0;
    // Equivalent of uniform belief for PSRs.
    /*
    for (TestsMap::iterator q=coreTests.begin(); q != coreTests.end(); q++) {
	belief[q->second] = 1.0/q->first.length();
    }
    */
    stateTransRow = new Vector (numStates);
    observationRow = new Vector (numObservations);
    btp1.resize(coreTests.size());

    
}


int PSRCassandra::doAction (int action) {
	assert (action < (int)numActions);
	
	currAction = action;
	
	//update state.
	lastState = currState;

	noalias (*stateTransRow) = row (actionTransitions [currAction], lastState);
	currState = Sampler::discrete ((*stateTransRow), 1.0);

	noalias (*observationRow) = row (actionObservations [currAction], currState);
	
	currObservation = Sampler::discrete ((*observationRow) , 1.0);


	
	//cout << "start belief state generation." << endl;
	//cout << "action: " << action << endl;
	//cout << "Initial Belief: " << (*belief) << endl;
	
	// Update the PSR vector

	
	// Column tests.
	string prefix;
	prefix.push_back((char)(currAction + '0'));	
	prefix.push_back((char)(currObservation + '0'));

	Vector norm(1);

	
	// See how accurate we were
/*
	norm=prod(belief, oneStepParams[prefix]);
	double prob;
	prob = norm[0];
	cout<<"Predicted real action/obs="<<currAction<<","<<currObservation<<" = "<<prob<<endl;
*/

	// Gives a vector
	noalias(btp1) = prod(belief, coreParams[prefix]);
	norm=prod(belief, oneStepParams[prefix]);
	belief = btp1/norm[0];
	
	return 0;
}


/** 
 * Discover exact PSR core tests and params 
 */
void PSRCassandra::discoverPSR() {

    cout<<"Entering discovery\n";
    // Do depth first search looking for core tests

    // U is initially empty test prediction=column vec of 1's
    // plus space for an extra core test.

    Matrix U(numStates, 2); 
    U.assign(scalar_matrix<double>(numStates, 2, 1.0));

    // Column 0 is empty test.
    coreTests[""] = 0;
	
    string test("");

    recursePSRDiscovery(test, U);

    // At this point we have one too many columns in U
    // Reduce down to core test size. 
    cout<<"U before:"<<U<<endl;
    U.resize(numStates, coreTests.size(), true);
    cout<<"U after:"<<U<<endl;

    // Compute paramters for each core test and one step extension.
    cout<<"Core tests:\n";
    for (TestsMap::iterator q=coreTests.begin(); q != coreTests.end(); q++) {
	cout<<"\t"<<q->first<<endl;
    }


    // All core test params for single step
    Matrix Mao(coreTests.size(), coreTests.size());
    // single step params
    Matrix mao(coreTests.size(), 1); 

    Matrix  O(numStates, numStates);
    O.clear();

    // Get pseudo invese of U
    Matrix UInv(coreTests.size(), numStates);
    UBlasExtras::pseudoInverse(U, UInv);

    Matrix UInvT(coreTests.size(), numStates);
    Matrix UInvTO(coreTests.size(), numStates);
 
    // Loop over one step prefix extensions
    // computing PSR parameters
    for (size_t a=0; a < numActions; a++) {
	for (size_t o=0; o < numObservations; o++) {
	    string prefix;
	    prefix.push_back((char)(a + '0'));
	    prefix.push_back((char)(o + '0'));

	    cout<<"Computing params for a="<<a<<" o="<<o<<" '"<<prefix<<"'"<<endl;
	    for (size_t s=0; s < numStates; s++) {
		// Prob of action o, in state s, given action a
		O(s,s) = (actionObservations[a])(s, o); 
	    }

	    
	    axpy_prod(UInv, actionTransitions[a], UInvT, true); // clear first
	    axpy_prod(UInvT, O, UInvTO, true);
	    axpy_prod(UInvTO, U, Mao, true); // Find core parameters
	    // Find step parametersa
	    axpy_prod(UInvTO, scalar_matrix<double>(numStates, 1, 1.0), mao, true); 
	    
	    // Copy to appropriate structures
	    coreParams[prefix] = Mao;
	    oneStepParams[prefix] = mao;

	    //cout<<"Mao for '"<<prefix<<"' = "<<coreParams[prefix]<<endl;
	    //cout<<"mao for '"<<prefix<<"' = "<<oneStepParams[prefix]<<endl;

	}
    }    

}


/**
 * Depth first search for new core tests. 
 */
bool PSRCassandra::recursePSRDiscovery(string test, Matrix& U) {

    cout<<"recursing discovery for '"<<test<<"'\n";
    Matrix  O(numStates, numStates);
    O.clear();
    string candidate;


    // Loop over one step prefix extensions
    for (size_t a=0; a < numActions; a++) {
	for (size_t o=0; o < numObservations; o++) {
	    
	    candidate.clear();
	    // Construct candidate core test string
	    candidate.push_back((char)(a + '0'));
	    candidate.push_back((char)(o + '0'));
	    candidate += test;

	    cout<<"\tCreated candidate '"<<candidate<<"'\n";
	    // Construct diagonal observation matrix for obs o, and action a
	    for (size_t s=0; s < numStates; s++) {
		// Prob of action o, in state s, given action a
		O(s,s) = (actionObservations[a])(s, o); 
	    }
	    cout<<"\tCreated diag matrix for '"<<candidate<<"'\n";

	    // Update so called "outcome" vector
	    Vector tmp(numStates);
	    noalias(tmp) = prod(O, column(U, coreTests[test]));
	    cout<<"\tstep 2\n";
	    column(U, coreTests.size()) = prod(actionTransitions[a], tmp);

	    // Check rank of new U matrix. If we added a linearly
	    // *DE*pendent vector then the rank will be
	    // coreTests.size() But if it's one more than
	    // coreTests.size() we have found a new core test and it's
	    // time to recurse the search.
	    cout<<"\tDoing SVD\n";
	    size_t rank = UBlasExtras::rank(U);
	    cout<<"\tRank="<<rank<<"\n";
	    assert(rank <= U.size2()); // Rank less than smallest dim
	    assert(rank <= numStates); // Something has gone seriously wrong!
	    
	    // We found a new core test!
	    if (rank > coreTests.size()) {
		cout<<"Adding '"<<candidate<<"' to core tests\n";
		int pos = coreTests.size();
		coreTests[candidate] = pos;
		// Add space for a new core test, not touching
		// existing data.
		U.resize(numStates, coreTests.size() + 1, true);
		recursePSRDiscovery(candidate, U);
	    }
	    else if (rank < coreTests.size()) {
		cout<<"Something has gone wrong checking '"
		    <<candidate<<"', rank="
		    <<rank
		    <<" less than current core test set\n";
		//throw std::runtime_error("PSR Discovery failed");
	    }

	}
    }

    return true;

}


Vector PSRCassandra::getBelief () {
    return belief;
}


void PSRCassandra::getObservation (Observation& obs) {
    for(size_t i = 0; i < coreTests.size(); i++) {
	obs.getFeatures() (i,0) = belief[i];
    }
    if (obs.getSteps()%10000==0) cout<<"Obs: "<<obs.getFeatures()<<endl;
}


int PSRCassandra::getObsRows () {
    return coreTests.size();
}
