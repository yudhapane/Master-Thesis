/*
 *  BeliefStateCassandra.cc
 *  
 *
 *  Created by Owen Thomas on 23/06/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "BeliefStateCassandra.hh"

using namespace libpg;

int BeliefStateCassandra::doAction (int action) {
	assert (action < (int)numActions);
	
	currAction = action;
	
	//update state.
	lastState = currState;
	//Vector stateTransRow (numStates);
	noalias (*stateTransRow) = row (actionTransitions [currAction], lastState);
	currState = Sampler::discrete ((*stateTransRow), 1.0);
	
	//update observation.
	//Vector observationRow (numObservations);
	noalias (*observationRow) = row (actionObservations [currAction], currState);
	
	currObservation = Sampler::discrete ((*observationRow) , 1.0);
	
	//cout << "start belief state generation." << endl;
	//cout << "action: " << action << endl;
	//cout << "Initial Belief: " << (*belief) << endl;
	
	
	btp1->clear ();
	
	for (unsigned int i = 0; i < numStates; i++) {
		for(unsigned int j = 0; j < numStates; j++) {
			(*btp1) (j) += (*belief)(i)*actionTransitions [currAction] (i,j);
		}
	}
	
	for(unsigned int i = 0; i < numStates; i++) {
		(*belief) (i) = (*btp1) (i) * actionObservations [currAction] (i, currObservation);
	}
	
	(*belief) /= norm_1 (*belief);
	
	return 0;
}

