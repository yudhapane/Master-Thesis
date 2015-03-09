/*
 *  BeliefStateCassandra.hh
 *  
 *
 *  Created by Owen Thomas on 23/06/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __inc_belief_cassandra
#define __inc_belief_cassandra

#include "Cassandra.hh"
using namespace libpg;

class BeliefStateCassandra : public Cassandra {

	private:
		Vector* stateTransRow;
		Vector* observationRow;
		Vector* btp1;
	protected:
		Vector* belief;

	public:
	
		BeliefStateCassandra (Cassandra& c) : 
			Cassandra (c.actionTransitions, c.actionObservations,  c.rewards,
			c.numActions, c.numStates, c.numObservations, c.discount)
			
			{
			
			//init belief state -- curr state.
			belief = new Vector (numStates);
			belief->clear ();
			
			stateTransRow = new Vector (numStates);
			observationRow = new Vector (numObservations);
			btp1 = new Vector (numStates);
			//Or should this be uniform?
			(*belief) (currState) = 1.0;	
			
		}
		
	
		
		virtual int doAction (int action);
			
		virtual Vector getBelief () {
			return *(belief);
		}
		
		virtual void getObservation (Observation& obs) {
			for(int i = 0; i < (int)numStates; i++)	obs.getFeatures() (i,0) = (*belief) (i);
			obs.getFeatures() (numStates,0) = 1;
			//cout << "returning belief state: " << obs.getFeatures() << endl;
		}
		
		virtual int getObsRows () {
			return numStates + 1;
		}
};

#endif
