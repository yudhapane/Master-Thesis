// $Id$


#include"PGBasics.hh"
#include"Sampler.hh"
#include"ThreeState.hh"
#include<sstream>

using namespace std;
using namespace libpg;

// Set up three state model including stochastic trans matricies for each 
ThreeState::ThreeState() {

    actionDim = ACTION_DIM; // Simple model, 1
    obsRows = OBS_DIM;
    obsCols = 1;
    actions = ACTIONS;
    agents = 1;

    stateTrans = new Matrix[ACTIONS];

    // The transition and observation matrices
    istringstream data("[3,3]((0.2, 0.8, 0.0),(0.8, 0.0, 0.2),(0.2, 0.8, 0.0)) \
                        [3,3]((0.8, 0.2, 0.0),(0.2, 0.0, 0.8),(0.8, 0.2, 0.0)) \
                        [2,3]((0.667, 0.333, 0.2777),(6, 12, 5))");
    for (int a=0; a < ACTIONS; a++) {
	data>>stateTrans[a];
	cout<<"Matrix from raw :"<<a<<": "<<stateTrans[a]<<endl;
    }
    
    data>>allObs;
    cout<<"Observations: "<<allObs<<endl;

    rewards.resize(STATES);
    rewards(0) = 1;
    rewards(1) = -1;
    rewards(2) = 8;

    currState = random()%STATES;

}


// Return reward direct from reward vector 
// INPUT: vector to put the
// local rewards in. In this case, there's no local rewards so we put
// the same thing in the first element of the vector that we would
// otherwise return for the global reward.
void ThreeState::getReward(Vector& rewards) {

    assert(rewards.size() == 1);
    rewards[0] = this->rewards[currState];

}


// INPUT: matrix structre, 1x1, to return int observation in
void ThreeState::getObservation(Observation& obs) {

    column(obs.getFeatures(), 0) = column(allObs, currState);
//    obs.getFeatures()(0,0) = currState;

}


// Do the action, core of the simulator
// INPUT: action vector which will always be 1x1 in this case.
int ThreeState::doAction(Vector& action) {

    Vector stateTransRow(STATES);
    int a = (int)action[0];

    assert(a < ACTIONS);

    // Extract the correct row of the state transition matrix
    stateTransRow = row(stateTrans[a], currState);

    // Update the state randomly. We know the CDF is 1.0
    currState = Sampler::discrete(stateTransRow, 1.0);

    return 0;
}


// Maximum episode length in steps. For planning mode. Only used
// by GOALPomdp at the moment.
int ThreeState::getMaxEpisodeLength() {
    return 0;
}

// Get the number of rows in observation matrix. Generally the
// length of the observation vector.
int ThreeState::getObsRows() {
    return obsRows;
}

// Get the number of columns in the observation matrix Typically
// use multiple columns to provide a different observation to each
// agent, so the number of columns is the same as getAgents()
int ThreeState::getObsCols() {
    return obsCols;
}

// Get the total number of agents. Usually 1.
int ThreeState::getAgents() {
    return agents;
}

// Get the total action dimensionality. Often, but not
// necessarily, the same as the number of agents.
int ThreeState::getActionDim() {
    return actionDim;
}

/**
 * Return the number of actions the simulator knows about
 */
int ThreeState::getNumActions() {
    return ACTIONS;
}


// Get the dimensionality of the reward. Typicall 1 if there's a single
// Global reward.
int ThreeState::getRewardDim() {		
    return REWARD_DIM;
}
