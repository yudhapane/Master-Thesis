


#include<PGBasics.hh>
#include"FunctionSim.hh"
#include<limits>
#include<math.h>

using namespace libpg;



FunctionSim::FunctionSim(Function f) {

    function = f;
    error = 0;
} 


int FunctionSim::getObsRows() {
    return OBSDIM;
}


int FunctionSim::getObsCols() {
    return OBSCOLS;
}


int FunctionSim::getAgents() {
    return OBSCOLS; 
}


int FunctionSim::getActionDim() {
    return OBSCOLS;
}


int FunctionSim::getRewardDim() {
    return OBSCOLS;
}


void FunctionSim::getReward(Vector& rewards) {
    rewards[0]=error;
}


/**
 * This will just produce a linear x axis scale, looping over the function
 * domain.
 */
void FunctionSim::getObservation(Observation& obs) {

    // Move to next point
    x = MINX + (random()/(double)RAND_MAX)*(MAXX-MINX);

    obs.getFeatures()(0,0) = 1.0;
    obs.getFeatures()(1,0) = x;
    //obs.getFeatures()(2,0) = 1.0;//x*x;
    //std::cout<<"x = "<<obs.getFeatures()<<std::endl;
   
}


/**
 * Get the suggested regressed value and compare it with reality. Compute
 * the error, which becomes the partial gradient to feed back via the
 * regressor.
 */
int FunctionSim::doAction(Vector& action) {

    double y = action[0];

    switch (function) {
    case CONST: 
	error = 0.75 - y;
	break;
    case LINE:
	error = (-0.5*x + 0.5) - y;
	break;
    case SINE: 
	error = std::sin(x*M_PI) - y;
	break;
    case SQR:
	//error = x*x - y;
	error = x*x; // just for olbfgs
	break;
    case EXP:
	error = exp(x) - y;
	break;
    case NEG_TANH:
	error = -tanh(x) - y;
	break;
    case SINC:
	error = ((x!=0)?std::sin(x*4*M_PI)/(x*4):0.0) - y;
	break;
    case INV:
	error = 1/(x+2) - y;
	break;
    case LOG:
	error = std::log(x + 1.1) - y;
	break;
    default:
	std::cout<<"Unknown function";

    }

 
    return 0;
}
