/*
 *  Cassandra.cc
 *  
 *
 *  Created by Owen Thomas on 16/06/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "Cassandra.hh"

using namespace std;
using namespace libpg;

Cassandra::Cassandra (SparseMatrix* actionTransitions,
			SparseMatrix* actionObservations,
			SparseMatrix** rewards,
			int numActions, int numStates, 
			int numObservations, float discount) {

	init (actionTransitions, actionObservations, rewards,numActions,numStates,
		numObservations, (random()%numStates),discount);
}

Cassandra::Cassandra (SparseMatrix* actionTransitions,
			SparseMatrix* actionObservations,
			SparseMatrix** rewards,
			int numActions, int numStates, 
			int numObservations, int startState, float discount) {

	init (actionTransitions, actionObservations, rewards,numActions,numStates,
		numObservations, startState,discount);
}


void Cassandra::init (SparseMatrix* actionTransitions,
			SparseMatrix* actionObservations,
			SparseMatrix** rewards,
			int numActions, int numStates, 
			int numObservations, int startState, float discount)
{
	this->actionTransitions = actionTransitions;
	this->actionObservations = actionObservations;
	this->rewards = rewards;
	
	this->numActions = numActions;
	this->numStates = numStates;
	this->numObservations = numObservations;
	
	this->discount = discount;
	
	this->currState = startState;
	
	//As each observation depends upon the execution of
	//an action, we can have no initial observation. 
	lastState = currObservation = currAction = 0;
	
	this->cheat = false;
}

int Cassandra::doAction (Vector& action) {
	return doAction ( (int)action [0]);
}

int Cassandra::doAction (int a) {
	assert (a < (int)numActions);
	
	currAction = a;
	
	Vector stateTransRow (numStates);
	stateTransRow = row (actionTransitions [currAction], currState);

	lastState = currState;
	currState = Sampler::discrete (stateTransRow, 1.0);
	
	Vector observationRow (numObservations);
	observationRow = row (actionObservations [currAction], currState);
	
	if(!cheat)
		currObservation = Sampler::discrete (observationRow, 1.0);
	else
		currObservation = (currState / numStates) * numObservations;
		
//	cout << "[" << currAction << "," << lastState << "," << currState << "," << currObservation << "," << getReward () << "]" << endl;
	//cerr << "[" << currAction << "," << lastState << "," << currState << "," << currObservation << "," << getReward () << "]" << endl;
	return 0;
}

void Cassandra::getObservation(Observation& obs) {
	//Is this right??
	if(!cheat)
		obs.getFeatures() (0,0) = currObservation;
	if(cheat)
		obs.getFeatures() (0,0) = currState;
}

void Cassandra::getReward (Vector& rewards) {
	rewards [0] = getReward();
}

int Cassandra::getReward () {
	
		return (this->rewards [currAction] [lastState]) 
			(currState, currObservation);
	
			
}

void Cassandra::print () {
	cout << "numActions: " << numActions << endl;
	cout << "numStates: " << numStates << endl;
	cout << "numObservations: " << numObservations << endl;
	cout << "discount: " << discount << endl;
	
	cout << "obsRows: " << this->getObsRows () << endl;
	cout << "obsColumns: " << this->getObsCols() << endl;
	cout << "Action Transitions: " << endl;
	for(unsigned int i = 0; i < numActions; i++) {
		cout << i << ":" << actionTransitions [i] << endl;
	}
	cout << "Action Observations: " << endl;
	for(unsigned int i = 0; i < numActions; i++) {
		cout << i << ":" << actionObservations [i] << endl;
	}
	cout << "Rewards: " << endl;
	for(unsigned int i = 0; i < numActions; i++) {
		for(unsigned int j = 0; j < numStates; j++) {
			cout << i <<"," << j  << ":" << rewards [i] [j]<< endl;
		}
	}
}


void Cassandra::setCheat (bool cheat) {
	cout << "set cheat: " << cheat << endl;
	this->cheat = cheat;
}
