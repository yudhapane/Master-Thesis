/**
 * Welcome to libpg ThreeStateTest.cc. This file shows how to invoke most
 * of the basic algorithms supported by libpg on a simple ThreeState POMDP
 * described in the Baxter, Bartlett, Weaver JAIR paper in 2001.
 *
 * Please use this file to see how to link your simulator together
 * with your preferred algorithm.
 *
 * If you want to quickly get going, see the simulator directy "SimpleTemplate"
 *
 * Don't forget to refer to the libpg/docs/html/index.html documentation.
 */


// Main libpg header file for all libpg projects
#include"PGBasics.hh"

// Your simulator class
#include"ThreeState.hh"

// Approximators
#include"LookupTable.hh"
#include"LookupTableBatch.hh" // The batch versions are always safe to use over the non-batch
#include"NeuralNet.hh"
#include"NeuralNetBatch.hh"
#include"NACTransform.hh"

// Policies
#include"SoftmaxPolicy.hh"
#include"eGreedyPolicy.hh"

// Controllers
// Policy-gradient
#include"BasicController.hh"
#include"BinaryController.hh"
// Value based
#include"ValueController.hh"
#include"SARSAController.hh"
#include"QLearningController.hh"
#include"FactoredController.hh"
#include"LSTDQController.hh"

// RL Algorithms
#include"OLPomdp.hh"
#include"GPomdp.hh"
#include"LineSearchAlg.hh"

using namespace std;
using namespace libpg;

#define TD_DISCOUNT 0.6 // THe problem discount factor
#define STEPS_PER_EPOCH 10000
#define MAX_STEPS 10000000
#define MAX_TIME 0
#define TEST_RUNS 1
#define NN_LAYERS 2 // 2 is linear, 3 or more for MLP
#define PARAM_FILE "ThreeState.params"
#define MAX_RAND_PARAM 0.00
#define APPROX_OUTPUTS 2
#define LINE_SEARCH_STEPS 20
#define TOLERANCE 1e-9
#define BACKOFF 3.0
#define EXPONENT_BASE 2.0 
#define MAX_TRIES 16
#define DOWNHILL_THRESH 0.05


/**
 * Temp is a subclass of Temperature class, which
 * belongs to SoftmaxPolicy domain. Temp implements
 * the user specific temperature decain function.
 */
class Temp : public SoftmaxPolicy::Temperature {

public:
    
    /** 
     * Setting K to 0 makes a constant 1.0 temperature. K>0 implements a temp that
     * decays with inverse square of (steps*K + 1).
     */
    Temp(double K) {
	this->K = K;
    }
    
    /**
     * Implements the temperature decaying function.
     * @param steps number of steps done so far.
     * @return temperature value.
     */
    double getValue(int steps) {
	return (double) 1.0 / ((1.0 + K * (double)steps)*(1.0 + K * (double)steps));
    }
    
private:
    
    double K;
    
};


class EpsilonDecay : public eGreedyPolicy::EpsilonFunction {
    
private:
    double K;
    
public:
    
    EpsilonDecay(double K) {
	this->K=K;
    }
    
    double getValue(int steps) {
	return (double) 1.0 / ((1.0 + K * (double)steps)*(1.0 + K * (double)steps));
    }
    
};


#define commonInit() \
    Vector nnDims(NN_LAYERS); \
    nnDims[0] = OBS_DIM; \
    nnDims[1] = APPROX_OUTPUTS; \
    Vector squash(NN_LAYERS); \
    squash.clear();


/**
 * SARSA controller with linear function approximator and
 * Softmax policy (decaying temperature).
 * @return OLPomdp agent
 **/
OLPomdp* SARSA_NeuralNet_Softmax_Decaying() {
    commonInit();


    // With eligibility traces
    // Expected reward ~ 2.58
    double lambda =  0.4;
    double step_size = 0.0001;
    double kapa =      1e-05;

    cout << endl << "Running SARSA with NeuralNet approximator and Softmax policy with decaying temperature:" << endl << endl;
 
    Controller* controller = new SARSAController(new NeuralNet(nnDims, squash),
                                                 new SoftmaxPolicy(new Temp(kapa)),
                                                 TD_DISCOUNT);

    OLPomdp* learner = new OLPomdp(controller, new ThreeState(), lambda, step_size);



    return learner;
}



/**
 * Q values estimated in an online way by LSTDQ.
 * Softmax policy (decaying temperature).
 * @return OLPomdp agent
 **/
OLPomdp* LSTDQ_NeuralNet_Softmax_Decaying() {
    commonInit();

    // With eligibility traces
    // Expected reward ~ 2.58
    double lambda =  0.0; // Should be 0.0 for true LSTDQ
    double step_size = 0.0; // Ignored
    double kapa =      1e-05;

    cout << endl << "Running LSPI with softmax policy with decaying temperature:" << endl << endl;
 

    // Note that construction of the basis vectors for LSTDQ implicity produces an OBS_DIM*APPROX_OUTPUTS
    // vector, with all 0's except it has a copy of the observation vector at the relevant action
    // for whatever action we are approximating the value of.
    Controller* controller = new LSTDQController(new NeuralNet(OBS_DIM, APPROX_OUTPUTS),
						(Policy*) new SoftmaxPolicy(new Temp(kapa)),
						TD_DISCOUNT);

    OLPomdp* learner = new OLPomdp(controller, new ThreeState(), lambda, step_size);

    

    return learner;
}


/**
 * Least squares policy iteration. Note that this just uses LSTDQ as a
 * value estimator, and a batch GPomdp algorithm to drive the policy
 * updates.  @return OLPomdp agent
 **/
GPomdp* LSPI() {

    commonInit();

    // With eligibility traces
    // Expected reward ~ 2.58
    double lambda =  0.0; // Should be 0.0 for true LSPI
    double step_size = 0.0; // Ignored
    double epsilon =   0.05; // 0.0 = greedy

    cout << endl << "Running LSPI with softmax policy with decaying temperature:" << endl << endl;
 

    // Note that construction of the basis vectors for LSTDQ implicity produces an OBS_DIM*APPROX_OUTPUTS
    // vector, with all 0's except it has a copy of the observation vector at the relevant action
    // for whatever action we are approximating the value of.
    Controller* controller = new LSTDQController(new NeuralNet(OBS_DIM, APPROX_OUTPUTS),
						 new eGreedyPolicy(epsilon),
						TD_DISCOUNT);

    // THe only difference between online LSTDQ learning, and LSPI, is
    // the use of the GPOMDP batch algorithm in the line below and the
    // epsilon greedy policy
    GPomdp* learner = new GPomdp(controller, new ThreeState(), lambda, step_size);



    return learner;
}



/**
 * SARSA controller with linear function approximator and
 * e-Greedy policy (constant epsilon).
 * @return OLPomdp agent
 **/
OLPomdp* SARSA_NeuralNet_eGreedy_Constant() {
    commonInit();

    // Expected reward ~ 2.5
    double lambda =  0.5;
    double step_size = 0.001;
    double epsilon =   0.01;
  
    cout << endl << "Running SARSA with NeuralNet approximator and e-Greedy policy with constant epsilon:" << endl << endl;

    Controller* controller= new SARSAController(new NeuralNet(nnDims, squash),
                                                (Policy*) new eGreedyPolicy(epsilon),
                                                TD_DISCOUNT);

    OLPomdp* learner = new OLPomdp(controller, new ThreeState(), lambda, step_size);

    

    return learner;
}


/**
 * SARSA controller with linear function approximator and
 * e-Greedy policy (decaying epsilon).
 * @return OLPomdp agent
 **/
OLPomdp* SARSA_NeuralNet_eGreedy_Decaying() {
    commonInit();

    // Expected reward ~ 2.6
    double lambda =  0.7;
    double step_size = 0.001;
    double kapa =      0.001;
  
    cout << endl << "Running SARSA with NeuralNet approximator and e-Greedy policy with decaying epsilon:" << endl << endl;

    Controller* controller = new SARSAController(new NeuralNet(nnDims, squash),
                                                 (Policy*) new eGreedyPolicy(new EpsilonDecay(kapa)),
                                                 TD_DISCOUNT);

    OLPomdp* learner = new OLPomdp(controller, new ThreeState(), lambda, step_size);

    

    return learner;
}


/**
 * QLearning controller with linear function approximator and
 * Softmax policy (decaying temperature).
 * @return OLPomdp agent
 **/
OLPomdp* QLearning_NeuralNet_Softmax_Decaying() {

    commonInit();

    // Expected reward ~ 2.6
    double lambda =  0.4;
    double step_size = 0.001;
    double kapa =      1e-05;
  
    cout << endl << "Running QLearning with NeuralNet approximator and Softmax policy with decaying temperature:" << endl << endl;

    Controller* controller = new QLearningController(new NeuralNet(nnDims, squash),
                                                     (Policy*) new SoftmaxPolicy(new Temp(kapa)),
                                                     TD_DISCOUNT);

    OLPomdp* learner = new OLPomdp(controller, new ThreeState(), lambda, step_size);

    

    return learner;
}


/**
 * QLearning controller with linear function approximator and
 * e-Greedy policy (constant epsilon).
 * @return OLPomdp agent
 **/
OLPomdp* QLearning_NeuralNet_eGreedy_Constant() {
    commonInit();

    // Expected reward ~ 2.5
    double lambda =  0.5;
    double step_size = 0.001;
    double epsilon =   0.01;
  
    cout << endl << "Running QLearning with NeuralNet approximator and e-Greedy policy with constant temperature:" << endl << endl;

    Controller* controller = new QLearningController(new NeuralNet(nnDims, squash),
                                                     (Policy*) new eGreedyPolicy(epsilon),
                                                     TD_DISCOUNT);

    OLPomdp* learner = new OLPomdp(controller, new ThreeState(), lambda, step_size);

    

    return learner;
}


/**
 * QLearning controller with linear function approximator and
 * e-Greedy policy (decaying epsilon).
 * @return OLPomdp agent
 **/
OLPomdp* QLearning_NeuralNet_eGreedy_Decaying() {

    commonInit();

    // Expected reward ~ 2.6
    double lambda =  0.4;
    double step_size = 0.001;
    double kapa =      0.001;
  
    cout << endl << "Running QLearning with NeuralNet approximator and e-Greedy policy with decaying temperature:" << endl << endl;

    Controller* controller = new QLearningController(new NeuralNet(nnDims, squash),
                                                     (Policy*) new eGreedyPolicy(new EpsilonDecay(kapa)),
                                                     TD_DISCOUNT);

    OLPomdp* learner = new OLPomdp(controller, new ThreeState(), lambda, step_size);

    

    return learner;
}



/**
 * Binary controller with neural net function approximator. First policy gradient method.
 * Assumes two actions only.
 * @return OLPomdp agent
 **/
OLPomdp* Binary_NeuralNet() {
   
    // commonInit(); Not needed due to use of simple NeuralNet constructor

    // Expected reward ~ 2.6
    double lambda =  0.6;
    double step_size = 0.001;
  
    cout << endl << "Running Binary controller with NeuralNet approximator:" << endl << endl;

    // Note the simplified NeuralNet constructor for creating a linear
    // approximator. BinaryController requires it's approximator to
    // have only 1 output dimension.
    Controller* controller = new BinaryController(new NeuralNet(OBS_DIM, 1));

    OLPomdp* learner = new OLPomdp(controller, new ThreeState(), lambda, step_size);

    

    return learner;
}


/**
 * NAC Transform with Binary controller with neural net function approximator.
 * @return OLPomdp agent
 **/
OLPomdp* NACTransform_Binary_NeuralNet() {

    // commonInit();

    // Expected reward ~ 2.6
    double lambda =  0.4;
    double step_size = 0.001;
  
    cout << endl << "Running Binary controller with NeuralNet approximator:" << endl << endl;

    // Again BinaryController assumes only 2 actions. Use BasicController otherwise.
    Controller* controller= new NACTransform(new BinaryController(new NeuralNetBatch(OBS_DIM, 1)), TD_DISCOUNT);

    OLPomdp* learner = new OLPomdp(controller, new ThreeState(), lambda, step_size);

    return learner;
}

/**
 * Factored Binary controller with neural net function approximator.
 * This is an example of how to create multi-agent problems.
 * @return OLPomdp agent
 **/
OLPomdp* Factored_Binary_NeuralNet() {
    // commonInit();

    // Expected reward ~ 2.5
    double lambda =  0.8;
    double step_size = 0.001;
  
    cout << endl << "Running Binary controller with NeuralNet approximator:" << endl << endl;

    // Create one controller per agent.
    FactoredController::Controllers controllers;
    controllers.push_back(new BinaryController(new NeuralNet(OBS_DIM, 1)));

    OLPomdp* learner = new OLPomdp(new FactoredController(controllers, false, false), new ThreeState(), lambda, step_size);

    

    return learner;
}



/**
 * If a command line parameter is provided it is treated as the id number
 * of the algorithm to run. Otherwise we loop through all algorithms.
 */
int main(int argc, char** argv) {

    RLAlg* learner = NULL;
    time_t startTime = time(NULL);
 

    if (argc > 1) {

        switch (atoi(argv[1])) {

            case 1:
                learner = SARSA_NeuralNet_Softmax_Decaying();
                break;

            case 2:
                learner = SARSA_NeuralNet_eGreedy_Constant();
                break;

            case 3:
                learner = SARSA_NeuralNet_eGreedy_Decaying();
                break;

            case 4:
                learner = QLearning_NeuralNet_Softmax_Decaying();
                break;

            case 5:
                learner = QLearning_NeuralNet_eGreedy_Constant();
                break;

            case 6:
                learner = QLearning_NeuralNet_eGreedy_Decaying();
                break;

	    case 7:
		learner = Binary_NeuralNet();
		break;

	    case 8:
		learner = NACTransform_Binary_NeuralNet();
		break;

	    case 9:
		learner = Factored_Binary_NeuralNet();
		break;

  	    case 10:
		learner = LSTDQ_NeuralNet_Softmax_Decaying();
		break;

 	    case 11:
		learner = LSPI();
		break;

            default:
		cout << "Invalid option!" << endl;
                break;
        }
	
	if (learner != NULL) {
            learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
            delete learner;  
	}
    }
    else {
        learner = SARSA_NeuralNet_Softmax_Decaying();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
	delete learner;

        learner = SARSA_NeuralNet_eGreedy_Constant();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
	delete learner;

        learner = SARSA_NeuralNet_eGreedy_Decaying();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
	delete learner;

        learner = QLearning_NeuralNet_Softmax_Decaying();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
	delete learner;

        learner = QLearning_NeuralNet_eGreedy_Constant();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
	delete learner;

        learner = QLearning_NeuralNet_eGreedy_Decaying();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
	delete learner;

	learner = LSTDQ_NeuralNet_Softmax_Decaying();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
	delete learner;

	learner = LSPI();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
	delete learner;

        learner = Binary_NeuralNet();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
        delete learner;

        learner = NACTransform_Binary_NeuralNet();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
        delete learner;
	
        learner = Factored_Binary_NeuralNet();
        learner->learn(STEPS_PER_EPOCH, MAX_TIME, MAX_STEPS);
        delete learner;
    }

    cout<<"Took "<<time(NULL) - startTime<<" secs\n";

    return 0;
}
