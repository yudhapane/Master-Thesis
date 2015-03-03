function V = critic(state, params)
%CRITIC evaluate the value function at a certain state
%
%   V = critic(state, params) evaluates the value function of state with
%       parameters defined in params.theta
% 
% Copyright 2015 Yudha Pane
    V = params.theta'*rbf(state, params);          % calculate value