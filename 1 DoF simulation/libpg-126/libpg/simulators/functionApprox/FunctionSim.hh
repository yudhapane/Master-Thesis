#ifndef FunctionSim_hh
#define FunctionSim_hh
// $Id: Simulator.hh 29 2006-12-19 02:16:20Z daa $

#include<Observation.hh>
#include<Simulator.hh>

#define OBSDIM 2
#define OBSCOLS 1 

#define MINX -1.0
#define MAXX 1.0
#define XINC  0.05

/**
 * Simulator that implements supervised learning for learning mathematical
 * functions.
 */

class FunctionSim : public libpg::Simulator {

public:

    enum Function { CONST, LINE, SINE, SQR, EXP , NEG_TANH, SINC, INV, LOG}; 

protected:

    Function function;
    double x; 
    double error;

public:

    /**
     * Just tell the function simulator what class of function it's simulating
     */
    FunctionSim(Function f);

    /**
     * Shut up compiler warning
     */
    virtual ~FunctionSim() {};


    /**
     * Get the number of rows in observation matrix. Generally the
     * length of the observation vector.
     */
    virtual int getObsRows();

    /**
     * Get the number of columns in the observation matrix Typically
     * use multiple columns to provide a different observation to each
     * agent, so the number of columns is the same as getAgents()
     */
    virtual int getObsCols();

    /**
     * Get the total number of agents. Usually 1.
     */
    virtual int getAgents();

    /**
     * Get the total action dimensionality. Often, but not
     * necessarily, the same as the number of agents.
     */
    virtual int getActionDim();

    /** 
     * A dummy value 
     */
    virtual int getNumActions() { return 1; };

    /**
     * Get the dimensionality of the reward vector. Often, but not
     * necessarily, the same as the number of agents.
     */
    virtual int getRewardDim();

    /**
     * Just return the reward associated with the most recent state achieved.
     * @param  vector of rewards. The vector has getAgents() dimensionality
     * In non-local reward mode this vector will just be averaged.
     */
    virtual void getReward(libpg::Vector& rewards);

    /**
     * Return the observation of the current state. Could be a scalar,
     * vector, or matrix, so we left the type as Matrix
     * @return  possibly stochastic observation of the current state
     */
    virtual void getObservation(libpg::Observation& obs);

    /**
     * Do action issued by a controller 
     * @return  vector of actions,
     * typically just a scalar which is cast to an int, but it there
     * are multiple agents it will be a vector of actions for each
     * agent.
     * @param  vector that describes the action to perform.
     */
    virtual int     doAction(libpg::Vector& action);


};

#endif
