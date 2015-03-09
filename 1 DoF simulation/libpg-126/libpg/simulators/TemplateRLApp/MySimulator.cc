// This is your first RL application. Fill in the blanks shown below
// and in MySimulator.hh. This file is the definition of your control
// problem for reinforcement learning to optimise. You'll probably
// also have to edit TemplateRLApp.cc to get the best performance, but
// your first goal is to fill in the blanks below, compile, and run.

// NOTICE: This template assumes a single RL agent. You should be able
// to figure out what to modify here and in TemplateRLApp.cc to make
// it multi-agent.

// Look for the text [WRITE CODE HERE] to see where to fill stuff in.

#include"PGBasics.hh"
#include"MySimulator.hh"

// You can use normal C++ symbols
using namespace std;

// You can access the libpg library
using namespace libpg;

/**
 * Constructor for your simulator. Do any once off
 * initialisation. Read data files, allocate memory, stuff like that.
 * Can be left empty if need be.
 */
MySimulator::MySimulator() {

    // [WRITE CODE HERE]

}



/**
 * Return the reward for the current state, or previous-state and action combination.
 * In the multi-agent case there is one reward per reward. Here we assume a single agent, so only
 * a single scalar reward. It's often easier to compute the reward during doAction() and save
 * it a class variable to return here.
 * @param rewards vector to put the rewards in. 
 */
void MySimulator::getReward(Vector& rewards) {

    // Check single agent assumption
    assert(rewards.size() == 1);

    // Initialise reward
    double reward = 0;

    // [WRITE CODE HERE]
    // reward = ...

    // Insert scalar reward into return vector
    rewards[0] = reward;

}


/**
 * Fill in a description of the current state. Think of the
 * observation for a single agent as being a single column matrix of
 * dimension getObsRows()*1. 
 *
 * An observation is sometimes refered to as a basis feature.
 *
 * It's up to you to define below in
 * getObsRows() how big the observation is. Making it really big
 * can degrade performance because more parameters are required.
 * Choose your observation features carefully.  
 *
 * One tip is to try and normalise each element of your observation
 * to be between -1 and 1. Another tip is to make sure at least
 * one element of the observation is a constant 1.0 to provide
 * bias to the neural network approximator (don't worry if you don't
 * know what this means, just do it.)
 *
 * In an MDP setting there would typically be only one
 * element in the observation, the state-ID. This is not
 * appropriate for TemplateRLApp. Use a state basis feature
 * instead. If you want to use state-IDs for observations, edit
 * TemplateRLApp.cc to use TableLookup controllers instead of
 * NeuralNet controllers.
 *
 * @param obs The observation class to fill in and return
 */
void MySimulator::getObservation(Observation& obs) {

    // Retrieve the feature matrix from the observation.
    Matrix myObs = obs.getFeatures();

    // Check size assumptions
    assert(myObs.size1() == (size_t)getObsRows());
    assert(myObs.size2() == 1); // Single agent

    myObs.clear();

    // Fill in the observations for each row of the matrix
    // There's only one column, column 0.
    // [WRITE CODE HERE]
    // myObs(0, 0) = some value;
    // myObs(1, 0) = some value;
    // ...
    // myObs(getObsRows()-1, 0) = some value;

    // Note, don't do anything that affects the state of the 
    // simulator. You never know if getObservation() will
    // be called more than once between doAction()

}


/**
 * Do the action passed in. Again, in the single agent case the action vector
 * will always be 1x1, and contain an integer valued action.
 * You should take this action and update the state of the simulator 
 * accordingly. You can add state variables in MySimulator.hh
 * IMPORTANT: For this template I assume an infinite-horizon process.
 * This means the simulation does not terminate. If your problem
 * is finite-horizon (it has terminal state or goals), then it's your job
 * to reset the state to the initial state when you encounter a terminal 
 * state here. By editing TemplateRLApp.cc you can choose GoalPOMDP as the
 * top level algorithm appropriate for episodic cases.
 * @param action The 1x1 vector containing the action to do.
 * @return 1 for goal state (ignored in TemplateRLApp).
 */
int MySimulator::doAction(Vector& action) {

    // Check the assumption on the size of the action
    assert(action.size() == 1);
   
    int actionID = (int)action[0]; // Will generate a compiler warning until used

    // [WRITE CODE HERE]
    // Do stuff with actionID to update the state

    return 0; // This indicates goal state not encountered.
              // Always return 0 for infinite horizon.
              // This actually won't be checked by TemplateRLApp
              // Unless you change which RLAlg is used.
}



/**
 * This defines how many elements there are in your observation vector.
 * This can be as big as you want. Minimum 1 element.
 * 
 * If you in an MDP setting, you can just say getObsRows=1 and you 
 * put the state-id in that element. But see warning above.... you
 * need to edit TempateRLApp.cc to do this properly.
 */
int MySimulator::getObsRows() {
  
    int obsRows;

    // [WRITE CODE HERE] 
    // obsRows = how many elements are there per observation

    return obsRows;
}


/**
 * Return the number of actions the simulator knows about.
 * The controller will only produce actionID's between
 * 0 and getNumActions()-1
 */
int MySimulator::getNumActions() {

    int numActions;

    // [WRITE CODE HERE]
    // numActions = how many actions do you have?

    return numActions;
}



/**
 * The discount factor for this problem. If you don't know what a discount factor is
 * then read Barto and Sutton "Reinforcement Learning: An introduction", 1998.
 * Note this is not part of the standard simulator interface, but many RL people
 * expect the discount factor to be defined as part of the problem. For infinite
 * horizon settings (assumed by TemplateRLApp) this needs to be between [0,1).
 * Do not choose 1.0!
 */
double MySimulator::getDiscountFactor() {

    double discount = 0.9;

    // [WRITE CODE HERE]
    // discount =

    assert(discount >= 0.0);
    assert(discount < 1.0);
    return discount;

}


/***************************************************************
 * You don't need to edit any lower than this line for the
 * single-agent case.
 ***************************************************************/


/**
 * Get the dimensionality of the reward.
 * Hard wired to 1 for single agent case.
 * Only different if multi-agents and each
 * agent generates its own local reward.
 */
int MySimulator::getRewardDim() {		
    return 1;
}


/**
 * Get the total number of agents. Hard wired to 1 here.
 */
int MySimulator::getAgents() {
    return 1;
}


/** 
 * Maximum episode length in steps. For planning mode. Only used
 * by GOALPomdp at the moment.
 * Do not change.
 */
int MySimulator::getMaxEpisodeLength() {
    return 0;
}


/**
 * Get the number of columns in the observation matrix Typically
 * use multiple columns to provide a different observation to each
 * agent, so the number of columns is the same as getAgents()
 */
int MySimulator::getObsCols() {
    return getAgents();
}


/**
 * Get the total action dimensionality. Often, but not
 *  necessarily, the same as the number of agents.
 */
int MySimulator::getActionDim() {
    return getAgents();
}


