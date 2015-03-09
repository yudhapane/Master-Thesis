/*
 *  BlindCassandra.h
 *  
 *
 *  Created by Owen Thomas on 26/06/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */
#include "Cassandra.hh"
using namespace libpg;

/**
 * Cassandra simulator that produces no useful observations, forcing controller
 * to find best stationary policy
 */
class BlindCassandra : public Cassandra {
    
public:
    BlindCassandra (Cassandra& c) : 
	Cassandra (c.actionTransitions, c.actionObservations,  c.rewards,
		   c.numActions, c.numStates, c.numObservations, c.discount)
    {}
    
    virtual void getObservation (Observation& obs) {
	obs.getFeatures() (0,0) = 0;
    }
    
    
    virtual int  getNumObs () {return 1; }

};


