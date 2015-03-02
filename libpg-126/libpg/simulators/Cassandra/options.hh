// Overall
#define MODE "Table"  // default=Owens table, MEM=finite memory, PSR=psr, HMM, CHEAT, BELIEF, EXACT_PSR
#define PARAM_FILE "Cassandra.params" // Save parameters here.
#define PROBLEM_FILE "" // If not specified without option on command line
#define PENALTY 0.0 // Quadratic (Tikhonov) penalty term

// OLPomdp stuff
#define STEP_SIZE 0.00001 // Gradient step size, a.k.a., alpha
#define STEPS_PER_EPOCH 10000 // Steps per epoch. Has implications for baseline estimation
#define MAX_STEPS 0 // If this is >0 stop at this many steps
#define MAX_TIME 0 // If this is >0 stop after this time (seconds)

// Approximator stuff
#define HIDDEN 0 // includes input dimension and output dimension
#define MAX_RAND_PARAM 0.00 // maximum initial param value. For NN_LAYERS>2 cannot be 0

// Natural Actor-Critic stuff
#define TD_DISCOUNT 0.25 // Value estimate discount, a.k.a., gamma

// PSR stuff
#define PSR_STEP_SIZE 1.0 // PSR learning rate
#define MAX_HISTORY 1000 // Size of PSR history matrix
#define MAX_CORE_TESTS 6 // Default is counted as a core test.
#define MIN_KAPPA_FOR_CORE 10000.0 // Minumum condition number for a proposed new ccore test.
#define PASSES_BEFORE_ADDCORES 3 // Restimate history matrix this number of times before doing core test addition.

// Finite stuff (finite observation window)
#define MEM_MAX_HISTORY 2

// HMM stuff
#define HMM_MAX_HISTORY 1000 // How much history used for re-estimating model ?
#define HMM_WINDOW 0 // How much history used for estimating state ? (0 -> =HMM_MAX_HISTORY )


