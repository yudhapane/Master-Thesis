// This is your first RL application. You shouldn't need to edit this
// file until you have MySimulator.cc and MySimlator.hh complete and
// the code compiles and runs. Then you should consider modifying this
// file for best performance and using alternative algorithms. 

// The first thing to consider changing is the algorithm used. This
// file defaults to Natural Actor-Critic. You can also switch to
// Least Squares Policy Iteration. These are both state of the art
// algorithms in RL. See documentation for other altgorithms and
// advanced things to do with TransformControllers.

// The next thing you can do is twiddle the values of the #defines
// for better performance. Note that this code also shows how to
// save parameter files so that you can recover your optimised
// policy.

#include"PGBasics.hh"

// Your simulator
#include"MySimulator.hh"

// Approximators
#include"NeuralNetBatch.hh"
#include"LookupTableBatch.hh" // Use for exact MDPs

// Controllers
#include"BasicController.hh"
#include"NACTransform.hh"
#include"eGreedyPolicy.hh"
#include"LSTDQController.hh"

// RLAlg
#include"OLPomdp.hh"

/**********************************************************
 * Hard coded settings you can change
 **********************************************************/

#define NAC 0
#define LSPI 1
#define ALGORITHM NAC // 0 for Natural actor critic, 1 for LSPI
                      // If you change this, change LAMBDA below.

#define MAX_STEPS 0 // Total max optimisation steps, 0=unbounded
#define MAX_TIME 0 // Total max wall clock time, 0=unbounded
#define EPOCH_STEPS 1000 // Steps per epoch (progress print outs/LSTDQ steps)
                         // Increase this if youre getting too much information.
                         // Decrease this if things are very slow, but not
                         // too small for LSPI otherwise it will
                         // increase variance.
                       

// Location of parameter save file
#define PARAMETER_FILE "parameters.txt"

#define LAMBDA 0.8 // For NAC, start with 0.8. Always stay less than
                   // 1.0 Increase slowly only if you're not getting a
                   // good solution. If you're getting no improvement
                   // at all start by increasing the step size below.
                   // For LSPI, start with 0.0 and try values up to an
                   // including 1.0

// LSPI Specific settings
#define EXPLORATION_PROB 0.1 // Probability of a random action

// Natural actor-critic specific settings
#define STEP_SIZE 0.00001 // Start small and grow by 10x until things
                          // become unstable during optimisation. If
                          // nothing at all seems to change, this value
                          // might need to be much bigger.
                          // If things improve quickly at the start, then
                          // the reward suddenly gets much worse, make
                          // this smaller.




using namespace std;
using namespace libpg;


RLAlg* createRLAlg() {
    
    // STEP 1. Create simulator
    // [WRITE CODE HERE] Insert parameters into constructor if needed.
    MySimulator* mySimulator = new MySimulator();

    // STEP 2. Create linear approximator (see documentation to change this to a multi-layer perceptron)
    Approximator* neuralNet = new NeuralNetBatch(mySimulator->getObsRows(), mySimulator->getNumActions());
    
    // STEP 3. Create a controller (implement different algorithms here)
    Controller* controller;
    if (ALGORITHM == NAC) {
      // Create Natural Actor-Critic controller
      controller = new NACTransform(new BasicController(neuralNet), mySimulator->getDiscountFactor());  
    }
    else {
      // Create alternative LSPI controller
      controller = new LSTDQController(neuralNet, 
				       new eGreedyPolicy(EXPLORATION_PROB),
				       mySimulator->getDiscountFactor());
    }
      
    // STEP 4. Link all together with an algorithm
    RLAlg* alg = new OLPomdp(controller, mySimulator, LAMBDA, STEP_SIZE);

    return alg;

}


void optimise() {

    RLAlg* alg = createRLAlg();

    // Indicate to automatically save the parameters whenever performance improves
    alg->saveBest(PARAMETER_FILE);

    // STEP 5. Learn!
    alg->learn(EPOCH_STEPS, MAX_TIME, MAX_STEPS);

    // Will exit when MAX_TIME or MAX_STEPS is exceeded.
}


void evaluate(char* oldParametersFile) {

    RLAlg* alg = createRLAlg();
    
    alg->read(oldParametersFile);
    alg->evaluateAndPrint(EPOCH_STEPS, MAX_STEPS);

    // Will return when MAX_STEPS is reached

}


/**
 * Main routine to switch between evaluation and optimisation
 */
int main(int argc, char** argv) {

    if (argc == 1) {
	// No command line parameters. Do optimisation
	optimise();
    }
    else {
	// Evaluate a previous set of saved parameters
	evaluate(argv[1]);
    }

    exit(EXIT_SUCCESS);

}


