/*
 *  PSRCassandra.hh
 *  
 *
 *  Created by Douglas Aberdeen on 27/06/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __inc_psr_cassandra
#define __inc_psr_cassandra

#include "Cassandra.hh"

using namespace libpg;

class PSRCassandra : public Cassandra {

private:
    Vector* stateTransRow;
    Vector* observationRow;
    Vector btp1;

    // Map of PSR tests to matrix columns 
    typedef std::map<std::string, size_t> TestsMap; 
    TestsMap coreTests;
    
    // Map test to matrix of params.
    typedef std::map<std::string, Matrix> TestsParams; 
    TestsParams coreParams;
    TestsParams oneStepParams;


protected:
    Vector belief;

    void discoverPSR(); // Create MQ matrix 

    bool recursePSRDiscovery(std::string test, Matrix& U);

public:
	
    PSRCassandra (Cassandra& c);
		
    virtual int doAction (int action);
			
    virtual Vector getBelief ();
		
    virtual void getObservation (Observation& obs);
		
    virtual int getObsRows ();

};

#endif
