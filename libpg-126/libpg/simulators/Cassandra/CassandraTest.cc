#include"PGBasics.hh"

#include"LookupTable.hh"
#include"LookupTableBatch.hh"
#include"NeuralNet.hh"
#include"NeuralNetBatch.hh"
#include"BasicController.hh"    
#include"PSRTransform.hh"
#include"NACTransform.hh"
#include"CatObsActionTransform.hh"
#include"ObsWindowTransform.hh"
#include"MultidimensionalLookupTable.hh"
#include"HMMTransform.hh"
#include"BinaryController.hh"
#include"OLPomdp.hh"
#include"RegularizeTransform.hh"

#include"Ps.hh"

#include "CassandraReader.hh"
#include "BeliefStateCassandra.hh"
#include "BlindCassandra.hh"
#include "PSRCassandra.hh"

#define MLP_LAYERS 3
#define LINEAR_LAYERS 2
#define STR_BUF_LEN 1024

using namespace std;
using namespace libpg;

int CassandraTest(int argc, char** argv);

/*
int main (int argc, char** argv) {

    try {
	CassandraTest(argc, argv);
    }
    catch (exception& e) {
	cout<<e.what();
	return 1;
    }

    return 0;

}
*/

int main(int argc, char** argv) {
    
    // Allow command line options
    int optind = Ps::processOpts(argc, argv);

    char hostname[STR_BUF_LEN];
    gethostname(hostname, STR_BUF_LEN);
    cout<<"Running on '"<<hostname<<"'\n";

    // Dump options to standard output
    Ps::print(cout);
    
    if (optind < argc) Ps::pPROBLEM_FILE = strdup(argv[optind]);
    if (strlen(Ps::pPROBLEM_FILE) == 0) throw std::runtime_error("Need a .POMDP file either in --PROBLEM_FILE or end/beginning of command line");

    cout<<"Running problem '"<<Ps::pPROBLEM_FILE<<"'\n";
    ifstream file (Ps::pPROBLEM_FILE);
    if (!file.is_open()) throw std::runtime_error("Problem opening file");

    // Create Cassandra with file from command line.
    Cassandra* pomdp = readCassandra (file);
	
    pomdp->print();
    
	srandom (time(NULL));
	srand (time(NULL));
	
    Controller* controller;

    // Modes govern the structure of controllers
    // There should be a mode for:
    // Table (default)
    // Finite (Olivier's finite history)
    // HMM
    // PSR
    // Belief
    if (strcmp(Ps::pMODE, "PSR")==0) {
	cout<<"Setting up PSR controller\n";
	
	Vector nnDims(Ps::pHIDDEN?MLP_LAYERS:LINEAR_LAYERS);
	Vector squash(Ps::pHIDDEN?MLP_LAYERS:LINEAR_LAYERS); squash.clear();
	
	if (Ps::pHIDDEN > 0) {
	    if (Ps::pMAX_RAND_PARAM == 0.0) throw std::runtime_error("MAX_RAND_PARAMS=0");
	    nnDims[0] = Ps::pMAX_CORE_TESTS;
	    nnDims[1] = Ps::pHIDDEN;
	    nnDims[2] = pomdp->getNumActions();
	    squash[1] = 1.0;
	}
	else {
	    nnDims[0] = Ps::pMAX_CORE_TESTS;
	    nnDims[1] = pomdp->getNumActions();
	}
	
	controller=new RegularizeTransform(new PSRTransform(new NACTransform(new BasicController(new NeuralNetBatch(nnDims, 
														    squash
														    )
												 ), Ps::pTD_DISCOUNT
									     ), 
							    
							    Ps::pMAX_HISTORY, 
							    pomdp->getNumObs(),
							    pomdp->getNumActions(), 
							    Ps::pPSR_STEP_SIZE, 
							    Ps::pPASSES_BEFORE_ADDCORES, 
							    Ps::pMIN_KAPPA_FOR_CORE),
					   Ps::pPENALTY);
    }
    else if (strcmp(Ps::pMODE, "MEM")==0) {
	cout<<"Setting up Finite (observation window) controller\n";
	
	controller= 
	    new RegularizeTransform(new CatObsActionTransform(new ObsWindowTransform(new BasicController(new MultidimensionalLookupTable(
																	 pomdp->getNumObs() > pomdp->getNumActions() ?
																	 pomdp->getNumObs() : pomdp->getNumActions(),
																	 pomdp->getNumActions(),
																	 Ps::pMEM_MAX_HISTORY*2
																	 )
													 ),
										     Ps::pMEM_MAX_HISTORY, 2, 1, 1),
							      1,1,1,1),
				    Ps::pPENALTY);
				    
    }
    else if (strcmp(Ps::pMODE, "HMM")==0) {
	cout<<"Setting up HMM controller\n";
	
	Vector nnDims(Ps::pHIDDEN?MLP_LAYERS:LINEAR_LAYERS);
	Vector squash(Ps::pHIDDEN?MLP_LAYERS:LINEAR_LAYERS); squash.clear();
	
	if (Ps::pHIDDEN > 0) {
	    if (Ps::pMAX_RAND_PARAM == 0.0) throw std::runtime_error("MAX_RAND_PARAMS=0");
	    nnDims[0] = pomdp->getNumStates();
	    nnDims[1] = Ps::pHIDDEN;
	    nnDims[2] = pomdp->getNumActions();
	    squash[1] = 1.0;
	}
	else {
	    nnDims[0] = pomdp->getNumStates();
	    nnDims[1] = pomdp->getNumActions();
	}

	size_t window;
	window = (Ps::pHMM_WINDOW == 0) ?
	    Ps::pHMM_MAX_HISTORY : Ps::pHMM_WINDOW;
	
	controller= 
	    new RegularizeTransform(new HMMTransform(new NACTransform(new BasicController(new NeuralNetBatch(nnDims, 
													     squash
													     )
											  ), Ps::pTD_DISCOUNT
								      ), 
						     Ps::pHMM_MAX_HISTORY, window,
						     pomdp->getNumStates(),
						     pomdp->getNumObs(),
						     pomdp->getNumActions()),
				    Ps::pPENALTY);
    }
    // Test standard single agent case
    else if (strcmp(Ps::pMODE, "CHEAT")==0) {
	pomdp->setCheat (true);
	controller= new RegularizeTransform(new BasicController(new LookupTable(pomdp->getNumObs(), pomdp->getNumActions())),
					    Ps::pPENALTY
					    );
    }
    else if (strcmp(Ps::pMODE, "BELIEF")==0) {
	cout<<"Belief mode\n";
	pomdp = new BeliefStateCassandra (*pomdp);
		
	Vector nnDims(Ps::pHIDDEN?MLP_LAYERS:LINEAR_LAYERS);
	Vector squash(Ps::pHIDDEN?MLP_LAYERS:LINEAR_LAYERS); squash.clear();
	
	if (Ps::pHIDDEN > 0) {
	    if (Ps::pMAX_RAND_PARAM == 0.0) throw std::runtime_error("MAX_RAND_PARAMS=0");
	    nnDims[0] = pomdp->getObsRows();
	    nnDims[1] = Ps::pHIDDEN;
	    nnDims[2] = pomdp->getNumActions();
	    squash[1] = 1.0;
	}
	else {
	    nnDims[0] = pomdp->getObsRows();
	    nnDims[1] = pomdp->getNumActions();
	}

 
	controller = new RegularizeTransform(new NACTransform(new BasicController (new NeuralNetBatch(nnDims, squash)), Ps::pTD_DISCOUNT) ,Ps::pPENALTY);
		
    }
    else if (strcmp(Ps::pMODE, "EXACT_PSR")==0) {
	cout<<"Exact PSR\n";
	pomdp = new PSRCassandra (*pomdp);
		
	Vector nnDims(Ps::pHIDDEN?MLP_LAYERS:LINEAR_LAYERS);
	Vector squash(Ps::pHIDDEN?MLP_LAYERS:LINEAR_LAYERS); squash.clear();
	
	if (Ps::pHIDDEN > 0) {
	    if (Ps::pMAX_RAND_PARAM == 0.0) throw std::runtime_error("MAX_RAND_PARAMS=0");
	    nnDims[0] = pomdp->getObsRows();
	    nnDims[1] = Ps::pHIDDEN;
	    nnDims[2] = pomdp->getNumActions();
	    squash[1] = 1.0;
	}
	else {
	    nnDims[0] = pomdp->getObsRows();
	    nnDims[1] = pomdp->getNumActions();
	}

	controller = new RegularizeTransform(new NACTransform(new BasicController (new NeuralNetBatch(nnDims, squash)), Ps::pTD_DISCOUNT), Ps::pPENALTY);
		
    }
    else if (strcmp(Ps::pMODE, "BLIND")==0) {
	cout << "blind" << endl;
	pomdp = new BlindCassandra (*pomdp);
	controller = new BasicController (new LookupTable (pomdp->getNumObs (), pomdp->getNumActions ()));
		
		
    }
    // Test standard single agent case
    else controller= new RegularizeTransform(new BasicController(new LookupTable(pomdp->getNumObs(), pomdp->getNumActions())), 
					     Ps::pPENALTY);
	

    
    OLPomdp learner(controller, pomdp, pomdp->getDiscount(), Ps::pSTEP_SIZE);
    

    learner.saveBest(Ps::pPARAM_FILE);
    learner.setUseAutoBaseline(true);

    time_t startTime = time(NULL);
	
    controller->randomizeParams(Ps::pMAX_RAND_PARAM); 
    learner.learn(Ps::pSTEPS_PER_EPOCH, Ps::pMAX_TIME, Ps::pMAX_STEPS);
    cout<<"Took "<<time(NULL) - startTime<<" secs\n";
	
    return 0;

}
