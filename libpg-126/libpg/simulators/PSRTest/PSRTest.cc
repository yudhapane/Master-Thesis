#include"PGBasics.hh"
#include"Sequence.hh"
#include"LookupTable.hh"
#include"LookupTableBatch.hh"
#include"NeuralNet.hh"
#include"NeuralNetBatch.hh"
#include"PSRTransform.hh"
#include"BasicController.hh"    

#include"BinaryController.hh"
#include"OLPomdp.hh"

using namespace std;
using namespace libpg;

#define DISCOUNT 0.90
#define TD_DISCOUNT 0.9
#define PSR_STEP_SIZE 0.25
#define STEP_SIZE 0.001
#define STEPS_PER_EPOCH 100
#define MAX_STEPS 0
#define MAX_TIME 0
#define TEST_RUNS 1
#define NN_LAYERS 2 // includes input dimension and output dimension
#define PARAM_FILE "PSRTest.params"
#define MAX_RAND_PARAM 0.00
#define MAX_HISTORY 1000
#define MAX_CORE_TESTS 10
#define MIN_KAPPA_FOR_CORE 20.0
#define PASSES_BEFORE_ADDCORES 5

int main(int argc, char** argv) {

	
    Vector nnDims(NN_LAYERS);
    nnDims[0] = MAX_CORE_TESTS;
    nnDims[1] = APPROX_OUTPUTS;
    Vector squash(NN_LAYERS); 
    squash.clear();

    int stepsPerEpoch = STEPS_PER_EPOCH;

    if (argc > 1) {
	stepsPerEpoch = atoi(argv[1]);
	//srandom(seed);
	//cout<<"Seed "<<seed<<endl;
    }


    // Create Cassandra with file from command line.
    Sequence* pomdp = new Sequence();
    
    // Test standard single agent case
    Controller* controller= new PSRTransform(new BasicController(new NeuralNet(nnDims, squash)), MAX_HISTORY, SYMBOLS, ACTIONS, PSR_STEP_SIZE, PASSES_BEFORE_ADDCORES, MIN_KAPPA_FOR_CORE);
    
    OLPomdp learner(controller, pomdp, DISCOUNT, STEP_SIZE);
    

    learner.saveBest(PARAM_FILE);
    learner.setUseAutoBaseline(true);

    time_t startTime = time(NULL);
    for (int i=0; i < TEST_RUNS; i++) {
	controller->randomizeParams(MAX_RAND_PARAM);    	
	learner.learn(stepsPerEpoch, MAX_TIME, MAX_STEPS);
    }
    cout<<"Took "<<time(NULL) - startTime<<" secs\n";

    return 0;

}
