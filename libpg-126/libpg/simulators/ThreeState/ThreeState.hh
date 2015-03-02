// $Id$

#define STATES 3
#define ACTIONS 2
#define ACTION_DIM 1
#define OBS_DIM 2
#define REWARD_DIM 1


class ThreeState : public libpg::Simulator {

    libpg::Matrix* stateTrans; // Array of state transition matricies for
                         // each action
   
    libpg::Matrix  allObs;  // Matrix where each col describes the observation
		   // for that state.

    libpg::Vector rewards; // reward for each state
    int currState; // The current state

    int obsRows;
    int obsCols;
    int agents;
    int actionDim;
    int actions;


public:

    ThreeState();
    virtual ~ThreeState() {};
    
    virtual void    getReward(libpg::Vector& rewards);
    virtual void    getObservation(libpg::Observation& obs);
    virtual int     doAction(libpg::Vector& action);


    // Maximum episode length in steps. For planning mode. Only used
    // by GOALPomdp at the moment.
    virtual int getMaxEpisodeLength();

    // Get the number of rows in observation matrix. Generally the
    // length of the observation vector.
    virtual int getObsRows();

    // Get the number of columns in the observation matrix Typically
    // use multiple columns to provide a different observation to each
    // agent, so the number of columns is the same as getAgents()
    virtual int getObsCols();

    // Get the total number of agents. Usually 1.
    virtual int getAgents();

    // Get the total action dimensionality. Often, but not
    // necessarily, the same as the number of agents.
    virtual int getActionDim();

    /**
     * Return the number of actions 
     */
    virtual int getNumActions();

    // Get the dimensionality of the reward. Typicall 1 if there's a single
    // Global reward.
    virtual int getRewardDim();		

  
};
