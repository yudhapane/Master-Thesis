function V = critic(state, params)
    theta = transpose(params.theta)
    rbfcritic = transpose(rbf(state,params))
    V = params.theta'*rbf(state, params);          % calculate value