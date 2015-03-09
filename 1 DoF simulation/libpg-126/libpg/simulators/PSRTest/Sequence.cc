// $Id$


#include"PGBasics.hh"
#include"Sequence.hh"

#define SEQ_NOISE 10
int Sequence::sequence[] = {0, 1, 2, 1, 2, 3};
int Sequence::seqLen = 6;

using namespace libpg;


// Set up three state model including stochastic trans matricies for each 
Sequence::Sequence() {

    actionDim = ACTION_DIM; // Simple model, 1
    obsRows = OBS_DIM;
    obsCols = 1;
    actions = ACTIONS;
    agents = 1;

    reward=0;
}


void Sequence::getReward(Vector& rewards) {

    rewards[0] = reward;

}



void Sequence::getObservation(Observation& obs) {

    int obsSymbol = sequence[obs.getSteps()%seqLen];
    if (random()%SEQ_NOISE == 0) obsSymbol = random()%SYMBOLS;
    obs.getFeatures()(0,0) = obsSymbol;

    nextObs = sequence[(obs.getSteps()+1)%seqLen];

}


int Sequence::doAction(Vector& action) {
    // Rewards for predicing next observation in sequence
    if (action[0] == nextObs) reward=1.0;
    else reward=0.0;
    return 0;
}


// Maximum episode length in steps. For planning mode. Only used
// by GOALPomdp at the moment.
int Sequence::getMaxEpisodeLength() {
    return 0;
}

// Get the number of rows in observation matrix. Generally the
// length of the observation vector.
int Sequence::getObsRows() {
    return obsRows;
}

// Get the number of columns in the observation matrix Typically
// use multiple columns to provide a different observation to each
// agent, so the number of columns is the same as getAgents()
int Sequence::getObsCols() {
    return obsCols;
}

// Get the total number of agents. Usually 1.
int Sequence::getAgents() {
    return agents;
}

// Get the total action dimensionality. Often, but not
// necessarily, the same as the number of agents.
int Sequence::getActionDim() {
    return actionDim;
}


// Get the dimensionality of the reward. Typically 1 if there's a single
// Global reward.
int Sequence::getRewardDim() {		
    return REWARD_DIM;
}
