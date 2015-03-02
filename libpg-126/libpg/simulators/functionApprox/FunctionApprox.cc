#include"PGBasics.hh"
#include"NeuralNetBatch.hh"
#include"RegressionController.hh"
#include"OLPomdp.hh"
#include"FunctionSim.hh"
#include"OLBFGS.hh"

using namespace std;
using namespace libpg;

#define STEP_SIZE 0.0001
#define STEPS_PER_EPOCH 10000
#define PARAMS_FILE "final_params"
#define MAX_STEPS 10000000

#define LAYERS 3
#define HIDDEN 4
#define MAX_PARM 0.1
#define MEM_LENGTH 10

void test(Controller* controller);

int main(int argc, char** argv) {


    Vector dims(LAYERS);
    dims[0] = OBSDIM;
    dims[1] = HIDDEN;
    dims[LAYERS-1] = OBSCOLS;
    Vector squash(LAYERS);
    squash.clear();
    squash[1] = 1.0; // Squash hidden units

    Controller* controller= new OLBFGS(new RegressionController(new NeuralNetBatch(dims, squash)), MEM_LENGTH);

    OLPomdp learner(controller, new FunctionSim(FunctionSim::SQR), 0.0, STEP_SIZE);
    

    if (argc > 1) {
	ifstream o(argv[1]);
	cout<<"Test mode only, reading "<<argv[1]<<endl;

	if (o.is_open()) {
	    controller->read(o);
	    o.close();
	}
	test(controller);
    }
    else {
	controller->randomizeParams(MAX_PARM);
	
	learner.learn(STEPS_PER_EPOCH, 0, MAX_STEPS);
	learner.write(PARAMS_FILE);
	
	test(controller);
    }

    return 0;

}


void test(Controller* controller) {

    ofstream o("data");

    Observation obs(OBSDIM, OBSCOLS, OBSCOLS, 1);
    Vector action(1);

    obs.getFeatures()(0,0) = 1.0;
    obs.getFeatures()(1,0) = MINX;
    //obs.getFeatures()(2,0) = 1.0;//MINX*MINX;

    while (obs.getFeatures()(1, 0) < MAXX+XINC) {
	controller->getAction(obs, action);
	o<<obs.getFeatures()(1,0)<<"\t"<<action[0]<<endl;
	obs.getFeatures()(1,0) += XINC;
	//obs.getFeatures()(2,0) = 1.0;//obs.getFeatures()(1,0)*obs.getFeatures()(1,0);
    }

    o.close();
}
