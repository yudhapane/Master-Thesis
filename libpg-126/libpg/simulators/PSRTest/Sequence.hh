// $Id$

#define SYMBOLS 4
#define ACTIONS SYMBOLS
#define ACTION_DIM 1
#define OBS_DIM 1
#define REWARD_DIM 1
#define APPROX_OUTPUTS ACTIONS

using namespace libpg;

class Sequence : public Simulator {

    static int sequence[];
    static int seqLen;
    double reward;

    int actionDim;
    int obsRows;
    int obsCols;
    int actions;
    int agents;

    int nextObs;

public:

    Sequence();
    virtual ~Sequence() {};
    
    virtual void    getReward(Vector& rewards);
    virtual void    getObservation(Observation& obs);
    virtual int     doAction(Vector& action);


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


    // Get the dimensionality of the reward. Typically 1 if there's a single
    // Global reward.
    virtual int getRewardDim();		

};
