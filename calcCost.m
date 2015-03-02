function r = calcCost(state, input, params)
    ref = params.ref;
    r = -(state-ref)'*params.Q*(state-ref) - input*params.R*input;